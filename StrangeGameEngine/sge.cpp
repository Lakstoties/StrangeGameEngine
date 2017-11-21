#include "include\SGE\sge.h"
#include "sharedinternal.h"
#include <GLFW\glfw3.h>

namespace SGE
{
	//GLFW Window pointer for the Strange Game Engine
	GLFWwindow* mainWindow = nullptr;

	//Callback to handle any error reporting from GLFW
	void GLFWErrorCallback(int error, const char* description)
	{
		//Dump the error info straight to the stderr
		fprintf(stderr, "GLFW Error: %s\n", description);
	}

	//Window Resize Context callback for GLFW
	void WindowResizeCallback(GLFWwindow* window, int width, int height)
	{
		//Update the framebuffer window sizes
		SGE::Display::FrameBufferX = width;
		SGE::Display::FrameBufferY = height;

		//Flag that the framebuffer window size has changed for the rest of the system
		SGE::Display::FrameBufferChanged = true;
	}

	//Mouse Scrool Wheel callback for GLFW
	void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset)
	{
		SGE::Controls::MouseScrollX += xOffset;
		SGE::Controls::MouseScrollY += yOffset;

		//fprintf(stderr, "Scroll event catpured: %f, %f \n", xOffset, yOffset);
	}

	//Mouse Button callback for GLFW
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		switch (action)
		{
		case GLFW_PRESS:
			SGE::Controls::MouseButtons[button] = true;
			break;

		case GLFW_RELEASE:
			SGE::Controls::MouseButtons[button] = false;
			break;
		}
	}


	//Keyboard callback for GLFW
	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		//Check to make sure we are dealing with the GLFW_UNKNOWN key bit.
		//Fuck that unkwown key bullshit.
		//Ain't having it.
		//Nope...
		if (key >= 0)
		{
			//Update the key array for the state of the key
			if (action == GLFW_PRESS)
			{
				SGE::Controls::KeyboardStatus[key] = true;
			}

			else if (action == GLFW_RELEASE)
			{
				SGE::Controls::KeyboardStatus[key] = false;
			}
		}
	}

	//Mouse Cursor callback for GLFW
	void CursorCallback(GLFWwindow* window, double xPosition, double yPosition)
	{
		int currentFrameBufferX = 0;
		int currentFrameBufferY = 0;

		int currentFrameBufferXOffset = 0;
		int currentFrameBufferYOffset = 0;

		//Get the current Frame buffer data
		glfwGetFramebufferSize(mainWindow, &currentFrameBufferX, &currentFrameBufferY);

		//Calculate offsets
		currentFrameBufferXOffset = (currentFrameBufferX - (currentFrameBufferY * SGE::Display::ResolutionX) / SGE::Display::ResolutionY) / 2;
		currentFrameBufferYOffset = (currentFrameBufferY - (currentFrameBufferX * SGE::Display::ResolutionY) / SGE::Display::ResolutionX) / 2;

		//Short circuit logic
		//If the Offset goes negative it needs to be hard capped or it throws off calculations.
		(currentFrameBufferXOffset < 0) && (currentFrameBufferXOffset = 0);
		(currentFrameBufferYOffset < 0) && (currentFrameBufferYOffset = 0);

		//Raw Numbers
		SGE::Controls::MousePositionRawX = (int)xPosition;
		SGE::Controls::MousePositionRawY = (int)yPosition;

		//Scaled
		SGE::Controls::MousePositionX = ((SGE::Controls::MousePositionRawX - currentFrameBufferXOffset) * SGE::Display::ResolutionX) / (currentFrameBufferX - currentFrameBufferXOffset * 2);
		SGE::Controls::MousePositionY = ((SGE::Controls::MousePositionRawY - currentFrameBufferYOffset) * SGE::Display::ResolutionY) / (currentFrameBufferY - currentFrameBufferYOffset * 2);
	}


	//Startup Function that performs all initializations to the system overall
	//Needs to be called first.   Really...  Call it first...
	void Startup(int windowX, int windowY, const char* gameTitle)
	{
		//Initialize GLFW
		//If the initialization doesn't succeed
		if (!glfwInit())
		{
			//Report the error
			fprintf(stderr, "GLFW initialization failure!\n");
		}

		//Set the GLFW Error Callback
		glfwSetErrorCallback(GLFWErrorCallback);

		//Start up the sound system
		SGE::Sound::Start();

		//Check to make sure another window isn't active, we only want one window going at a time.
		if (mainWindow != nullptr)
		{
			fprintf(stderr, "There's already a game window open!\n");
			return;
		}

		//Otherwise, let's create this window.
		mainWindow = glfwCreateWindow(windowX, windowY, gameTitle, NULL, NULL);

		if (mainWindow == nullptr)
		{
			//Huh, window creation failed...
			fprintf(stderr, "GLFW failed to create the main game window.\n");
		}

		//
		//Register all the major callbacks for SGE system
		//

		//Window Resize callback
		glfwSetWindowSizeCallback(SGE::mainWindow, WindowResizeCallback);

		//Mouse scrool wheel callback
		glfwSetScrollCallback(SGE::mainWindow, ScrollWheelCallback);

		//Mouse button callback
		glfwSetMouseButtonCallback(SGE::mainWindow, MouseButtonCallback);

		//Mouse cursor callback
		glfwSetCursorPosCallback(SGE::mainWindow, CursorCallback);

		//Keyboard callback
		glfwSetKeyCallback(SGE::mainWindow, KeyboardCallback);

	}

	//Shutdown function that performs all the shutdown and termination bits for the system.
	void Shutdown()
	{
		//Clean UP!
		//Make sure the mainDisplay has stopped drawing.
		SGE::Display::StopDrawing();

		//Check to see if there is an active window to close
		if (mainWindow == nullptr)
		{
			//There's no window to close!
			fprintf(stderr, "There is no active game window to close.\n");
		}

		//Close that window
		glfwDestroyWindow(mainWindow);

		//Make sure the mainSoundSystem has stopped running.
		SGE::Sound::Stop();

		//Terminate GLFW, if the game engine is done, so are we.
		glfwTerminate();
	}
}