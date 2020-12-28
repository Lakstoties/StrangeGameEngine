#include "include\SGE\system.h"
#include "GLFW\glfw3.h"
#include <chrono>
#include <ctime>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <thread>

//
//  Because of certain weirdness with Windows and the terminal emulator, we have to check and do some weirdness to get colored text
//
#ifdef _WIN32
//
//  Include the Windows API
//
#define CINTERFACE 
#include <Windows.h>
#endif

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace System
	{
		//
		//  OS Specific Stuff
		//
		namespace OS
		{

		}

		//
		//  Callbacks namespace for handling any callbacks related to System
		//
		namespace Callbacks
		{
			//
			//  Callback to handle any error reporting from GLFW
			//
			void GLFWError(int error, const char* description)
			{
				//
				//  Report the error through the SGE message system
				//
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "GLFW Error: " + std::string(description) + "\n");
			}


			void Register()
			{
				//
				//  Set the GLFW Error Callback
				//
				glfwSetErrorCallback(SGE::System::Callbacks::GLFWError);
			}
		}



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
			//  Message Add Mutex
			//  This mutex keeps the messages from colliding when they get added to the message queue
			//
			std::mutex messageQueueMutex;

			//
			//  Messge System Thread
			//  This thread operated the messaging system and allows message outputs to be non-blocking
			//
			std::thread* messageSystemThread = NULL;

			//
			//  Message System Data Structure
			//  This is the data structure for a message, it contains the relevant information to allow it to be effectively stored until it is able to be dispalys
			//
			struct MessageEntry
			{
				//  Type of Message
				int level = 0;

				//  Time Inputted
				std::time_t time = 0;

				//  Source of the message
				std::string source;

				//  Message contents
				std::string message;
			};


			//
			//  Message System Queue
			//
			std::queue<MessageEntry> messageQueue;

			bool MessageThreadKeepAlive = false;

			std::condition_variable messageProcessingHold;

			//
			//  Add Message Function
			//  Adds a message to the system message queue
			//
			void SGEAPI Output(int messageLevel, const std::string source, const std::string message)
			{
				//
				//  Sanity Check arguments
				//

				//  Message Level Valid?
				if (messageLevel < System::Message::Levels::Error || messageLevel > System::Message::Levels::Debug)
				{
					//  Not a valid message level
					return;
				}

				//  Message Source Valid
				if (source == "")
				{
					//  NULL message source?
					return;
				}

				//  An actual message exists
				if (message == "")
				{
					//  NULL message?
					return;
				}

				//
				//  Check to see if we even bother adding this message
				//
				if (messageLevel > CurrentOutputLevel)
				{
					//Nope...  Return
					return;
				}

				//
				//  Load up the message
				//
				MessageEntry tempMessage;
				
				tempMessage.level = messageLevel;
				tempMessage.source = source;
				tempMessage.time = std::time(NULL);
				tempMessage.message = message;

				//
				//  Lock up the messageQueue mutex
				//
				messageQueueMutex.lock();

				messageQueue.push(tempMessage);
				//
				//  Unlock the Message Queue Mutex
				//
				messageQueueMutex.unlock();

				//
				//  Poke the Message Processing Thread
				//
				if (messageSystemThread != NULL)
				{
					messageProcessingHold.notify_all();
				}
			}

			//
			//  Print Message Function
			//
			void PrintMessage(MessageEntry &currentMessageEntry)
			{
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

				//
				//  Generate the timestamp to use
				//

				//  Temp Time Stamp Buffer
				char timestamp[100];

				std::strftime(timestamp, 100, "%Y-%m-%e %H:%M:%S", std::localtime(&currentMessageEntry.time));


				//
				//  Depending on the message level output it in the right format
				//

				//
				//  MUTEX LOCK Start
				//
				consoleOutputMutex.lock();

				switch (currentMessageEntry.level)
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

					fprintf(stderr, "%s ---ERROR---(%s) ", timestamp, currentMessageEntry.source.c_str());
					fprintf(stderr, currentMessageEntry.message.c_str());

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

					printf("%s INFORMATION(%s) ", timestamp, currentMessageEntry.source.c_str());
					printf(currentMessageEntry.message.c_str());

					break;

					//If it's a Warning message
				case Message::Levels::Warning:
					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::BrightYellow);
					#else
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
					#endif

					printf("%s --WARNING--(%s) ", timestamp, currentMessageEntry.source.c_str());
					printf(currentMessageEntry.message.c_str());

					break;

					//If it's a Debug message
				case Message::Levels::Debug:
					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::Green);
					#else	
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_GREEN);
					#endif

					printf("%s ---DEBUG---(%s) ", timestamp, currentMessageEntry.source.c_str());
					printf(currentMessageEntry.message.c_str());

					break;
				}

				//
				//  Reset the console colors
				//

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

				//
				//  MUTEX UNLOCK
				//
				consoleOutputMutex.unlock();
			}


			void MessageProcessorThread()
			{
				//
				//  Set up a mutex tied to the conditional variable
				//
				std::mutex processingMutex;
				std::unique_lock<std::mutex> processingLock(processingMutex);

				while (MessageThreadKeepAlive)
				{
					//
					//  Check for a new message
					//
					if (messageQueue.size() > 0)
					{
						//  Print that message out
						PrintMessage(messageQueue.front());

						//  Delete that message
						messageQueue.pop();
					}
					else
					{
						//  Go to sleep
						messageProcessingHold.wait(processingLock);
					}
				}

				return;
			}

			void SGEAPI StartMessageSystem()
			{
				MessageThreadKeepAlive = true;

				messageSystemThread = new std::thread(MessageProcessorThread);
			}

			void SGEAPI StopMessageSystem()
			{
				MessageThreadKeepAlive = false;

				if (messageSystemThread->joinable())
				{
					messageProcessingHold.notify_all();
					messageSystemThread->join();
				}

				delete messageSystemThread;
				messageSystemThread = NULL;
			}
		}
	}
}