#pragma once
//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Controls namespace that handles input from keyboards, mice, and other controllers
	//
	namespace Controls
	{
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
			const int KEY_SPACE =		32;		//' ' 
			const int KEY_APOSTROPHE =	39;		// ' 
			const int KEY_COMMA =		44;		// ,
			const int KEY_MINUS =		45;		// - 
			const int KEY_PERIOD =		46;		// . 
			const int KEY_SLASH =		47;		// / 
			const int KEY_0 =			48;		// 0
			const int KEY_1 =			49;		// 1
			const int KEY_2 =           50;		// 2
			const int KEY_3 =           51;		// 3
			const int KEY_4 =           52;		// 4
			const int KEY_5 =           53;		// 5
			const int KEY_6 =			54;		// 6
			const int KEY_7 =			55;		// 7
			const int KEY_8 =			56;		// 8
			const int KEY_9 =			57;		// 9
			const int KEY_SEMICOLON =	59;		// ; 
			const int KEY_EQUAL =		61;		// = 
			const int KEY_A =			65;		// a/A
			const int KEY_B =			66;		// b/B
			const int KEY_C =			67;		// c/C
			const int KEY_D =			68;		// d/D
			const int KEY_E =			69;		// e/E
			const int KEY_F =			70;		// f/F
			const int KEY_G =			71;		// g/G
			const int KEY_H =			72;		// h/H
			const int KEY_I =			73;		// i/I
			const int KEY_J =			74;		// j/J
			const int KEY_K =			75;		// k/K
			const int KEY_L =			76;		// l/L
			const int KEY_M =			77;		// m/M
			const int KEY_N =			78;		// n/N
			const int KEY_O =			79;		// o/O
			const int KEY_P =			80;		// p/P
			const int KEY_Q =			81;		// q/Q
			const int KEY_R =			82;		// r/R
			const int KEY_S =			83;		// s/S
			const int KEY_T =			84;		// t/T
			const int KEY_U =			85;		// u/U
			const int KEY_V =			86;		// v/V
			const int KEY_W =			87;		// w/W
			const int KEY_X =			88;		// x/X
			const int KEY_Y =			89;		// y/Y
			const int KEY_Z =			90;		// z/Z
			const int KEY_LEFT_BRACKET = 91;	// [ 
			const int KEY_BACKSLASH =	92;		// "\" 
			const int KEY_RIGHT_BRACKET = 93;	// ] 
			const int KEY_GRAVE_ACCENT = 96;	// `
			const int KEY_WORLD_1 =		161;	// non-US #1
			const int KEY_WORLD_2 =		162;	// non-US #2

			// Function keys
			const int KEY_ESCAPE =		256;	// Esc
			const int KEY_ENTER =		257;	// Enter
			const int KEY_TAB =			258;	// Tab
			const int KEY_BACKSPACE =	259;	// Backspace
			const int KEY_INSERT =		260;	// Insert
			const int KEY_DELETE =		261;	// Delete
			const int KEY_RIGHT =		262;	// ->
			const int KEY_LEFT =		263;	// <-
			const int KEY_DOWN =		264;	// v
			const int KEY_UP =			265;	// ^
			const int KEY_PAGE_UP =		266;	// Page Up
			const int KEY_PAGE_DOWN =	267;	// Page Down
			const int KEY_HOME =		268;	// Home
			const int KEY_END =			269;	// End
			const int KEY_CAPS_LOCK =	280;	// Caps Lock
			const int KEY_SCROLL_LOCK = 281;	// Scroll Lock
			const int KEY_NUM_LOCK =	282;	// Num Lock
			const int KEY_PRINT_SCREEN = 283;	// Print Screen
			const int KEY_PAUSE =		284;	// Pause
			const int KEY_F1 =			290;	// F1
			const int KEY_F2 =			291;	// F2
			const int KEY_F3 =			292;	// F3
			const int KEY_F4 =			293;	// F4
			const int KEY_F5 =			294;	// F5
			const int KEY_F6 =			295;	// F6
			const int KEY_F7 =			296;	// F7
			const int KEY_F8 =			297;	// F8
			const int KEY_F9 =			298;	// F9
			const int KEY_F10 =			299;	// F10
			const int KEY_F11 =			300;	// F11
			const int KEY_F12 =			301;	// F12
			const int KEY_F13 =			302;	// F13
			const int KEY_F14 =			303;	// F14
			const int KEY_F15 =			304;	// F15
			const int KEY_F16 =			305;	// F16
			const int KEY_F17 =			306;	// F17
			const int KEY_F18 =			307;	// F18
			const int KEY_F19 =			308;	// F19
			const int KEY_F20 =			309;	// F20
			const int KEY_F21 =			310;	// F21
			const int KEY_F22 =			311;	// F22
			const int KEY_F23 =			312;	// F23
			const int KEY_F24 =			313;	// F24
			const int KEY_F25 =			314;	// F25
			const int KEY_KP_0 =		320;	// Keypad 0
			const int KEY_KP_1 =		321;	// Keypad 1
			const int KEY_KP_2 =		322;	// Keypad 2
			const int KEY_KP_3 =		323;	// Keypad 3
			const int KEY_KP_4 =		324;	// Keypad 4
			const int KEY_KP_5 =		325;	// Keypad 5
			const int KEY_KP_6 =		326;	// Keypad 6
			const int KEY_KP_7 =		327;	// Keypad 7
			const int KEY_KP_8 =		328;	// Keypad 8
			const int KEY_KP_9 =		329;	// Keypad 9
			const int KEY_KP_DECIMAL =	330;	// Keypad .
			const int KEY_KP_DIVIDE =	331;	// Keypad /
			const int KEY_KP_MULTIPLY = 332;	// Keypad *
			const int KEY_KP_SUBTRACT = 333;	// Keypad -
			const int KEY_KP_ADD =		334;	// Keypad +
			const int KEY_KP_ENTER =	335;	// Keypad Enter
			const int KEY_KP_EQUAL =	336;	// Keypad =
			const int KEY_LEFT_SHIFT =	340;	// Left Shift
			const int KEY_LEFT_CONTROL = 341;	// Left Control
			const int KEY_LEFT_ALT =	342;	// Left Alt
			const int KEY_LEFT_SUPER =	343;	// Left Super/Windows
			const int KEY_RIGHT_SHIFT = 344;	// Right Shift	
			const int KEY_RIGHT_CONTROL = 345;	// Right Control
			const int KEY_RIGHT_ALT = 346;		// Right Alt
			const int KEY_RIGHT_SUPER = 347;	// Right Super/Windows
			const int KEY_MENU = 348;			// Menu

			const int KEY_LAST = KEY_MENU;		// Last
		}
	}
}