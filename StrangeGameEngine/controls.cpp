#include "include\SGE\controls.h"
#include "sharedinternal.h"
#include <GLFW\glfw3.h>
#include <thread>

namespace SGE
{
	namespace Controls
	{
		//Keyboard status array that keeps track of which keys are pressed
		bool KeyboardStatus[NUMBER_OF_KEYS] = { false };

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

				if (action == GLFW_RELEASE)
				{
					KeyboardStatus[key] = false;
				}
			}
		}

		//Mouse Cursor callback for GLFW
		void CursorCallback(GLFWwindow* window, double xPosition, double yPosition)
		{

		}

		//Mouse Button callback for GLFW
		void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
		{

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


			//Start Handling Events through Main Interface
			//This has to handled by the same thread that spawned the GUI window
			//And in OSX this has to be done by the thread that invoked the main function of the program..  Fuckin' Macs...

			//While the main window is going and isn't set to close...
			while (!glfwWindowShouldClose(SGE::mainWindow))
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
			memcpy(targetKeyboardStatusArray, KeyboardStatus, sizeof(KeyboardStatus));
		}
	}
}