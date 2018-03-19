#pragma once

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  This namespace contains items related to the OS interaction, debugging, logging, and other system features
	//
	namespace System
	{
		namespace Callbacks
		{
			void Register();
		}


		//
		//  Message system to logging and output system messages
		//
		namespace Message
		{
			//
			//  Message Output level to control what messages get sent out
			//
			extern int CurrentOutputLevel;

			//
			//  Handles output to the console and changes output format and color slightly based on severity
			//  Severety levels:   -1 = ERROR, 0 = INFORMATION, 1 = WARNING, 2 = DEBUG
			//
			void Output(int messageLevel, const char* source, const char* message, ...);

			//
			//  Level of messages for the message system
			//
			namespace Levels
			{
				const int Error = -1;
				const int Information = 0;
				const int Warning = 1;
				const int Debug = 2;
			}

			//
			//  Official sources of the system messages can come from
			//
			namespace Sources
			{
				const char System[] = "SGE::System";
				const char Inputs[] = "SGE::Inputs";
				const char FileFormats[] = "SGE::FileFormats";
				const char Render[] = "SGE::Render";
				const char Sound[] = "SGE::Sound";
				const char Display[] = "SGE::Display";
				const char Utility[] = "SGE::Utility";
				const char SGE[] = "SGE";
			}
		}

		//
		//  ANSI Escape Color Codes for provide color to the console output
		//
		namespace ANSIEscapeColorCodes
		{
			const char Reset[] = "\x1b[39m";
			const char Black[] = "\x1b[30m";
			const char Red[] = "\x1b[31m";
			const char Green[] = "\x1b[32m";
			const char Yellow[] = "\x1b[33m";
			const char Blue[] = "\x1b[34m";
			const char Magenta[] = "\x1b[35m";
			const char Cyan[] = "\x1b[36m";
			const char White[] = "\x1b[37m";
			const char BrightBlack[] = "\x1b[90m";
			const char BrightRed[] = "\x1b[91m";
			const char BrightGreen[] = "\x1b[92m";
			const char BrightYellow[] = "\x1b[93m";
			const char BrightBlue[] = "\x1b[94m";
			const char BrightMagenta[] = "\x1b[95m";
			const char BrightCyan[] = "\x1b[96m";
			const char BrightWhite[] = "\x1b[97m";
		}
	}
}
