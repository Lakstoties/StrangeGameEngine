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
			//  Copy current keyboard state to another array
			//
			void SaveStatus(bool targetKeyboardStatusArray[NUMBER_OF_KEYS])
			{
				std::memcpy(targetKeyboardStatusArray, Status, sizeof(Status));
			}

			//
			//  Function to convert a keyboard status to ASCII characters
			//
			int StatusToASCII(bool targetKeyboardStatusArray[NUMBER_OF_KEYS], char* targetASCIIArray, int maxASCIICharacters)
			{
				//
				//  Scan through array for keys that map to ASCII characters
				//
				
				//  Check letter keys
			

				//  Check nubmer keys
				
				return 0;
			}

		}

		//
		//  Keymappings from External SGE to Internal GLFW
		//
		namespace Keymap
		{
			//
			//  Printable keys
			//
			const int KEY_SPACE = GLFW_KEY_SPACE;			//' ' 
			const int KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE;	// ' 
			const int KEY_COMMA = GLFW_KEY_COMMA;			// ,
			const int KEY_MINUS = GLFW_KEY_MINUS;			// - 
			const int KEY_PERIOD = GLFW_KEY_PERIOD;			// . 
			const int KEY_SLASH = GLFW_KEY_SLASH;			// / 
			const int KEY_0 = GLFW_KEY_0;					// 0
			const int KEY_1 = GLFW_KEY_1;					// 1
			const int KEY_2 = GLFW_KEY_2;					// 2
			const int KEY_3 = GLFW_KEY_3;					// 3
			const int KEY_4 = GLFW_KEY_4;					// 4
			const int KEY_5 = GLFW_KEY_5;					// 5
			const int KEY_6 = GLFW_KEY_6;					// 6
			const int KEY_7 = GLFW_KEY_7;					// 7
			const int KEY_8 = GLFW_KEY_8;					// 8
			const int KEY_9 = GLFW_KEY_9;					// 9
			const int KEY_SEMICOLON = GLFW_KEY_SEMICOLON;	// ; 
			const int KEY_EQUAL = GLFW_KEY_EQUAL;			// = 
			const int KEY_A = GLFW_KEY_A;					// a/A
			const int KEY_B = GLFW_KEY_B;					// b/B
			const int KEY_C = GLFW_KEY_C;					// c/C
			const int KEY_D = GLFW_KEY_D;					// d/D
			const int KEY_E = GLFW_KEY_E;					// e/E
			const int KEY_F = GLFW_KEY_F;					// f/F
			const int KEY_G = GLFW_KEY_G;					// g/G
			const int KEY_H = GLFW_KEY_H;					// h/H
			const int KEY_I = GLFW_KEY_I;					// i/I
			const int KEY_J = GLFW_KEY_J;					// j/J
			const int KEY_K = GLFW_KEY_K;					// k/K
			const int KEY_L = GLFW_KEY_L;					// l/L
			const int KEY_M = GLFW_KEY_M;					// m/M
			const int KEY_N = GLFW_KEY_N;					// n/N
			const int KEY_O = GLFW_KEY_O;					// o/O
			const int KEY_P = GLFW_KEY_P;					// p/P
			const int KEY_Q = GLFW_KEY_Q;					// q/Q
			const int KEY_R = GLFW_KEY_R;					// r/R
			const int KEY_S = GLFW_KEY_S;					// s/S
			const int KEY_T = GLFW_KEY_T;					// t/T
			const int KEY_U = GLFW_KEY_U;					// u/U
			const int KEY_V = GLFW_KEY_V;					// v/V
			const int KEY_W = GLFW_KEY_W;					// w/W
			const int KEY_X = GLFW_KEY_X;					// x/X
			const int KEY_Y = GLFW_KEY_Y;					// y/Y
			const int KEY_Z = GLFW_KEY_Z;					// z/Z
			const int KEY_LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET;	// [ 
			const int KEY_BACKSLASH = GLFW_KEY_BACKSLASH;		// "\" 
			const int KEY_RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET;	// ] 
			const int KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT;	// `
			const int KEY_WORLD_1 = GLFW_KEY_WORLD_1;		// non-US #1
			const int KEY_WORLD_2 = GLFW_KEY_WORLD_2;		// non-US #2

			//
			// Function keys
			//
			const int KEY_ESCAPE = GLFW_KEY_ESCAPE;				// Esc
			const int KEY_ENTER = GLFW_KEY_ENTER;				// Enter
			const int KEY_TAB = GLFW_KEY_TAB;					// Tab
			const int KEY_BACKSPACE = GLFW_KEY_BACKSPACE;		// Backspace
			const int KEY_INSERT = GLFW_KEY_INSERT;				// Insert
			const int KEY_DELETE = GLFW_KEY_DELETE;				// Delete
			const int KEY_RIGHT = GLFW_KEY_RIGHT;				// ->
			const int KEY_LEFT = GLFW_KEY_LEFT;					// <-
			const int KEY_DOWN = GLFW_KEY_DOWN;					// v
			const int KEY_UP = GLFW_KEY_UP;						// ^
			const int KEY_PAGE_UP = GLFW_KEY_PAGE_UP;			// Page Up
			const int KEY_PAGE_DOWN = GLFW_KEY_PAGE_DOWN;		// Page Down
			const int KEY_HOME = GLFW_KEY_HOME;					// Home
			const int KEY_END = GLFW_KEY_END;					// End
			const int KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK;		// Caps Lock
			const int KEY_SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK;	// Scroll Lock
			const int KEY_NUM_LOCK = GLFW_KEY_NUM_LOCK;			// Num Lock
			const int KEY_PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN;	// Print Screen
			const int KEY_PAUSE = GLFW_KEY_PAUSE;				// Pause
			const int KEY_F1 = GLFW_KEY_F1;						// F1
			const int KEY_F2 = GLFW_KEY_F2;						// F2
			const int KEY_F3 = GLFW_KEY_F3;						// F3
			const int KEY_F4 = GLFW_KEY_F4;						// F4
			const int KEY_F5 = GLFW_KEY_F5;						// F5
			const int KEY_F6 = GLFW_KEY_F6;						// F6
			const int KEY_F7 = GLFW_KEY_F7;						// F7
			const int KEY_F8 = GLFW_KEY_F8;						// F8
			const int KEY_F9 = GLFW_KEY_F9;						// F9
			const int KEY_F10 = GLFW_KEY_F10;					// F10
			const int KEY_F11 = GLFW_KEY_F11;					// F11
			const int KEY_F12 = GLFW_KEY_F12;					// F12
			const int KEY_F13 = GLFW_KEY_F13;					// F13
			const int KEY_F14 = GLFW_KEY_F14;					// F14
			const int KEY_F15 = GLFW_KEY_F15;					// F15
			const int KEY_F16 = GLFW_KEY_F16;					// F16
			const int KEY_F17 = GLFW_KEY_F17;					// F17
			const int KEY_F18 = GLFW_KEY_F18;					// F18
			const int KEY_F19 = GLFW_KEY_F19;					// F19
			const int KEY_F20 = GLFW_KEY_F20;					// F20
			const int KEY_F21 = GLFW_KEY_F21;					// F21
			const int KEY_F22 = GLFW_KEY_F22;					// F22
			const int KEY_F23 = GLFW_KEY_F23;					// F23
			const int KEY_F24 = GLFW_KEY_F24;					// F24
			const int KEY_F25 = GLFW_KEY_F25;					// F25
			const int KEY_KP_0 = GLFW_KEY_KP_0;					// Keypad 0
			const int KEY_KP_1 = GLFW_KEY_KP_1;					// Keypad 1
			const int KEY_KP_2 = GLFW_KEY_KP_2;					// Keypad 2
			const int KEY_KP_3 = GLFW_KEY_KP_3;					// Keypad 3
			const int KEY_KP_4 = GLFW_KEY_KP_4;					// Keypad 4
			const int KEY_KP_5 = GLFW_KEY_KP_5;					// Keypad 5
			const int KEY_KP_6 = GLFW_KEY_KP_6;					// Keypad 6
			const int KEY_KP_7 = GLFW_KEY_KP_7;					// Keypad 7
			const int KEY_KP_8 = GLFW_KEY_KP_8;					// Keypad 8
			const int KEY_KP_9 = GLFW_KEY_KP_9;					// Keypad 9
			const int KEY_KP_DECIMAL = GLFW_KEY_KP_DECIMAL;		// Keypad .
			const int KEY_KP_DIVIDE = GLFW_KEY_KP_DIVIDE;		// Keypad /
			const int KEY_KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY;	// Keypad *
			const int KEY_KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT;	// Keypad -
			const int KEY_KP_ADD = GLFW_KEY_KP_ADD;				// Keypad +
			const int KEY_KP_ENTER = GLFW_KEY_KP_ENTER;			// Keypad Enter
			const int KEY_KP_EQUAL = GLFW_KEY_KP_EQUAL;			// Keypad =
			const int KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT;		// Left Shift
			const int KEY_LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL;	// Left Control
			const int KEY_LEFT_ALT = GLFW_KEY_LEFT_ALT;			// Left Alt
			const int KEY_LEFT_SUPER = GLFW_KEY_LEFT_SUPER;		// Left Super/Windows
			const int KEY_RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT;	// Right Shift	
			const int KEY_RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL;	// Right Control
			const int KEY_RIGHT_ALT = GLFW_KEY_RIGHT_ALT;		// Right Alt
			const int KEY_RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER;	// Right Super/Windows
			const int KEY_MENU = GLFW_KEY_MENU;					// Menu

			const int KEY_LAST = KEY_MENU;						// Last
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
