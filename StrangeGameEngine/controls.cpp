#include "include\SGE\controls.h"
#include <GLFW\glfw3.h>
#include <thread>

namespace SGE
{
	namespace Controls
	{
		//Keyboard status array that keeps track of which keys are pressed
		bool keyboardStatus[NUMBER_OF_KEYS];

		//Keyboard status array that keeps track of which scancodes have been triggered
		bool keyboardScancodeStatus[NUMBER_OF_KEYS];


		//Keyboard callback for GLFW
		void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			//Update the key array for the state of the key
			if (action == GLFW_PRESS)
			{
				keyboardStatus[key] = true;
				keyboardScancodeStatus[scancode] = true;
			}

			else if (action == GLFW_RELEASE)
			{
				keyboardStatus[key] = false;
				keyboardScancodeStatus[scancode] = false;
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

		void HandleEvents(GLFWwindow* targetWindow)
		{
			//Initializations

			//Initialize the keyboard status array
			for (int i = 0; i < 128; i++)
			{
				keyboardStatus[i] = false;
				keyboardScancodeStatus[i] = false;
			}

			//
			//Register the callbacks
			//

			//Keyboard callback
			glfwSetKeyCallback(targetWindow, KeyboardCallback);

			//Mouse cursor callback
			glfwSetCursorPosCallback(targetWindow, CursorCallback);

			//Mouse button callback
			glfwSetMouseButtonCallback(targetWindow, MouseButtonCallback);


			//Start Handling Events through Main Interface
			//This has to handled by the same thread that spawned the GUI window
			//And in OSX this has to be done by the thread that invoked the main function of the program..  Fuckin' Macs...

			//While the main window is going and isn't set to close...
			while (!glfwWindowShouldClose(targetWindow))
			{
				//Poll for events
				glfwPollEvents();

				//Wait for new events
				glfwWaitEvents();
			}
		}
		
		SavedKeyboardState::SavedKeyboardState()
		{
			//Populate Keyboard State from Interface
			this->SaveState();
		}

		void SavedKeyboardState::SaveState()
		{
			for (int i = 0; i < NUMBER_OF_KEYS; i++)
			{
				previousKeyboardState[i] = keyboardStatus[i];
			}
		}

		bool SavedKeyboardState::KeyChanged(int key)
		{
			return previousKeyboardState[key] != keyboardStatus[key];
		}

		bool SavedKeyboardState::GetPreviousState(int key)
		{
			return previousKeyboardState[key];
		}
	}
}