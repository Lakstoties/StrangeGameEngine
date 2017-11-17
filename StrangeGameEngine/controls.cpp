#include "include\SGE\controls.h"
#include "sharedinternal.h"
#include <GLFW\glfw3.h>
#include <thread>
#include <algorithm>

namespace SGE
{
	namespace Controls
	{
		//Flag to indicate to the handle events threads to continue or not
		bool ContinueToHandleEvents = true;


		//Keyboard status array that keeps track of which keys are pressed
		bool KeyboardStatus[NUMBER_OF_KEYS] = { false };

		int MousePositionX = 0;
		int MousePositionRawX = 0;

		int MousePositionY = 0;
		int MousePositionRawY = 0;

		float MouseXScaling = 1.0f;
		int MouseXOffset = 0;

		float MouseYScaling = 1.0f;
		int MouseYOffset = 0;

		int MouseScrollX = 0;
		int MouseScrollY = 0;


		bool MouseButtons[NUMBER_OF_BUTTONS] = { false };


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
					KeyboardStatus[key] = true;
				}

				else if (action == GLFW_RELEASE)
				{
					KeyboardStatus[key] = false;
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
			MousePositionRawX = (int) xPosition;
			MousePositionRawY = (int) yPosition;

			//Scaled
			MousePositionX = ((MousePositionRawX - currentFrameBufferXOffset) * SGE::Display::ResolutionX) / (currentFrameBufferX - currentFrameBufferXOffset * 2);
			MousePositionY = ((MousePositionRawY - currentFrameBufferYOffset) * SGE::Display::ResolutionY) / (currentFrameBufferY - currentFrameBufferYOffset * 2);
		}

		//Mouse Button callback for GLFW
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{
			switch (action)
			{
			case GLFW_PRESS:
				MouseButtons[button] = true;
				break;

			case GLFW_RELEASE:
				MouseButtons[button] = false;
				break;
			}

		}

		//Mouse Scrool Wheel callback for GLFW
		void ScrollWheelCallback(GLFWwindow* window, double xOffset, double yOffset)
		{
			MouseScrollX += xOffset;
			MouseScrollY += yOffset;

			//fprintf(stderr, "Scroll event catpured: %f, %f \n", xOffset, yOffset);
		}

		//Window Resize Context callback for GLFW
		void WindowResizeCallback(GLFWwindow* window, int width, int height)
		{
			//Update the framebuffer window sizes
			SGE::Display::FrameBufferX = width;
			SGE::Display::FrameBufferY = height;

			//Update the viewport sizes and offsets
			//Calculate offsets
			SGE::Display::ViewPortWindowOffsetX = (SGE::Display::FrameBufferX - (SGE::Display::FrameBufferY * SGE::Display::ResolutionX) / SGE::Display::ResolutionY) >> 1;
			SGE::Display::ViewPortWindowOffsetY = (SGE::Display::FrameBufferY - (SGE::Display::FrameBufferX * SGE::Display::ResolutionY) / SGE::Display::ResolutionX) >> 1;

			//Short circuit logic
			//If the Offset goes negative it needs to be hard capped or it throws off calculations.
			(SGE::Display::ViewPortWindowOffsetX < 0) && (SGE::Display::ViewPortWindowOffsetX = 0);
			(SGE::Display::ViewPortWindowOffsetY < 0) && (SGE::Display::ViewPortWindowOffsetY = 0);

			//Calculate viewport
			//Take the framebuffer and subtract double the viewport offsets to scale appropriately for the window
			SGE::Display::ViewPortWindowX = SGE::Display::FrameBufferX - (SGE::Display::ViewPortWindowOffsetX << 1);
			SGE::Display::ViewPortWindowY = SGE::Display::FrameBufferY - (SGE::Display::ViewPortWindowOffsetY << 1);

			//Flag that the framebuffer window size has changed for the rest of the system
			SGE::Display::FrameBufferChanged = true;
		}


		void HandleEvents()
		{
			//
			//Register the callbacks
			//

			//Keyboard callback
			glfwSetKeyCallback(SGE::mainWindow, KeyboardCallback);

			//Mouse cursor callback
			glfwSetCursorPosCallback(SGE::mainWindow, CursorCallback);

			//Mouse button callback
			glfwSetMouseButtonCallback(SGE::mainWindow, MouseButtonCallback);

			//Mouse scrool wheel callback
			glfwSetScrollCallback(SGE::mainWindow, ScrollWheelCallback);

			//Window Resize callback
			glfwSetWindowSizeCallback(SGE::mainWindow, WindowResizeCallback);


			//Start Handling Events through Main Interface
			//This has to handled by the same thread that spawned the GUI window
			//And in OSX this has to be done by the thread that invoked the main function of the program..  Fuckin' Macs...

			//While the main window is going and isn't set to close...
			while (!glfwWindowShouldClose(SGE::mainWindow) && ContinueToHandleEvents)
			{
				//Poll for events
				glfwPollEvents();

				//Wait for new events
				glfwWaitEvents();
			}
		}

		//Copy current keyboard state to another array
		void SaveKeyboardStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS])
		{
			//memcpy(targetKeyboardStatusArray, KeyboardStatus, sizeof(KeyboardStatus));
			//std::copy(KeyboardStatus, KeyboardStatus + NUMBER_OF_KEYS, targetKeyboardStatusArray);
			std::memmove(targetKeyboardStatusArray, KeyboardStatus, sizeof(KeyboardStatus));
		}
	}
}
