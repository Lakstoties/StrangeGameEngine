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


		void HandleEvents()
		{
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
