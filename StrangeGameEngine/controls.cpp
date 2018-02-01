#include "include\SGE\controls.h"
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
	//  Controls namespace that handles input from keyboards, mice, and other controllers
	//
	namespace Controls
	{
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
			//  Current keyboard input buffer
			//
			int InputBuffer[INPUT_BUFFER_SIZE] = { 0 };

			//
			//  Current buffer positions
			//
			int CurrentBufferPosition = 0;

			//
			//  Clear and reset the buffer.
			//
			void ResetInputBuffer()
			{
				//
				//  Reset position
				//
				CurrentBufferPosition = 0;

				//
				//  Clear out all the data in the buffer
				//
				std::memset(InputBuffer, 0, sizeof(InputBuffer));
			}

			//
			//  Copy current keyboard state to another array
			//
			void SaveStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS])
			{
				std::memcpy(targetKeyboardStatusArray, Status, sizeof(Status));
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
