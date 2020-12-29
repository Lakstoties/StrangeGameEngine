#pragma once
#include "api.h"
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
		enum Keymap : int
		{
			//
			//  Printable keys
			//
			SPACE		= 32,	//' ' 
			APOSTROPHE	= 39,	// ' 
			COMMA		= 44,	// ,
			MINUS		= 45,	// - 
			PERIOD		= 46,	// . 
			SLASH		= 47,	// / 
			ZERO		= 48,	// 0
			ONE			= 49,	// 1
			TWO			= 50,	// 2
			THREE		= 51,	// 3
			FOUR		= 52,	// 4
			FIVE		= 53,	// 5
			SIX			= 54,	// 6
			SEVEN		= 55,	// 7
			EIGHT		= 56,	// 8
			NINE		= 57,	// 9
			SEMICOLON	= 59,	// ; 
			EQUAL		= 61,	// = 
			A			= 65,	// a/A
			B			= 66,	// b/B
			C			= 67,	// c/C
			D			= 68,	// d/D
			E			= 69,	// e/E
			F			= 70,	// f/F
			G			= 71,	// g/G
			H			= 72,	// h/H
			I			= 73,	// i/I
			J			= 74,	// j/J
			K			= 75,	// k/K
			L			= 76,	// l/L
			M			= 77,	// m/M
			N			= 78,	// n/N
			O			= 79,	// o/O
			P			= 80,	// p/P
			Q			= 81,	// q/Q
			R			= 82,	// r/R
			S			= 83,	// s/S
			T			= 84,	// t/T
			U			= 85,	// u/U
			V			= 86,	// v/V
			W			= 87,	// w/W
			X			= 88,	// x/X
			Y			= 89,	// y/Y
			Z			= 90,	// z/Z
			LEFT_BRACKET = 91,	// [ 
			BACKSLASH	= 92,		// "\" 
			RIGHT_BRACKET = 93,	// ] 
			GRAVE_ACCENT = 96,	// `
			WORLD_1		= 161,		// non-US #1
			WORLD_2		= 162,		// non-US #2

			//
			// Function keys
			//
			ESCAPE		= 256,	// Esc
			ENTER		= 257,	// Enter
			TAB			= 258,	// Tab
			BACKSPACE	= 259,	// Backspace
			INSERT		= 260,	// Insert
			DELETE		= 261,	// Delete
			RIGHT		= 262,	// ->
			LEFT		= 263,	// <-
			DOWN		= 264,	// v
			UP			= 265,	// ^
			PAGE_UP		= 266,	// Page Up
			PAGE_DOWN	= 267,	// Page Down
			HOME		= 268,	// Home
			END			= 269,	// End
			CAPS_LOCK	= 280,	// Caps Lock
			SCROLL_LOCK = 281,	// Scroll Lock
			NUM_LOCK	= 282,	// Num Lock
			PRINT_SCREEN = 283,	// Print Screen
			PAUSE		= 284,	// Pause
			F1			= 290,	// F1
			F2			= 291,	// F2
			F3			= 292,	// F3
			F4			= 293,	// F4
			F5			= 294,	// F5
			F6			= 295,	// F6
			F7			= 296,	// F7
			F8			= 297,	// F8
			F9			= 298,	// F9
			F10			= 299,	// F10
			F11			= 300,	// F11
			F12			= 301,	// F12
			F13			= 302,	// F13
			F14			= 303,	// F14
			F15			= 304,	// F15
			F16			= 305,	// F16
			F17			= 306,	// F17
			F18			= 307,	// F18
			F19			= 308,	// F19
			F20			= 309,	// F20
			F21			= 310,	// F21
			F22			= 311,	// F22
			F23			= 312,	// F23
			F24			= 313,	// F24
			F25			= 314,	// F25
			KP_0		= 320,	// Keypad 0
			KP_1		= 321,	// Keypad 1
			KP_2		= 322,	// Keypad 2
			KP_3		= 323,	// Keypad 3
			KP_4		= 324,	// Keypad 4
			KP_5		= 325,	// Keypad 5
			KP_6		= 326,	// Keypad 6
			KP_7		= 327,	// Keypad 7
			KP_8		= 328,	// Keypad 8
			KP_9		= 329,	// Keypad 9
			KP_DECIMAL	= 330,	// Keypad .
			KP_DIVIDE	= 331,	// Keypad /
			KP_MULTIPLY = 332,	// Keypad *
			KP_SUBTRACT = 333,	// Keypad -
			KP_ADD		= 334,	// Keypad +
			KP_ENTER	= 335,	// Keypad Enter
			KP_EQUAL	= 336,	// Keypad =
			LEFT_SHIFT	= 340,	// Left Shift
			LEFT_CONTROL = 341,	// Left Control
			LEFT_ALT	= 342,	// Left Alt
			LEFT_SUPER	= 343,	// Left Super/Windows
			RIGHT_SHIFT = 344,	// Right Shift	
			RIGHT_CONTROL = 345,// Right Control
			RIGHT_ALT	= 346,	// Right Alt
			RIGHT_SUPER = 347,	// Right Super/Windows
			MENU		= 348,	// Menu

			LAST		= 348	// Last
		};
	}
}