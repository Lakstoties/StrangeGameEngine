//GLEW goes before GLFW3 because GLFW inlcudes GL.h
#include "GL\glew.h"

#include <GLFW\glfw3.h>
#include <thread>
#include <mutex>
#include "include\SGE\display.h"
#include "sharedinternal.h"



namespace SGE
{
	namespace Display
	{
		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to VRAM
		std::mutex refreshHold;

		//The virtual video horizontal resolution
		int ResolutionX = 0;

		//The virtual video vertical resolution
		int ResolutionY = 0;

		//The framebuffer horizontal resolution within the OS Window
		int FrameBufferX = 0;

		//The framebuffer vertical resoltuion within the OS Window
		int FrameBufferY = 0;

		//Flag to indicate the framebuffer window size has changed
		bool FrameBufferChanged = false;

		//The viewpoint window (within the framebuffer window) vertical resolution
		int ViewPortWindowX = 0;

		//The viewpoint window (within the framebuffer window) horizontal resolution
		int ViewPortWindowY = 0;

		//The viewpoint window (within the framebuffer window) X offset
		int ViewPortWindowOffsetX = 0;

		//The viewpoint window (within the framebuffer window) Y offset
		int ViewPortWindowOffsetY = 0;

		//Internal flag to signal if a game display is presently open
		bool GameDisplayOpen = false;

		//Flag to indicate the game resolution has changed
		//Start as true to set up the initial texture
		bool GameResolutionChanged = true;


		//The virtual video RAM.  Public accessible to allow other components to write to it directly.
		unsigned int* VideoRAM = nullptr;

		//The virtual video back buffer.
		unsigned int* VideoBackBuffer = nullptr;

		//The virtual row buffer
		unsigned int* VideoRowBuffer = nullptr;

		//The virtual video RAM Size.
		unsigned int VideoRAMSize = 0;

		//Virtual Display Scaling between intended resolution and actual.
		float VirtualDisplayScaling = 1.0f;

		//C++ thread pointer to keep track of the spawned drawing thread.
		std::thread* drawingThread = nullptr;

		//Boolean used to control whether the draw thread keeps on running or not.
		bool continueDrawing = true;

		//Main update thread to take what's in video RAM and dump it on the screen.
		void UpdateThread()
		{
			//Handle to call the OpenGL texture we are using
			GLuint textureHandle = 0;

			//Check to see if we have a valid window to draw to or if the window is closing down
			if (glfwWindowShouldClose(SGE::mainWindow))
			{
				//If the window is closing, we ain't doing anymore drawing.
				continueDrawing = false;

				//We're out!
				return;
			}

			//Snag the Open context for the main window
			glfwMakeContextCurrent(SGE::mainWindow);

			//Initialize GLEW so it can find all the bits and pieces it needs.
			glewInit();


			//
			//Do some initialization bits
			//

			//Enable 2D Texturing
			glEnable(GL_TEXTURE_2D);

			//Disable Alpha Blending
			glDisable(GL_BLEND);

			//Generate the texture space and pass the handle off
			glGenTextures(1, &textureHandle);

			//
			//Manage updating the texture that serves as our "buffer"
			//

			//Bind a 2D texture to the texture handle
			glBindTexture(GL_TEXTURE_2D, textureHandle);

			//Turn off Mip map filtering and all that jazz
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			//Adjust Texture magnification
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			//Since there is only one texture, we never need to worry about rebinding textures.

			//Experimental
			GLuint pixelBufferObject;

			glGenBuffers(1, &pixelBufferObject);

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBufferObject);

			char* pixelBufferMapping = nullptr;


