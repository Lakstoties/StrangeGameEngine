#include "display.h"

namespace SGE
{
	//Main update thread to take what's in video RAM and dump it on the screen.
	void VirtualDisplay::UpdateThread()
	{
		//Variable to maintain the opengl framebuffer width and height
		int frameBufferWidth = 0;
		int frameBufferHeight = 0;
		float frameBufferRatio = 0.0f;

		//Handle to call the OpenGL texture we are using
		GLuint textureHandle = 0;

		//Snag the Open context for our targetWindow, unless for some strange reason it's already suppose to close.
		if (!glfwWindowShouldClose(targetWindow))
		{
			glfwMakeContextCurrent(targetWindow);

			//Do some initialization bits

			//Enable 2D Texturing
			glEnable(GL_TEXTURE_2D);

			//Disable Alpha Blending
			glDisable(GL_BLEND);

			//Generate the texture space and pass the handle off
			glGenTextures(1, &textureHandle);
		}

		//Do some drawing.
		while (continueDrawing)
		{
			//Manage updating the texture that serves as our "buffer"
			//Lock the refresh mutex
			refreshHold.lock();

			//Bind a 2D texture to the texture handle
			glBindTexture(GL_TEXTURE_2D, textureHandle);

			//Turn off Mip map filtering and all that jazz
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			//Adjust Texture magnification
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			//Upload the data for the texture to the actual video card.
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, virtualVideoX, virtualVideoY, 0, GL_RGBA, GL_UNSIGNED_BYTE, virtualVideoRAM);

			//Unlock the refresh mutex
			refreshHold.unlock();

			//Check to see if we have a valid place to update to
			//Or if the window should be closed.
			if (!glfwWindowShouldClose(targetWindow))
			{
				//Grab the current frameBuffer size
				glfwGetFramebufferSize(targetWindow, &frameBufferWidth, &frameBufferHeight);

				//Setup the openGL Viewport
				glViewport(0, 0, frameBufferWidth, frameBufferHeight);

				//Clear the color buffers
				glClear(GL_COLOR_BUFFER_BIT);

				//Set the Matrix we are going to modify
				glMatrixMode(GL_PROJECTION);

				//Load the identity matrix
				glLoadIdentity();

				//Setup an orthographic view
				glOrtho(-frameBufferRatio, frameBufferRatio, -1.f, 1.f, 1.f, -1.f);

				//Switch to the Model view
				glMatrixMode(GL_MODELVIEW);

				//Load the identity matrix
				glLoadIdentity();
			

				//Set this as the texture to use.
				glBindTexture(GL_TEXTURE_2D, textureHandle);

				//Draw a quad
				glBegin(GL_QUADS);
				glTexCoord2f(0.0f, 1.0f);  	glVertex3f(-1.0f, -1.0f, 0.0f);		//Bottom Left
				glTexCoord2f(1.0f, 1.0f); 	glVertex3f( 1.0f, -1.0f, 0.0f);		//Bottom Right
				glTexCoord2f(1.0f, 0.0f); 	glVertex3f( 1.0f,  1.0f, 0.0f);		//Top Right
				glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f,  1.0f, 0.0f);		//Top Left

				//Done Drawing that
				glEnd();


				//Display the new shit after we are done drawing it
				glfwSwapBuffers(targetWindow);

				//Frame Rate Limiter Section
				std::this_thread::sleep_for(std::chrono::milliseconds(frameWaitMilliseconds));
			}

			//We don't have a valid place to write to
			//Shut it down
			else
			{
				continueDrawing = false;
			}
		}
	}

	//Constructor for the new display
	VirtualDisplay::VirtualDisplay(int newVideoX, int newVideoY, GLFWwindow* newTargetWindow)
	{
		int targetWindowWidth;
		int targetWindowHeight;

		virtualVideoX = newVideoX;
		virtualVideoY = newVideoY;
		targetWindow = newTargetWindow;

		//Initialize the Virtual Video RAM
		virtualVideoRAM = new unsigned int[virtualVideoX * virtualVideoY];

		//Set the scaling so virtual screen matches the game window
		glfwGetFramebufferSize(targetWindow, &targetWindowWidth, &targetWindowHeight);
		videoScaling = float(targetWindowWidth) / float(virtualVideoX);

	}

	VirtualDisplay::~VirtualDisplay()
	{
		//Signal it to stop.
		continueDrawing = false;

		//Wait for it to join back up, if it already hasn't
		if (drawingThread->joinable())
			drawingThread->join();

		//Make sure created bits are deleted appropriately.
		delete drawingThread;
		delete virtualVideoRAM;
	}

	void VirtualDisplay::StartDrawing()
	{
		//Flag the thread to start drawiong
		continueDrawing = true;

		//Launch that thread
		drawingThread = new std::thread(&VirtualDisplay::UpdateThread, this);
	}

	void VirtualDisplay::StopDrawing()
	{
		//Signal it to stop.
		continueDrawing = false;

		//Wait for it to join back up
		if (drawingThread->joinable())
			drawingThread->join();
	}
}