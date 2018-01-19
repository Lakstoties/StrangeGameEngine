#include "include\SGE\controls.h"
#include "sharedinternal.h"
#include <GLFW\glfw3.h>
#include <thread>
#include <algorithm>

namespace SGE
{
	namespace Controls
	{
		namespace Mouse
		{
			int PositionX = 0;
			int PositionRawX = 0;

			int PositionY = 0;
			int PositionRawY = 0;

			int ScrollX = 0;
			int ScrollY = 0;

			bool Buttons[NUMBER_OF_BUTTONS] = { false };
		}

		namespace Keyboard
		{
			//Keyboard status array that keeps track of which keys are pressed
			bool Status[NUMBER_OF_KEYS] = { false };

			//Current keyboard input buffer
			int InputBuffer[INPUT_BUFFER_SIZE] = { 0 };

			//Current buffer positions
			int CurrentBufferPosition = 0;

			//Clear and reset the buffer.
			void ResetInputBuffer()
			{
				//Reset position
				CurrentBufferPosition = 0;

				//Clear out all the data in the buffer
				std::memset(InputBuffer, 0, sizeof(InputBuffer));
			}


			//Copy current keyboard state to another array
			void SaveStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS])
			{
				memcpy(targetKeyboardStatusArray, Status, sizeof(Status));
			}
		}


		//Flag to indicate to the handle events threads to continue or not
		bool ContinueToHandleEvents = true;

		void HandleEvents()
		{
			//Start Handling Events through Main Interface
			//This has to handled by the same thread that spawned the GUI window
			//And in OSX this has to be done by the thread that invoked the main function of the program..  Fuckin' Macs...

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
