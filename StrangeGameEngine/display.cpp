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

		//The virtual video RAM.  Public accessible to allow other components to write to it directly.
		unsigned int* VideoRAM = nullptr;

		//The virtual video back buffer.
		unsigned int* VideoBackBuffer = nullptr;

		//The virtual row buffer
		unsigned int* VideoRowBuffer = nullptr;

		//The virtual video RAM Size.
		unsigned int VideoRAMSize = 0;

		//C++ thread pointer to keep track of the spawned drawing thread.
		std::thread* drawingThread = nullptr;

		//Boolean used to control whether the draw thread keeps on running or not.
		bool continueDrawing = true;

		//Main update thread to take what's in video RAM and dump it on the screen.
		void UpdateThread()
		{
			//Variable to maintain the opengl framebuffer width and height
			int frameBufferWidth = 0;
			int frameBufferHeight = 0;

			//Track the previous width and height in case the framebuffer changes and we need to rescale stuff.
			int frameBufferPreviousWidth = 0;
			int frameBufferPreviousHeight = 0;


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


			//Do some drawing.
			while (continueDrawing)
			{
				//
				//  OpenGL Window sizing, scaling, and centering!
				//

				//Grab the current frameBuffer size
				glfwGetFramebufferSize(SGE::mainWindow, &frameBufferWidth, &frameBufferHeight);

				//Check to see if this stuff has changed from previous
				if (frameBufferHeight != frameBufferPreviousHeight || frameBufferWidth != frameBufferPreviousWidth)
				{
					//Welp, shit has changed! Recalculate the viewport!

					//Save the new values into the old ones
					frameBufferPreviousHeight = frameBufferHeight;
					frameBufferPreviousWidth = frameBufferWidth;

					//Calculate the virtualDisplay's aspect ratio
					float virtualDisplayAspectRatio = float (SGE::Display::ResolutionX) / float (SGE::Display::ResolutionY);
					
					//Test the aspect ratio against the new width to see if we can scale using the height
					if (frameBufferHeight * virtualDisplayAspectRatio <= frameBufferWidth)
					{
						//Set new the Viewport
						glViewport(
							//Center it in the middle of the X axis
							int((frameBufferWidth - frameBufferHeight * virtualDisplayAspectRatio) / 2),
							//Set the Y to the origin
							0, 
							//Scale the width based on the height and aspect ratio
							int(frameBufferHeight * virtualDisplayAspectRatio), 
							//Set the height to the frameBufferHeight
							frameBufferHeight);
					}

					//Otherwise, just scale using the new width
					else
					{
						//Set new the viewport
						glViewport(
							//Set the X to the origin
							0,
							//Center it in the middle of the Y axis
							int((frameBufferHeight - frameBufferWidth / virtualDisplayAspectRatio) / 2),
							//Set the width to the frame buffer
							frameBufferWidth, 
							//Scale the height based on the width and aspect ratio
							int(frameBufferWidth / virtualDisplayAspectRatio));
					}
				}

				//Lock the refresh mutex
				//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
				refreshHold.lock();

				//Upload the data for the texture to the actual video card.
				//Yes, this method is technically slower, but there is ONLY one texture in the entire program and no additial code is required to handle resolution changes
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ResolutionX, ResolutionY, 0, GL_RGBA, GL_UNSIGNED_BYTE, VideoRAM);

				//Unlock the refresh mutex
				refreshHold.unlock();

				//
				//  Start drawing the textured quad
				//

				//Clear the color buffers
				//Possibly no need since we are going to overwrite everything anyway with the single all encompassing quad.
				//glClear(GL_COLOR_BUFFER_BIT);

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
			ResolutionX = newVideoX;
			ResolutionY = newVideoY;
			VideoRAMSize = ResolutionX * ResolutionY;

			//Initialize the Virtual Video RAM
			VideoRAM = new unsigned int[VideoRAMSize];

			//Initialize the Virtual Video Back Buffer
			VideoBackBuffer = new unsigned int[VideoRAMSize];

			//Initialize the Virtual Row Buffer
			VideoRowBuffer = new unsigned int[ResolutionX];
		}

		void Close()
		{
			StopDrawing();

			//Make sure created bits are deleted appropriately.
			delete drawingThread;
			delete VideoRAM;
			delete VideoBackBuffer;
			delete VideoRowBuffer;
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
	}
}