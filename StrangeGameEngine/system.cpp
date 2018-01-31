#include "include\SGE\system.h"
#include <chrono>
#include <ctime>
#include "sharedinternal.h"

//
//  Because of certain weirdness with Windows and the terminal emulator, we have to check and do some weirdness to get colored text
//
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32

//
//  Clang compile has some weirdness with IUnknown, so make sure it is declared before the Windows header
//
#define CINTERFACE 

//
//  Include the Windows API
//
#include <Windows.h>
#endif


namespace SGE
{
	namespace System
	{
		//
		//  System Message Level Flag
		//
		int MessageLevel = 4;


		//
		//  Message Output Function to output message to the system/OS level console
		//
		void Message(int messageLevel, const char* source, const char* message, ...)
		{
			//
			//  Get the va_list
			//
			va_list messageArguments;
			va_start(messageArguments, message);




			//
			//  If running on Windows, check to see if the console window's handle is known, if not, find it and poke it to see what needs to be done to get colored text working.
			//
#ifdef _WIN32

			//
			//Check to see if we are running on Windows 10
			//  Later versions of Windows 10 support Virtual Terminal processing and can read ANSI escape codes
			//
			static HANDLE windowsSTDOUTHandle = NULL;
			static HANDLE windowsSTDERRHandle = NULL;
			static bool windowsCanHanldeANSI = false;

			if (windowsSTDOUTHandle == NULL || windowsSTDERRHandle == NULL)
			{
				//
				//  Get the stdout and stderr handles
				//
				windowsSTDOUTHandle = GetStdHandle(STD_OUTPUT_HANDLE);
				windowsSTDERRHandle = GetStdHandle(STD_ERROR_HANDLE);

		//
		//  Check for Windows 10 or higher, since it is the only current Window OS that have a terminal emulator that can possibly understand ANSI
		//
		#if (_WIN32_WINNT >= 0xA00)
				//
				//  Check the console window for ANSI capacities
				//
				windowsCanHanldeANSI = SetConsoleMode(windowsSTDOUTHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
				windowsCanHanldeANSI = SetConsoleMode(windowsSTDERRHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

				if (windowsCanHanldeANSI)
				{
					printf(ANSIEscapeColorCodes::Green);
					printf("Windows OS Console: CAN handle color text!\n");
					printf(ANSIEscapeColorCodes::Reset);
				}
				else
				{
					printf("Windows OS Console: CAN NOT handle color text!\n");
				}
		#endif
			}
#endif

			char timestamp[100];

			//
			//  Check to see if we even bother outputting this message
			//
			if (messageLevel > MessageLevel)
			{
				//Nope...  Return
				return;
			}

			//
			//  Generate the timestamp to use
			//

			//Grab the current time and store it as a time_t after converting it
			std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

			std::strftime(timestamp, 100, "%Y-%m-%e %H:%M:%S", std::localtime(&currentTime));


			//
			//  Depending on the message level output it in the right format
			//

			switch (messageLevel)
			{
				//If it's an Error message
			case MessageLevels::Error:
				//
				//Output straight to stderr and bypass stdout's buffering to get the message out.
				//

			//
			//Check for ANSI support
			//

			//If Windows 10 or a non-Windows OS (that will probably have a proper terminal emulator), then send the ANSI escape codes for a color change
			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				fprintf(stderr, ANSIEscapeColorCodes::Red);

			//If Windows, but not Windows 10, use the Windows API to change the console color
			#else
				SetConsoleTextAttribute(windowsSTDERRHandle, FOREGROUND_RED);
				
			#endif

				fprintf(stderr, "%s - ERROR: - %s -", timestamp, source);
				vfprintf(stderr, message, messageArguments);

			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				fprintf(stderr, ANSIEscapeColorCodes::Reset);
			#else	
				SetConsoleTextAttribute(windowsSTDERRHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			#endif

				break;

				//If it's an Information message
			case MessageLevels::Information:
			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::White);

			#else
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			#endif

				printf("%s - INFORMATION: - %s -", timestamp, source);
				vprintf(message, messageArguments);

			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::Reset);
			#else
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

			#endif		

				break;

				//If it's a Warning message
			case MessageLevels::Warning:
			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::Yellow);
			#else
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_BLUE | FOREGROUND_GREEN);
			#endif

				printf("%s - INFORMATION: - %s -", timestamp, source);
				vprintf(message, messageArguments);

			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::Reset);
			#else		
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			#endif	

				break;

				//If it's a Debug message
			case MessageLevels::Debug:
			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::Green);
			#else	
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_GREEN);
			#endif

				printf("%s - DEBUG: - %s -", timestamp, source);
				vprintf(message, messageArguments);

			#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
				printf(ANSIEscapeColorCodes::Reset);
			#else
				SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			#endif	

				break;
			}
		}
	}
}