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
		namespace Video
		{
			//The virtual video RAM.  Public accessible to allow other components to write to it directly.
			unsigned int* RAM = nullptr;

			//The virtual video back buffer.
			unsigned int* BackBuffer = nullptr;

			//The virtual row buffer
			unsigned int* RowBuffer = nullptr;

			//The virtual video RAM Size.
			unsigned int RAMSize = 0;

			//The virtual video horizontal resolution
			int ResolutionX = 0;

			//The virtual video vertical resolution
			int ResolutionY = 0;
		}


		//Virtual Display Scaling between intended resolution and actual.
		float VirtualDisplayScaling = 1.0f;


		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to VRAM
		std::mutex refreshHold;

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




		//C++ thread pointer to keep track of the spawned drawing thread.
		std::thread* drawingThread = nullptr;

		//Boolean used to control whether the draw thread keeps on running or not.
		bool continueDrawing = true;


		//Recalculate View Port Dimensions
		void RecalculateViewport()
		{
			//Update the viewport sizes and offsets
			//Calculate offsets
			SGE::Display::ViewPortWindowOffsetX = (SGE::Display::FrameBufferX - (SGE::Display::FrameBufferY * Video::ResolutionX) / Video::ResolutionY) >> 1;
			SGE::Display::ViewPortWindowOffsetY = (SGE::Display::FrameBufferY - (SGE::Display::FrameBufferX * Video::ResolutionY) / Video::ResolutionX) >> 1;

			//Short circuit logic
			//If the Offset goes negative it needs to be hard capped or it throws off calculations.
			(SGE::Display::ViewPortWindowOffsetX < 0) && (SGE::Display::ViewPortWindowOffsetX = 0);
			(SGE::Display::ViewPortWindowOffsetY < 0) && (SGE::Display::ViewPortWindowOffsetY = 0);

			//Calculate viewport
			//Take the framebuffer and subtract double the viewport offsets to scale appropriately for the window
			SGE::Display::ViewPortWindowX = SGE::Display::FrameBufferX - (SGE::Display::ViewPortWindowOffsetX << 1);
			SGE::Display::ViewPortWindowY = SGE::Display::FrameBufferY - (SGE::Display::ViewPortWindowOffsetY << 1);
		}

		void OpenGL20DrawFunction()
		{
			//Initialize for drawing

			//Create a simple handle for the Pixel Buffer Object
			GLuint pixelBufferObject;

			//Generate a Buffer
			glGenBuffers(1, &pixelBufferObject);

			//Bind the buffer to our context
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBufferObject);

			//Do some drawing.
			while (continueDrawing)
			{
				//
				//  OpenGL Window sizing, scaling, and centering!
				//

				//Check to see if this stuff has changed from previous
				if (SGE::Display::FrameBufferChanged)
				{
					RecalculateViewport();

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

					//Got it, reset the flag
					SGE::Display::FrameBufferChanged = false;
				}

				//Lock the refresh mutex
				//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
				refreshHold.lock();

				//Upload the data for the texture to the actual video card.
				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//Recreate the Buffer storage for the new video ram size
					//glBufferStorage(GL_PIXEL_UNPACK_BUFFER, VideoRAMSize * sizeof(unsigned int), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
					glBufferData(GL_PIXEL_UNPACK_BUFFER, Video::RAMSize * sizeof(unsigned int), Video::RAM, GL_DYNAMIC_DRAW);

					//Move load up the texture data from the buffer
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::ResolutionX, Video::ResolutionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

					//Set the flag back to normal
					GameResolutionChanged = false;

					//Flag the viewport to change to properly scale the new resolution
					FrameBufferChanged = true;
				}
				//Otherwise just update it
				else
				{
					glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, Video::RAMSize * sizeof(unsigned int), Video::RAM);

					//Move the data to the texture from the buffer
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::ResolutionX, Video::ResolutionY, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

		void OpenGL44DrawFunction()
		{
			//Initialize for drawing

			//Experimental
			//Create a simple handle for the Pixel Buffer Object
			GLuint pixelBufferObject;

			//Generate a Buffer
			glGenBuffers(1, &pixelBufferObject);

			//Bind the buffer to our context
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBufferObject);

			//Create a pointer to map to the memory OpenGL will grant us
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
					RecalculateViewport();

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

					//Got it, reset the flag
					SGE::Display::FrameBufferChanged = false;
				}

				//Lock the refresh mutex
				//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
				refreshHold.lock();

				//Upload the data for the texture to the actual video card.
				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//Recreate the Buffer storage for the new video ram size
					glBufferStorage(GL_PIXEL_UNPACK_BUFFER, Video::RAMSize * sizeof(unsigned int), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

					//Grab the pinter to the mapped buffer range
					pixelBufferMapping = (char*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, Video::RAMSize * sizeof(unsigned int), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

					//Copy our data to it
					memcpy(pixelBufferMapping, Video::RAM, Video::RAMSize * sizeof(unsigned int));

					//Move load up the texture data from the buffer
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::ResolutionX, Video::ResolutionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

					//Set the flag back to normal
					GameResolutionChanged = false;

					//Flag the viewport to change to properly scale the new resolution
					FrameBufferChanged = true;
				}
				//Otherwise just update it
				else
				{
					//Update the data
					memcpy(pixelBufferMapping, Video::RAM, Video::RAMSize * sizeof(unsigned int));

					//Move the data to the texture from the buffer
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::ResolutionX, Video::ResolutionY, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

		void FailSafeDrawFunction()
		{
			//Initialize for drawing

			//Do some drawing.
			while (continueDrawing)
			{
				//
				//  OpenGL Window sizing, scaling, and centering!
				//

				//Check to see if this stuff has changed from previous
				if (SGE::Display::FrameBufferChanged)
				{
					RecalculateViewport();

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

					//Got it, reset the flag
					SGE::Display::FrameBufferChanged = false;
				}

				//Lock the refresh mutex
				//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
				refreshHold.lock();

				//Upload the data for the texture to the actual video card.
				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//Slow method
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::ResolutionX, Video::ResolutionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);

					//Set the flag back to normal
					GameResolutionChanged = false;

					//Flag the viewport to change to properly scale the new resolution
					FrameBufferChanged = true;
				}
				//Otherwise just update it
				else
				{
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::ResolutionX, Video::ResolutionY, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);
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
			//Universal OpenGL commands
			//These have been in OpenGL forever and a day...  We shouldn't need to check to see if there are part of the system.
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

			//Detect what version of OpenGL were are working with and figure out what rendering method to use
			fprintf(stderr, "OpenGL Version: %s\n", glGetString(GL_VERSION));

			//Go through and find what method can be successfully used.

			//Ideal version that used persistent, coherently mapped pixel buffer object
			if (GLEW_VERSION_4_4)
			{
				fprintf(stderr, "OpenGL version 4.4 detected!\n");
				OpenGL44DrawFunction();
			}
			//Fallback to a lower standard
			else
			{
				//Used Pixel Buffer Objects with basic data transfer, supported in OpenGL 2.0... So should be a safe alternative.
				if (GLEW_VERSION_2_0)
				{
					fprintf(stderr, "OpenGL version 2.0 detected!\n");
					OpenGL20DrawFunction();
				}
				else
				{
					//Oh sweet baby jesus, we are on a potato.  Only OpenGL 1?  Really?
					fprintf(stderr, "OpenGL Potato Mode Engaged!\n");
					FailSafeDrawFunction();
				}
			}	
		}


		//Open the Virtual Display Window
		void Open(int newVideoX, int newVideoY)
		{
			if (!GameDisplayOpen)
			{
				Video::ResolutionX = newVideoX;
				Video::ResolutionY = newVideoY;
				Video::RAMSize = Video::ResolutionX * Video::ResolutionY;

				//Initialize the Virtual Video RAM
				Video::RAM = new unsigned int[Video::RAMSize];

				//Initialize the Virtual Video Back Buffer
				Video::BackBuffer = new unsigned int[Video::RAMSize];

				//Initialize the Virtual Row Buffer
				Video::RowBuffer = new unsigned int[Video::ResolutionX];

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
				delete Video::RAM;
				delete Video::BackBuffer;
				delete Video::RowBuffer;

				//Set the new resolution values
				Video::ResolutionX = width;
				Video::ResolutionY = height;
				Video::RAMSize = Video::ResolutionX * Video::ResolutionY;

				//
				//Make some new RAM bits!
				//

				//Initialize the Virtual Video RAM
				Video::RAM = new unsigned int[Video::RAMSize];

				//Initialize the Virtual Video Back Buffer
				Video::BackBuffer = new unsigned int[Video::RAMSize];

				//Initialize the Virtual Row Buffer
				Video::RowBuffer = new unsigned int[Video::ResolutionX];

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
				delete Video::RAM;
				delete Video::BackBuffer;
				delete Video::RowBuffer;

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