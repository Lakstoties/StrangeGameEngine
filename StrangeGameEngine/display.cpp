//
//  GLEW goes before GLFW3 because GLFW inlcudes GL.h
//
#include "GL\glew.h"

//
//  Include GLFW to handle windowing
//
#include <GLFW\glfw3.h>

//
//  Include threading and mutexs
//
#include <thread>
#include <mutex>

//
//  Vectors?
//
#include <vector>

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
	//  Extern to a compoment within the SGE namespace.  It's the main GLFW window
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
			//  Default RAM - This is RAM that is used by default is certain optmizations are not possible.
			//
			pixel DefaultRAM[MAX_VIDEO_RAM];

			//
			//  The virtual video RAM.  Publically accessible to allow other components to write to it directly.
			//  This is by design.  Also, it is initially set to the DefaultRAM, but can be changed later.
			//
			pixel* RAM = DefaultRAM;

			//The virtual video horizontal resolution
			int X = 0;

			//The virtual video vertical resolution
			int Y = 0;
		}


		enum class DisplayRenderingModes : int
		{
			Derp = 0,
			Potato = 10,
			OpenGL20 = 20,
			OpenGL44 = 44,
			Vulkan = 99
		};

		DisplayRenderingModes CurrentRenderingMode = DisplayRenderingModes::Derp;


		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to VRAM
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
			SGE::Display::ViewPortWindowOffsetX = (frameBufferX - (frameBufferY * Video::X) / Video::Y) >> 1;
			SGE::Display::ViewPortWindowOffsetY = (frameBufferY - (frameBufferX * Video::Y) / Video::X) >> 1;

			//If the Offset goes negative it needs to be hard capped or it throws off calculations.
			if (SGE::Display::ViewPortWindowOffsetX < 0) { (SGE::Display::ViewPortWindowOffsetX = 0); }
			if (SGE::Display::ViewPortWindowOffsetY < 0) { (SGE::Display::ViewPortWindowOffsetY = 0); }

			//Calculate viewport
			//Take the framebuffer and subtract double the viewport offsets to scale appropriately for the window
			SGE::Display::ViewPortWindowX = frameBufferX - (SGE::Display::ViewPortWindowOffsetX << 1);
			SGE::Display::ViewPortWindowY = frameBufferY - (SGE::Display::ViewPortWindowOffsetY << 1);
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
			//Initialize for drawing
			//

			//Experimental
			//Create a simple handle for the Pixel Buffer Object
			GLuint pixelBufferObject;
			if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44 || CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
			{
				//Generate a Buffer
				glGenBuffers(1, &pixelBufferObject);

				//Bind the buffer to our context
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixelBufferObject);
			}

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

				//Upload the data for the texture to the actual video card.
				//If the game resolution has changed, then a new texture is needed, since the texture dimensions could have changed.
				if (GameResolutionChanged)
				{
					//
					//  Setup the new texture based on the current rendering mode.
					//
					if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44)
					{
						//Recreate the Buffer storage for the new video ram size
						glBufferStorage(GL_PIXEL_UNPACK_BUFFER, Video::X * Video::Y * sizeof(Video::pixel), 0, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);

						//Grab the pinter to the mapped buffer range
						//  And assign it as the current Video RAM
						SGE::Display::Video::RAM = (SGE::Display::Video::pixel*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, Video::X * Video::Y * sizeof(Video::pixel), GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
					}
					else if (CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
					{
						//Recreate the Buffer storage for the new video ram size
						glBufferData(GL_PIXEL_UNPACK_BUFFER, Video::X * Video::Y * sizeof(Video::pixel), Video::RAM, GL_DYNAMIC_DRAW);
					}

					//Lock the refresh mutex
					//If we can't get the lock, then there's a chance someone is working on the VideoRAM and we should wait for them to get done to prevent a tearing effect.
					refreshHold.lock();

					//Move load up the texture data from the buffer
					if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44 || CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::X, Video::Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
					}
					else
					{
						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Video::X, Video::Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);
					}

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

					if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44)
					{

					}
					else if (CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
					{
						glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, Video::X * Video::Y * sizeof(Video::pixel), Video::RAM);
					}

					//Move the data to the texture from the buffer
					if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44 || CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
					{
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::X, Video::Y, GL_RGBA, GL_UNSIGNED_BYTE, 0);
					}
					else
					{
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Video::X, Video::Y, GL_RGBA, GL_UNSIGNED_BYTE, Video::RAM);
					}

					//Wait until thE GPU is done.
					glFinish();

					//Unlock the refresh mutex
					refreshHold.unlock();
				}


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

				//Check to see if we have a valid place to update to
				//Or if the window should be closed.
				if (!glfwWindowShouldClose(SGE::OSWindow))
				{
					//Display the new shit after we are done drawing it
					glfwSwapBuffers(SGE::OSWindow);
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

			//Initialize GLEW so it can find all the bits and pieces it needs.
			glewInit();

			//
			//  Detect what version of OpenGL were are working with and figure out what rendering method to use
			//
			SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Display, "OpenGL Version: %s\n", glGetString(GL_VERSION));


			//
			//  Let's figure out what Display Mode to Use
			//

			if (GLEW_VERSION_4_4)
			{
				CurrentRenderingMode = DisplayRenderingModes::OpenGL44;
			}
			else if (GLEW_VERSION_2_0)
			{
				CurrentRenderingMode = DisplayRenderingModes::OpenGL20;
			}
			else
			{
				CurrentRenderingMode = DisplayRenderingModes::Potato;
			}

			//
			//  For test purposes
			//  Manually set the rendering mode.
			//CurrentRenderingMode = DisplayRenderingModes::Potato;

			//Ideal version that used persistent mapped pixel buffer object
			if (CurrentRenderingMode == DisplayRenderingModes::OpenGL44)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Display, "OpenGL version 4.4 or greater detected!\n");
			}
			else if (CurrentRenderingMode == DisplayRenderingModes::OpenGL20)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Display, "OpenGL version 2.0 or greater detected!\n");
			}
			else
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Display, "Old... OLD OpenGL detected:  Potato Mode ENGAGED!\n");
			}



			//
			//  Start the system up
			//
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
			OSWindow = glfwCreateWindow(windowX, windowY, gameTitle, NULL, NULL);

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
			//Flag the thread to start drawiong
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