#include "include\SGE\inputs.h"
#include "include\SGE\display.h"
#include <GLFW\glfw3.h>
#include <thread>
#include <algorithm>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  This is an extern to the main GLFW window that is part of the SGE namespace
	//
	extern GLFWwindow* OSWindow;

	//
	//  Inputs namespace that handles input from keyboards, mice, and other controllers
	//
	namespace Inputs
	{
		//
		//  Namespace for Callbacks related to inputs
		//
		namespace Callbacks
		{
			//
			//  Mouse Scrool Wheel callback for GLFW
			//
			void ScrollWheel(GLFWwindow* window, double xOffset, double yOffset)
			{
				SGE::Inputs::Mouse::ScrollX += (int)xOffset;
				SGE::Inputs::Mouse::ScrollY += (int)yOffset;
			}

			//
			//  Mouse Button callback for GLFW
			//
			void MouseButton(GLFWwindow* window, int button, int action, int mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
					SGE::Inputs::Mouse::Buttons[button] = true;
					break;

				case GLFW_RELEASE:
					SGE::Inputs::Mouse::Buttons[button] = false;
					break;
				}
			}

			//
			//  Keyboard callback for GLFW
			//
			void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				//
				//  Check to make sure we aren't dealing with the GLFW_UNKNOWN key.
				//  If so just ignore it.
				//
				if (key >= 0)
				{
					//  If pressed down
					if (action == GLFW_PRESS)
					{
						//
						//  Update the status array
						//
						SGE::Inputs::Keyboard::Status[key] = true;
					}

					// If released
					else if (action == GLFW_RELEASE)
					{
						//
						//  Update the status array
						//
						SGE::Inputs::Keyboard::Status[key] = false;
					}
				}
			}

			//
			//  Mouse Cursor callback for GLFW
			//
			void Cursor(GLFWwindow* window, double xPosition, double yPosition)
			{
				int currentFrameBufferX = 0;
				int currentFrameBufferY = 0;

				int currentFrameBufferXOffset = 0;
				int currentFrameBufferYOffset = 0;

				//
				//  Get the current Frame buffer data
				//
				glfwGetFramebufferSize(OSWindow, &currentFrameBufferX, &currentFrameBufferY);

				//
				//  Calculate offsets
				//
				currentFrameBufferXOffset = (currentFrameBufferX - (currentFrameBufferY * SGE::Display::Video::X) / SGE::Display::Video::Y) / 2;
				currentFrameBufferYOffset = (currentFrameBufferY - (currentFrameBufferX * SGE::Display::Video::Y) / SGE::Display::Video::X) / 2;

				//
				//If the Offset goes negative it needs to be hard capped or it throws off calculations.
				//
				if (currentFrameBufferXOffset < 0) { (currentFrameBufferXOffset = 0); }
				if (currentFrameBufferYOffset < 0) { (currentFrameBufferYOffset = 0); }

				//
				//Raw Numbers
				//
				SGE::Inputs::Mouse::PositionRawX = (int)xPosition;
				SGE::Inputs::Mouse::PositionRawY = (int)yPosition;

				//
				//Scaled
				//
				SGE::Inputs::Mouse::PositionX = ((SGE::Inputs::Mouse::PositionRawX - currentFrameBufferXOffset) * SGE::Display::Video::X) / (currentFrameBufferX - currentFrameBufferXOffset * 2);
				SGE::Inputs::Mouse::PositionY = ((SGE::Inputs::Mouse::PositionRawY - currentFrameBufferYOffset) * SGE::Display::Video::Y) / (currentFrameBufferY - currentFrameBufferYOffset * 2);
			}

			//
			//  Function to register callbacks for Input related functions
			//
			void Register()
			{
				//
				//  Mouse scrool wheel callback
				//
				glfwSetScrollCallback(SGE::OSWindow, SGE::Inputs::Callbacks::ScrollWheel);

				//
				//  Mouse button callback
				//
				glfwSetMouseButtonCallback(SGE::OSWindow, SGE::Inputs::Callbacks::MouseButton);

				//
				//  Mouse cursor callback
				//
				glfwSetCursorPosCallback(SGE::OSWindow, SGE::Inputs::Callbacks::Cursor);

				//
				//  Keyboard callback
				//
				glfwSetKeyCallback(SGE::OSWindow, SGE::Inputs::Callbacks::Keyboard);
			}
		}


		//
		//  Mouse namespace that contains global variables containing mouse positioning data.
		//
		namespace Mouse
		{
			//
			//  Mapped Mouse position relative to the game screen
			//
			int PositionX = 0;
			int PositionY = 0;

			//
			//  Raw Mouse position data reported from the OS relative to the OS window
			//
			int PositionRawX = 0;
			int PositionRawY = 0;

			//
			//  Scroll position data
			//
			int ScrollX = 0;
			int ScrollY = 0;

			//
			//  Flag array for button statuses
			//
			bool Buttons[NUMBER_OF_BUTTONS] = { false };
		}

		//
		//  Keyboard namespace that contains global variables containing keyboard key statuses
		//
		namespace Keyboard
		{
			//
			//  Keyboard status array that keeps track of which keys are pressed
			//
			bool Status[NUMBER_OF_KEYS] = { false };

			//
			//  Copy current keyboard state to another array
			//
			void SaveStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS])
			{
				//std::memcpy(targetKeyboardStatusArray, Status, sizeof(Status));
				std::copy(Status, Status + NUMBER_OF_KEYS, targetKeyboardStatusArray);
			}

			//
			//  Function to convert a keyboard status to ASCII characters
			//
			int StatusToASCII(bool targetKeyboardStatusArray[NUMBER_OF_KEYS], char* targetASCIIArray, int maxASCIICharacters)
			{
				//
				//  Scan through array for keys that map to ASCII characters
				//

				//  Check status of modifier keys
				
				//  Check letter keys
	

				//  Check nubmer keys
				
				return 0;
			}

		}

		//
		//  Flag to indicate to the handle events thread to continue or not
		//
		bool ContinueToHandleEvents = true;

		//
		//  This function is called when the system is suppose to start Handling Events through Main Interface
		//  This is usually the last thing called and is usually reserved for for the thread that spawned the GUI window...  because MacOS only reports events to the thread that spawned the GUI window of any process.
		//  Stupid Macs... Really?
		void HandleEvents()
		{
			//While the main window is going and isn't set to close...
			while (!glfwWindowShouldClose(SGE::OSWindow) && ContinueToHandleEvents)
			{
				//Poll for events
				glfwPollEvents();

				//Wait for new events
				glfwWaitEvents();
			}
		}
	}
}