			//Do some drawing.
			while (continueDrawing)
			{
				//
				//  OpenGL Window sizing, scaling, and centering!
				//

				//Check to see if this stuff has changed from previous
				if (SGE::Display::FrameBufferChanged)
				{
					//Got it, reset the flag
					SGE::Display::FrameBufferChanged = false;

					//Welp, shit has changed!  Set new the Viewport
					glViewport(
						//Center it in the middle of the X axis
						SGE::Display::ViewPortWindowOffsetX,
						//Set the Y to the origin
						SGE::Display::ViewPortWindowOffsetY,
						//Scale the width based on the height and aspect ratio
						SGE::Display::ViewPortWindowX,
						//Set the height to the frameBufferHeight
						SGE::Display::ViewPortWindowY);
				}

				//Lock the refresh mutex
				//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
				refreshHold.lock();

				//Upload the data for the texture to the actual video card.
				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//glBufferData(GL_PIXEL_UNPACK_BUFFER, VideoRAMSize * sizeof(unsigned int), 0, GL_STREAM_DRAW);
					glBufferStorage(GL_PIXEL_UNPACK_BUFFER, VideoRAMSize * sizeof(unsigned int), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

					//char* tempPTR = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
					pixelBufferMapping = (char*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, VideoRAMSize * sizeof(unsigned int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

					//memcpy(tempPTR, VideoRAM, VideoRAMSize * sizeof(unsigned int));
					memcpy(pixelBufferMapping, VideoRAM, VideoRAMSize * sizeof(unsigned int));

					//glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

					//Yes, this method is technically slower, but there is ONLY one texture in the entire program and no additial code is required to handle resolution changes
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ResolutionX, ResolutionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				
					//Set the flag back to normal
					GameResolutionChanged = false;
				}
				//Otherwise just update it
				else
				{
					//char* tempPTR = (char*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

					//memcpy(tempPTR, VideoRAM, VideoRAMSize * sizeof(unsigned int));
					memcpy(pixelBufferMapping, VideoRAM, VideoRAMSize * sizeof(unsigned int));

					//glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);


					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, ResolutionX, ResolutionY, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				}

				//Unlock the refresh mutex
				refreshHold.unlock();

				//
				//  Start drawing the textured quad
				//

				//Clear the color buffers
				glClear(GL_COLOR_BUFFER_BIT);

				//Draw a quad
				glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 1.0f);  	glVertex3f(-1.0f, -1.0f, 0.0f);		//Bottom Left
				glTexCoord2f(1.0f, 1.0f); 	glVertex3f(1.0f, -1.0f, 0.0f);		//Bottom Right
				glTexCoord2f(1.0f, 0.0f); 	glVertex3f(1.0f, 1.0f, 0.0f);		//Top Right
				glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, 0.0f);		//Top Left

				//Done Drawing that
				glEnd();


				//Check to see if we have a valid place to update to
				//Or if the window should be closed.
				if (!glfwWindowShouldClose(SGE::mainWindow))
				{
					//Display the new shit after we are done drawing it
					glfwSwapBuffers(SGE::mainWindow);
				}

				//We don't have a window to swap buffers to
				//Shut it down
				else
				{
					//Done drawing this shit.
					continueDrawing = false;

					//We're out!
					return;
				}

				//Frame Rate Limiter Section
				//This is simple millisecond sleep timer to wait until making another drawing attempt
				std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_FRAME_WAIT_MILLISECONDS));
			}
		}

		//Open the Virtual Display Window
		void Open(int newVideoX, int newVideoY)
		{
			if (!GameDisplayOpen)
			{
				ResolutionX = newVideoX;
				ResolutionY = newVideoY;
				VideoRAMSize = ResolutionX * ResolutionY;

				//Initialize the Virtual Video RAM
				VideoRAM = new unsigned int[VideoRAMSize];

				//Initialize the Virtual Video Back Buffer
				VideoBackBuffer = new unsigned int[VideoRAMSize];

				//Initialize the Virtual Row Buffer
				VideoRowBuffer = new unsigned int[ResolutionX];

				//Flag that the game display is open
				GameDisplayOpen = true;
			}
		}

		//Change the game resolution and all appropriate bits.
		void ChangeGameResolution(int width, int height)
		{
			//Check for some kind of valid, non-zero resolution.
			//Otherwise, return.  Fuck this zero and less bullshit.
			if (width < 1 || height < 1)
				return;

			if (GameDisplayOpen)
			{
				//Hold the refresh
				//This way we don't confuse the display loop with weird memory glitches
				//Also this should halt any render operations from other game threads, until we are done.
				refreshHold.lock();

				//Delete the old RAM bits
				delete VideoRAM;
				delete VideoBackBuffer;
				delete VideoRowBuffer;

				//Set the new resolution values
				ResolutionX = width;
				ResolutionY = height;
				VideoRAMSize = ResolutionX * ResolutionY;

				//
				//Make some new RAM bits!
				//

				//Initialize the Virtual Video RAM
				VideoRAM = new unsigned int[VideoRAMSize];

				//Initialize the Virtual Video Back Buffer
				VideoBackBuffer = new unsigned int[VideoRAMSize];

				//Initialize the Virtual Row Buffer
				VideoRowBuffer = new unsigned int[ResolutionX];

				//Flag the game resolution has changed
				GameResolutionChanged = true;

				//Release the refresh
				refreshHold.unlock();
			}
		}

		void Close()
		{
			if (GameDisplayOpen)
			{
				StopDrawing();

				//Make sure created bits are deleted appropriately.
				delete drawingThread;
				delete VideoRAM;
				delete VideoBackBuffer;
				delete VideoRowBuffer;

				//Set that the game display is not open
				GameDisplayOpen = false;
			}
		}

		void StartDrawing()
		{
			//Flag the thread to start drawiong
			continueDrawing = true;

			//Launch that thread
			drawingThread = new std::thread(&Display::UpdateThread);
		}

		void StopDrawing()
		{
			//Signal it to stop.
			continueDrawing = false;

			//Wait for it to join back up
			if (drawingThread->joinable())
				drawingThread->join();
		}

		void BlockRefresh()
		{
			refreshHold.lock();
		}

		void AllowRefresh()
		{
			refreshHold.unlock();
		}

		void SetWindowSize(int width, int height)
		{
			//Check to make sure the window size is not negative or 0
			if (width < 1 || height < 1)
				return;
			
			//Adjust the window size
			glfwSetWindowSize(mainWindow, width, height);
		}
	}
}