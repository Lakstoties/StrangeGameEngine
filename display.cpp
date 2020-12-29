//
//  Include GLFW to handle windowing
//
#include <GLFW\glfw3.h>

//
//  Include threading and mutexes
//
#include <thread>
#include <mutex>

//
//  Include display header
//
#include "include\SGE\display.h"

//
//  Include SGE System header
//
#include "include\SGE\system.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Extern to a component within the SGE namespace.  It's the main GLFW window
	//
	extern GLFWwindow* OSWindow;

	//
	//  Display namespace contains anything relating to the system's display functionality.
	//
	namespace Display
	{
		//
		//  Callbacks namespace for callbacks related to Display
		//
		namespace Callbacks
		{
			//
			//  Window Resize Context callback for GLFW
			//
			void WindowResize(GLFWwindow* window, int width, int height)
			{
				//
				//  Flag that the framebuffer window size has changed for the rest of the system
				//
				SGE::Display::FrameBufferChanged = true;
			}

			//
			//  Function to register callbacks
			//
			void Register()
			{
				//
				//  Window Resize callback
				//
				glfwSetWindowSizeCallback(SGE::OSWindow, SGE::Display::Callbacks::WindowResize);
			}
		}


		//
		//  Video namespace contains anything relating directly to video output onto the main game window
		//
		namespace Video
		{
			//
			//  The virtual video RAM.  Publicly accessible to allow other components to write to it directly.
			//  This is by design.  Also, it is initially set to the DefaultRAM, but can be changed later.
			//
			pixel RAM[MAX_VIDEO_RAM] = { 0 };

			//The virtual video horizontal resolution
			int X = 0;

			//The virtual video vertical resolution
			int Y = 0;
		}

		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to Video RAM
		std::mutex refreshHold;

		//
		//  Flag to indicate the framebuffer window size has changed
		//  This way components outside the event callback are informed of the need to change and check
		//
		bool FrameBufferChanged = false;

		//
		//  The viewpoint window (within the framebuffer window) vertical resolution
		//
		int ViewPortWindowX = 0;

		//
		//  The viewpoint window (within the framebuffer window) horizontal resolution
		//
		int ViewPortWindowY = 0;
		
		//
		//  The viewpoint window (within the framebuffer window) X offset
		//
		int ViewPortWindowOffsetX = 0;

		//
		//  The viewpoint window (within the framebuffer window) Y offset
		//
		int ViewPortWindowOffsetY = 0;

		//
		//  Internal flag to signal if a game display is presently open
		//
		bool GameDisplayOpen = false;

		//
		//  Flag to indicate the game resolution has changed
		//  Start as true to set up the initial texture
		//
		bool GameResolutionChanged = true;

		//
		//  C++ thread pointer to keep track of the spawned drawing thread.
		//
		std::thread* drawingThread = nullptr;

		//
		//  Boolean used to control whether the draw thread keeps on running or not.
		//
		bool continueDrawing = true;

		//
		//  Frames rendered count
		//
		unsigned int FrameCount = 0;


		//
		//  Times of various stages
		//

		// Upload Timer:  This timer measures the time it takes for the current virtual video ram to be uploaded to the GPU, in microseconds.
		int UploadTime = 0;

		//  Rendering Timer: This timer measures the time it takes for system to viewing surface for rendering.
		int RenderTime = 0;

		//  Update Timer:  This is the overall time for the entire video update process.
		int UpdateTime = 0;

		//
		//  Recalculate View Port Dimensions
		//
		void RecalculateViewport()
		{
			//
			//  Temporary Values to get the latest framebuffer size from the OS
			//
			int frameBufferX = 0;
			int frameBufferY = 0;

			//
			//  Get the frame buffer size
			//
			glfwGetFramebufferSize(SGE::OSWindow, &frameBufferX, &frameBufferY);

			//Update the viewport sizes and offsets
			//Calculate offsets
			SGE::Display::ViewPortWindowOffsetX = (frameBufferX - (frameBufferY * Video::X) / Video::Y) / 2;
			SGE::Display::ViewPortWindowOffsetY = (frameBufferY - (frameBufferX * Video::Y) / Video::X) / 2;

			//If the Offset goes negative it needs to be hard capped or it throws off calculations.
			if (SGE::Display::ViewPortWindowOffsetX < 0) { (SGE::Display::ViewPortWindowOffsetX = 0); }
			if (SGE::Display::ViewPortWindowOffsetY < 0) { (SGE::Display::ViewPortWindowOffsetY = 0); }

			//Calculate viewport
			//Take the framebuffer and subtract double the viewport offsets to scale appropriately for the window
			SGE::Display::ViewPortWindowX = frameBufferX - (SGE::Display::ViewPortWindowOffsetX * 2);
			SGE::Display::ViewPortWindowY = frameBufferY - (SGE::Display::ViewPortWindowOffsetY * 2);
		}

		//
		//  Optimized Drawing Function for OpenGL 4.4 and later
		//
		void OpenGLDrawFunction()
		{
			//
			//Universal OpenGL commands
			//These have been in OpenGL forever and a day...  We shouldn't need to check to see if there are part of the system.
			//

			//Handle to call the OpenGL texture we are using
			GLuint textureHandle = 0;

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


			//Enable VSYNC to keep the frame rate from going too ridiculous.
			//There's no need to outpace the monitor update rate for updating the contents of the Virtual Video RAM
			glfwSwapInterval(1);


			//
			//  Time trackers
			//

			// Create a variable to capture time before rendering
			std::chrono::time_point<std::chrono::steady_clock> uploadStartTime;

			// Create a variable to capture time before rendering
			std::chrono::time_point<std::chrono::steady_clock> renderStartTime;

			// Create a variable to capture time before rendering
			std::chrono::time_point<std::chrono::steady_clock> updateStartTime;


			//Do some drawing.
			while (continueDrawing)
			{
				//
				//  OpenGL Window sizing, scaling, and centering!
				//

				//  Mark the time
				updateStartTime = std::chrono::steady_clock::now();

				//Check to see if this stuff has changed from previous
				if (SGE::Display::FrameBufferChanged)
				{
					RecalculateViewport();

					//It has changed!  Set new the Viewport
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

				//
				//  Upload CPU Virtual RAM to GPU Texture
				//

				//  Mark the time
				uploadStartTime = std::chrono::steady_clock::now();

				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//Lock the refresh mutex
					//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
					refreshHold.lock();

					//Move load up the texture data from the buffer
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::X, Video::Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);

					//Unlock the refresh mutex
					refreshHold.unlock();
									   					 
					//Set the flag back to normal
					GameResolutionChanged = false;

					//Flag the viewport to change to properly scale the new resolution
					FrameBufferChanged = true;
				}
				//Otherwise just update it
				else
				{
					//Lock the refresh mutex
					//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
					refreshHold.lock();

					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::X, Video::Y, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);

					//Unlock the refresh mutex
					refreshHold.unlock();
				}

				// Upload Time Stop
				UploadTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - uploadStartTime).count();

				//
				//  Rendering Timer Start
				//
				renderStartTime = std::chrono::steady_clock::now();

				//
				//  Start drawing the textured quad
				//

				//  Clear the color buffers
				glClear(GL_COLOR_BUFFER_BIT);

				//Draw a quad
				glBegin(GL_QUADS);

				glTexCoord2f(0.0f, 1.0f);  	glVertex3f(-1.0f, -1.0f, 0.0f);		//Bottom Left
				glTexCoord2f(1.0f, 1.0f); 	glVertex3f(1.0f, -1.0f, 0.0f);		//Bottom Right
				glTexCoord2f(1.0f, 0.0f); 	glVertex3f(1.0f, 1.0f, 0.0f);		//Top Right
				glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, 0.0f);		//Top Left

				//Done Drawing that
				glEnd();

				// Rendering Time Stop
				RenderTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - renderStartTime).count();

				//Check to see if we have a valid place to update to
				//Or if the window should be closed.
				if (!glfwWindowShouldClose(SGE::OSWindow))
				{
					//Display the new after we are done drawing it
					glfwSwapBuffers(SGE::OSWindow);
				}

				//We don't have a window to swap buffers to
				//Shut it down
				else
				{
					//Done drawing this.
					continueDrawing = false;

					//We're out!
					return;
				}

				// Update Time Stop
				UpdateTime = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - updateStartTime).count();

				//
				//  Count the frame
				//
				FrameCount++;
			}
		}

		

		//Main update thread to take what's in video RAM and dump it on the screen.
		void UpdateThread()
		{
			//Check to see if we have a valid window to draw to or if the window is closing down
			if (glfwWindowShouldClose(SGE::OSWindow))
			{
				//If the window is closing, we ain't doing anymore drawing.
				continueDrawing = false;

				//We're out!
				return;
			}

			//Snag the Open context for the main window
			glfwMakeContextCurrent(SGE::OSWindow);

			//  Start the system up
			OpenGLDrawFunction();
		}

		//
		//  Initialization Function to get the OSWindow set up the way it should be
		//

		void Init(int windowX, int windowY, const char* gameTitle)
		{
			//
			//  Check to make sure another window isn't active, we only want one window going at a time.
			//
			if (OSWindow != nullptr)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "There's already a game window open!\n");
				return;
			}

			//
			//  Create a standard Window
			//
			OSWindow = glfwCreateWindow(windowX, windowY, gameTitle, nullptr, nullptr);

			if (OSWindow == nullptr)
			{
				//
				//  Huh, window creation failed...
				//
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "GLFW failed to create the main game window.\n");
			}
		}

		//Open the Virtual Display Window
		void Open(int newVideoX, int newVideoY)
		{
			if (!GameDisplayOpen)
			{
				Video::X = newVideoX;
				Video::Y = newVideoY;

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

				//Set the new resolution values
				Video::X = width;
				Video::Y = height;

				//
				//Make some new RAM bits!
				//

				//Flag the game resolution has changed
				GameResolutionChanged = true;

				//Release the refresh
				refreshHold.unlock();
			}
		}
		
		//
		//  Closes out the drawing subsystem completely
		//
		void Close()
		{
			if (GameDisplayOpen)
			{
				//
				//  Stop the drawing thread
				//
				StopDrawing();

				//
				//  Make sure created bits are deleted appropriately.
				//
				delete drawingThread;
				//delete Video::RAM;

				//
				//  Set that the game display is not open
				//
				GameDisplayOpen = false;
			}
		}

		//
		//  Starts the main drawing thread
		//
		void StartDrawing()
		{
			//Flag the thread to start drawing
			continueDrawing = true;

			//Launch that thread
			drawingThread = new std::thread(&Display::UpdateThread);
		}

		//
		//  Stops the main drawing thread
		//
		void StopDrawing()
		{
			//
			//  Signal it to stop.
			//
			continueDrawing = false;

			//
			//  Wait for it to join back up
			//
			if (drawingThread->joinable())
				drawingThread->join();
		}

		//
		//  Blocks Drawing Refresh
		//
		void BlockRefresh()
		{
			refreshHold.lock();
		}

		//
		//  Release the lock on the drawing refresh
		//
		void AllowRefresh()
		{
			refreshHold.unlock();
		}

		//
		//  Sets the OS window size
		//
		void SetWindowSize(int width, int height)
		{
			//
			//  Check to make sure the window size is not negative or 0
			//
			if (width < 1 || height < 1)
				return;

			//
			//  Adjust the window size
			//
			glfwSetWindowSize(OSWindow, width, height);
		}


		//
		//  Hide the mouse cursor from the OS
		//
		void HideOSMouseCursor()
		{
			glfwSetInputMode(SGE::OSWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		//
		//  Show the mouse cursor from the OS
		//
		void ShowOSMouseCursor()
		{
			glfwSetInputMode(SGE::OSWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}