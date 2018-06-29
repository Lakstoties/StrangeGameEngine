#include "include\SGE\system.h"
#include "GLFW\glfw3.h"
#include <chrono>
#include <ctime>
#include <mutex>
#include <condition_variable>

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
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "GLFW Error: %s\n", description);
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
				const char* source = NULL;

				//  Message contents
				char* message = NULL;
			};


			//
			//  Message System Queue
			//
			const int MESSAGE_QUEUE_SIZE = 1000;
			MessageEntry queuedMessages[MESSAGE_QUEUE_SIZE];
			int queueHead = 0;
			int queueEnd = 0;
			int currentIndex = 0;

			bool MessageThreadKeepAlive = false;

			std::condition_variable messageProcessingHold;

			const int MESSAGE_MAX_SIZE = 10000;

			//
			//  Delete Message Function
			//  
			//
			void DeleteMessage(int index)
			{
				//
				//  Sanity Check Index
				//
				if (index < 0 || index >= MESSAGE_MAX_SIZE)
				{
					return;
				}

				//  Clean out this entry
				queuedMessages[index].level = 0;
				queuedMessages[index].source = NULL;
				queuedMessages[index].time = 0;

				// Delete the message
				delete queuedMessages[index].message;

				//NULL out the pointer
				queuedMessages[index].message = NULL;
			}

			//
			//  Add Message Function
			//  Adds a message to the system message queue
			//
			void SGEAPI Output(int messageLevel, const char* source, const char* message, ...)
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
				if (source == NULL)
				{
					//  NULL message source?
					return;
				}

				//  An actual message exists
				if (message == NULL)
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
				//  Process some initial bits of the message
				//

				//
				//  Get the va_list for the variable argument list, which should the argumented used with the message format text.
				//

				//Delcare a va_list
				va_list messageArguments;

				//Set the start point for the variable argument list to be a pointer right after the message pointer.
				va_start(messageArguments, message);

				//
				//  Form the message into a static char array
				//

				char temp[MESSAGE_MAX_SIZE + 1];
				int tempStringSize = 0;

				tempStringSize = vsprintf(temp, message, messageArguments);
				

				//
				//  Add to queue
				//  WARNING:  This will overwrite any message at the index indicated by the queueEnd.
				//

				//
				//  Lock up the messageQueue mtuex
				//
				messageQueueMutex.lock();

				//
				//  If the queueHead and the queueEnd are the same, check to see if we are overwriting an old message
				//
				if (queueHead == queueEnd)
				{
					//  Are we overwritting an existing message?
					if (queuedMessages[queueHead].message != NULL)
					{
						// Scoot the head over
						queueHead = (queueHead++) % MESSAGE_QUEUE_SIZE;

						DeleteMessage(queueEnd);
					}
				}

				//
				//  Load up the message
				//

				queuedMessages[queueEnd].level = messageLevel;
				queuedMessages[queueEnd].source = source;
				queuedMessages[queueEnd].time = std::time(NULL);

				//Create new message memory
				queuedMessages[queueEnd].message = new char[tempStringSize + 1];

				//Copy the temp message into the target
				strcpy(queuedMessages[queueEnd].message, temp);

				//
				//  Move the queueEnd up
				//
				queueEnd = (queueEnd++) % MESSAGE_QUEUE_SIZE;

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
			void PrintMessage(int index)
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

				std::strftime(timestamp, 100, "%Y-%m-%e %H:%M:%S", std::localtime(&queuedMessages[index].time));


				//
				//  Depending on the message level output it in the right format
				//

				//
				//  MUTEX LOCK Start
				//
				consoleOutputMutex.lock();

				switch (queuedMessages[index].level)
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

					fprintf(stderr, "%s ---ERROR---(%s) ", timestamp, queuedMessages[index].source);
					fprintf(stderr, queuedMessages[index].message);

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

					printf("%s INFORMATION(%s) ", timestamp, queuedMessages[index].source);
					printf(queuedMessages[index].message);

					break;

					//If it's a Warning message
				case Message::Levels::Warning:
					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::BrightYellow);
					#else
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
					#endif

					printf("%s --WARNING--(%s) ", timestamp, queuedMessages[index].source);
					printf(queuedMessages[index].message);

					break;

					//If it's a Debug message
				case Message::Levels::Debug:
					#if (_WIN32_WINNT >= 0x0A00 || !_WIN32)
					printf(ANSIEscapeColorCodes::Green);
					#else	
					SetConsoleTextAttribute(windowsSTDOUTHandle, FOREGROUND_GREEN);
					#endif

					printf("%s ---DEBUG---(%s) ", timestamp, queuedMessages[index].source);
					printf(queuedMessages[index].message);

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
					if (queuedMessages[currentIndex].message != NULL)
					{
						//
						//  Print that message out
						//
						PrintMessage(currentIndex);


						//
						//  Lock out Add from putting in
						//
						messageQueueMutex.lock();


						//
						//  Delete that message
						//
						DeleteMessage(currentIndex);

						//
						//  Unlock and allow add back in
						//
						messageQueueMutex.unlock();




						//
						//  Move to the next index
						//
						currentIndex = (currentIndex++) % MESSAGE_QUEUE_SIZE;
					}
					else
					{
						//
						//  Go to sleep
						//
						//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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