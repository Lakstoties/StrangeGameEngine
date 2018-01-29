#pragma once
namespace SGE
{
	namespace System
	{
		//
		//  Message Output level to control what messages get sent out
		//
		extern int MessageLevel;

		//
		//  Handles output to the console and changes output format and color slightly based on severity
		//  Severety levels:   -1 = ERROR, 0 = INFORMATION, 1 = WARNING, 2 = DEBUG
		//
		void Message(int messageLevel, const char* source, const char* message, ...);

		namespace MessageLevels
		{
			const int Error = -1;
			const int Information = 0;
			const int Warning = 1;
			const int Debug = 2;
		}

		namespace MessageSourceCategories
		{
			const char System[] = "SGE::System";
			const char Controls[] = "SGE::Controls";
			const char FileFormats[] = "SGE::FileFormats";
			const char Render[] = "SGE::Render";
			const char Sound[] = "SGE::Sound";
			const char Display[] = "SGE::Display";
			const char Utility[] = "SGE::Utility";
			const char SGE[] = "SGE";
		}

		namespace ANSIEscapeColorCodes
		{
			const char Reset[] = "\x1b[27m";
			const char Black[] = "\x1b[30m";
			const char Red[] = "\x1b[31m";
			const char Green[] = "\x1b[32m";
			const char Blue[] = "\x1b[33m";
			const char Yellow[] = "\x1b[34m";
			const char Magenta[] = "\x1b[35m";
			const char Cyan[] = "\x1b[36m";
			const char White[] = "\x1b[37m";
		}
	}
}
