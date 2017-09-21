#pragma once
namespace SGE
{
	namespace Controls
	{
		//Values for the key and scan code arrays
		const int NUMBER_OF_KEYS = 512;
		const int NUMBER_OF_SCANCODES = 512;

		//Current keyboard key state array
		//It's an extern to keep the linker from going crazy over multiple definitions between the headers.
		//It's officially defined in the control.cpp
		extern bool keyboardStatus[NUMBER_OF_KEYS];

		//Current keyboard scancode state array
		//It's an extern to keep the linker from being confused
		//Officially, defined in the control.cpp
		extern bool keyboardScancodeStatus[NUMBER_OF_KEYS];

		//This is a helper class to save the state of the keyboard and provide easy access to commonly used functions
		class SavedKeyboardState
		{
		private:
			//Previous state array
			bool previousKeyboardState[NUMBER_OF_KEYS];
			bool previousKeyboardScancodeState[NUMBER_OF_KEYS];

		public:
			//Default Constructor
			//Interface to pull state from is REQUIRED
			SavedKeyboardState();

			//Save the current state from the linked interface
			void SaveState();

			//Function to check to see if a key has changed between current state and previous state
			bool KeyChanged(int key);

			//Find out what the previous statew as for a key
			bool GetPreviousState(int key);
		};

	}

}