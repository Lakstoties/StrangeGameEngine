#pragma once
//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Inputs namespace that handles input from keyboards, mice, and other controllers
	//
	namespace Inputs
	{
		//
		//  Callbacks namespace for callbacks related to Inputs
		//
		namespace  Callbacks
		{
			//
			//  Function to register callbacks for Input related functions
			//
			void Register();
		}

		//
		//  Mouse namespace that presents globals to maintain current position and button status of the mouse
		//
		namespace Mouse
		{
			//
			//  Number of Button possible for a mouse
			//
			const int NUMBER_OF_BUTTONS = 10;

			//
			//  X Position Data
			//
			extern int PositionRawX;	//Raw Position Data from the OS
			extern int PositionX;		//Translated position data mapped to the game window

			//
			//  Y Position Data
			//
			extern int PositionRawY;	//Raw Position Data from the OS
			extern int PositionY;		//Translated position data mapped to the game window

			//
			//  Button Status array
			//
			extern bool Buttons[NUMBER_OF_BUTTONS];

			//
			//  Scroll Wheel Position data
			//
			extern int ScrollX;
			extern int ScrollY;
		}

		//
		//  Keyboard namespace that presents globals to maintain the current status of key on the keyboard and recent input
		//
		namespace Keyboard
		{
			//
			//  Values for the key and scan code arrays
			//
			const int NUMBER_OF_KEYS = 512;

			//
			//  Extern to declare the keyboard key state array
			//
			extern bool Status[NUMBER_OF_KEYS];

			//
			//  Function to save the current keyboard status array to another array
			//
			void SaveStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS]);

			//
			//  Function to convert a keyboard status to ASCII characters
			//
			int StatusToASCII(bool targetKeyboardStatusArray[NUMBER_OF_KEYS], char* targetASCIIArray, int maxASCIICharacters);
		}

		//
		//  Flag to indicate if the input handling system should keep on going.
		//
		extern bool ContinueToHandleEvents;

		//Start Handling Events through Main Interface
		//This has to handled by the same thread that spawned the GUI window
		//And in OSX this has to be done by the thread that invoked the main function of the program..  @#$%!#$^' Macs...
		void HandleEvents();

		///
		///Keymap
		///

		namespace Keymap
		{
			//Printable keys
			extern const int KEY_SPACE;			//' ' 
			extern const int KEY_APOSTROPHE;	// ' 
			extern const int KEY_COMMA;			// ,
			extern const int KEY_MINUS;			// - 
			extern const int KEY_PERIOD;		// . 
			extern const int KEY_SLASH;			// / 
			extern const int KEY_0;				// 0
			extern const int KEY_1;				// 1
			extern const int KEY_2;				// 2
			extern const int KEY_3;				// 3
			extern const int KEY_4;				// 4
			extern const int KEY_5;				// 5
			extern const int KEY_6;				// 6
			extern const int KEY_7;				// 7
			extern const int KEY_8;				// 8
			extern const int KEY_9;				// 9
			extern const int KEY_SEMICOLON;		// ; 
			extern const int KEY_EQUAL;			// = 
			extern const int KEY_A;				// a/A
			extern const int KEY_B;				// b/B
			extern const int KEY_C;				// c/C
			extern const int KEY_D;				// d/D
			extern const int KEY_E;				// e/E
			extern const int KEY_F;				// f/F
			extern const int KEY_G;				// g/G
			extern const int KEY_H;				// h/H
			extern const int KEY_I;				// i/I
			extern const int KEY_J;				// j/J
			extern const int KEY_K;				// k/K
			extern const int KEY_L;				// l/L
			extern const int KEY_M;				// m/M
			extern const int KEY_N;				// n/N
			extern const int KEY_O;				// o/O
			extern const int KEY_P;				// p/P
			extern const int KEY_Q;				// q/Q
			extern const int KEY_R;				// r/R
			extern const int KEY_S;				// s/S
			extern const int KEY_T;				// t/T
			extern const int KEY_U;				// u/U
			extern const int KEY_V;				// v/V
			extern const int KEY_W;				// w/W
			extern const int KEY_X;				// x/X
			extern const int KEY_Y;				// y/Y
			extern const int KEY_Z;				// z/Z
			extern const int KEY_LEFT_BRACKET;	// [ 
			extern const int KEY_BACKSLASH;		// "\" 
			extern const int KEY_RIGHT_BRACKET;	// ] 
			extern const int KEY_GRAVE_ACCENT;	// `
			extern const int KEY_WORLD_1;		// non-US #1
			extern const int KEY_WORLD_2;		// non-US #2

			// Function keys
			extern const int KEY_ESCAPE;		// Esc
			extern const int KEY_ENTER;			// Enter
			extern const int KEY_TAB;			// Tab
			extern const int KEY_BACKSPACE;		// Backspace
			extern const int KEY_INSERT;		// Insert
			extern const int KEY_DELETE;		// Delete
			extern const int KEY_RIGHT;			// ->
			extern const int KEY_LEFT;			// <-
			extern const int KEY_DOWN;			// v
			extern const int KEY_UP;			// ^
			extern const int KEY_PAGE_UP;		// Page Up
			extern const int KEY_PAGE_DOWN;		// Page Down
			extern const int KEY_HOME;			// Home
			extern const int KEY_END;			// End
			extern const int KEY_CAPS_LOCK;		// Caps Lock
			extern const int KEY_SCROLL_LOCK;	// Scroll Lock
			extern const int KEY_NUM_LOCK;		// Num Lock
			extern const int KEY_PRINT_SCREEN;	// Print Screen
			extern const int KEY_PAUSE;			// Pause
			extern const int KEY_F1;			// F1
			extern const int KEY_F2;			// F2
			extern const int KEY_F3;			// F3
			extern const int KEY_F4;			// F4
			extern const int KEY_F5;			// F5
			extern const int KEY_F6;			// F6
			extern const int KEY_F7;			// F7
			extern const int KEY_F8;			// F8
			extern const int KEY_F9;			// F9
			extern const int KEY_F10;			// F10
			extern const int KEY_F11;			// F11
			extern const int KEY_F12;			// F12
			extern const int KEY_F13;			// F13
			extern const int KEY_F14;			// F14
			extern const int KEY_F15;			// F15
			extern const int KEY_F16;			// F16
			extern const int KEY_F17;			// F17
			extern const int KEY_F18;			// F18
			extern const int KEY_F19;			// F19
			extern const int KEY_F20;			// F20
			extern const int KEY_F21;			// F21
			extern const int KEY_F22;			// F22
			extern const int KEY_F23;			// F23
			extern const int KEY_F24;			// F24
			extern const int KEY_F25;			// F25
			extern const int KEY_KP_0;			// Keypad 0
			extern const int KEY_KP_1;			// Keypad 1
			extern const int KEY_KP_2;			// Keypad 2
			extern const int KEY_KP_3;			// Keypad 3
			extern const int KEY_KP_4;			// Keypad 4
			extern const int KEY_KP_5;			// Keypad 5
			extern const int KEY_KP_6;			// Keypad 6
			extern const int KEY_KP_7;			// Keypad 7
			extern const int KEY_KP_8;			// Keypad 8
			extern const int KEY_KP_9;			// Keypad 9
			extern const int KEY_KP_DECIMAL;	// Keypad .
			extern const int KEY_KP_DIVIDE;		// Keypad /
			extern const int KEY_KP_MULTIPLY;	// Keypad *
			extern const int KEY_KP_SUBTRACT;	// Keypad -
			extern const int KEY_KP_ADD;		// Keypad +
			extern const int KEY_KP_ENTER;		// Keypad Enter
			extern const int KEY_KP_EQUAL;		// Keypad =
			extern const int KEY_LEFT_SHIFT;	// Left Shift
			extern const int KEY_LEFT_CONTROL;	// Left Control
			extern const int KEY_LEFT_ALT;		// Left Alt
			extern const int KEY_LEFT_SUPER;	// Left Super/Windows
			extern const int KEY_RIGHT_SHIFT;	// Right Shift	
			extern const int KEY_RIGHT_CONTROL;	// Right Control
			extern const int KEY_RIGHT_ALT;		// Right Alt
			extern const int KEY_RIGHT_SUPER;	// Right Super/Windows
			extern const int KEY_MENU;			// Menu

			extern const int KEY_LAST;			// Last
		}
	}
}