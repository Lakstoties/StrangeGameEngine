#include "include\SGE\system.h"
#include <chrono>
#include <ctime>
#include <mutex>

//
//  Because of certain weirdness with Windows and the terminal emulator, we have to check and do some weirdness to get colored text
//
#ifdef _WIN32
//
//  Include the Windows API
//
#include <Windows.h>
#endif

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace System
	{
		namespace Message
		{
			//
			//  System Message Level Flag
			//  This determines the levels of messages that will be currently outputted to the console
			//
			int CurrentOutputLevel = 4;

			//
			//  Output Mutex
			//  This mutex keeps the messages from running into each other in the output buffer
			//

			std::mutex consoleOutputMutex;


			//
			//  Message Output Function to output message to the system/OS level console
			//
			void Output(int messageLevel, const char* source, const char* message, ...)
			{
				//
				//  Get the va_list for the variable argument list, which should the argumented used with the message format text.
				//

				//Delcare a va_list
				va_list messageArguments;

				//Set the start point for the variable argument list to be a pointer right after the message pointer.
				va_start(messageArguments, message);

				//
				//  If running on Windows...
				//
				#ifdef _WIN32

				//
				//  These are the Windows OS Handles for the stdout and stderr pipes.
				//
				static HANDLE windowsSTDOUTHandle = NULL;
				static HANDLE windowsSTDERRHandle = NULL;

				//
				//  If we haven't already, grab the Windows OS handles for the stdout and stderr pipes.
				//
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
					//  Attempt to turn on ANSI capabilities for the console window
					//
					SetConsoleMode(windowsSTDOUTHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
					SetConsoleMode(windowsSTDERRHandle, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
					#endif
				}
				#endif

				char timestamp[100];

				//
				//  Check to see if we even bother outputting this message
				//
				if (messageLevel > CurrentOutputLevel)
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

				//
				//  MUTEX LOCK Start
				//
				consoleOutputMutex.lock();

				switch (messageLevel)
				{
					//If it's an Error message
				case Message::Levels::Error:

					//
					//  Output straight to stderr and bypass stdout's buffering to get the message out.
					//

					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					//
					//  If Windows 10 or a non-Windows OS (that will probably have a proper terminal emulator), then send the ANSI escape codes for a color change
					//

					fprintf(stderr, ANSIEscapeColorCodes::Red);

					#else
					//
					//  If Windows, but not Windows 10, use the Windows API to change the console color
					//
					SetConsoleTextAttribute(windowsSTDERRHandle, FOREGROUND_RED);
					#endif

					fprintf(stderr, "%s - ERROR: - %s -", timestamp, source);
					vfprintf(stderr, message, messageArguments);

					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					//
					//  Send the ANSI Reset code
					//
					fprintf(stderr, ANSIEscapeColorCodes::Reset);

					#else	
					//
					//  Set the Console Text back to how it was 
					//
					SetConsoleTextAttribute(windowsSTDERRHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
					#endif

					break;

					//If it's an Information message
				case Message::Levels::Information:
					//
					//  Output the message to the stdout, since it's not and emergency
					//

					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					//
					//  If Windows 10 or a non-Windows OS (that will probably have a proper terminal emulator), then send the ANSI escape codes for a color change
					//

					printf(ANSIEscapeColorCodes::BrightWhite);

					#else
					//
					//  If Windows, but not Windows 10, use the Windows API to change the console color
					//
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
				case Message::Levels::Warning:
					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::BrightYellow);
					#else
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_GREEN);
					#endif

					printf("%s - WARNING: - %s -", timestamp, source);
					vprintf(message, messageArguments);

					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::Reset);
					#else		
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
					#endif	

					break;

					//If it's a Debug message
				case Message::Levels::Debug:
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

				//
				//  MUTEX UNLOCK
				//
				consoleOutputMutex.unlock();
			}
		}
	}
}