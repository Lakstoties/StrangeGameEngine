#include "include\SGE\sge.h"
#include <GLFW\glfw3.h>
#include <ctime>
#include <chrono>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  GLFW Window pointer for the Strange Game Engine
	//
	GLFWwindow* OSWindow = nullptr;

	//
	//  Namespace that contains all the major callbacks to handle input from the operating system
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

		//
		//  Window Resize Context callback for GLFW
		//
		void WindowResize(GLFWwindow* window, int width, int height)
		{
			//
			//  Flag that the framebuffer window size has changed for the rest of the system
			//
			SGE::Display::FrameBufferChanged = true;
		}

		//
		//  Mouse Scrool Wheel callback for GLFW
		//
		void ScrollWheel(GLFWwindow* window, double xOffset, double yOffset)
		{
			SGE::Controls::Mouse::ScrollX += xOffset;
			SGE::Controls::Mouse::ScrollY += yOffset;
		}

		//
		//  Mouse Button callback for GLFW
		//
		void MouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			switch (action)
			{
			case GLFW_PRESS:
				SGE::Controls::Mouse::Buttons[button] = true;
				break;

			case GLFW_RELEASE:
				SGE::Controls::Mouse::Buttons[button] = false;
				break;
			}
		}

		//
		//  Keyboard callback for GLFW
		//
		void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			//
			//  Check to make sure we aren't dealing with the GLFW_UNKNOWN key.
			//  If so just ignore it.
			//
			if (key >= 0)
			{
				//  If pressed down
				if (action == GLFW_PRESS)
				{
					//
					//  Update the status array
					//
					SGE::Controls::Keyboard::Status[key] = true;
				}

				// If released
				else if (action == GLFW_RELEASE)
				{
					//
					//  Update the status array
					//
					SGE::Controls::Keyboard::Status[key] = false;
				}
			}
		}

		//
		//  Mouse Cursor callback for GLFW
		//
		void Cursor(GLFWwindow* window, double xPosition, double yPosition)
		{
			int currentFrameBufferX = 0;
			int currentFrameBufferY = 0;

			int currentFrameBufferXOffset = 0;
			int currentFrameBufferYOffset = 0;

			//
			//  Get the current Frame buffer data
			//
			glfwGetFramebufferSize(OSWindow, &currentFrameBufferX, &currentFrameBufferY);

			//
			//  Calculate offsets
			//
			currentFrameBufferXOffset = (currentFrameBufferX - (currentFrameBufferY * SGE::Display::Video::ResolutionX) / SGE::Display::Video::ResolutionY) / 2;
			currentFrameBufferYOffset = (currentFrameBufferY - (currentFrameBufferX * SGE::Display::Video::ResolutionY) / SGE::Display::Video::ResolutionX) / 2;

			//
			//If the Offset goes negative it needs to be hard capped or it throws off calculations.
			//
			if (currentFrameBufferXOffset < 0) { (currentFrameBufferXOffset = 0); }
			if (currentFrameBufferYOffset < 0) { (currentFrameBufferYOffset = 0); }

			//
			//Raw Numbers
			//
			SGE::Controls::Mouse::PositionRawX = (int)xPosition;
			SGE::Controls::Mouse::PositionRawY = (int)yPosition;

			//
			//Scaled
			//
			SGE::Controls::Mouse::PositionX = ((SGE::Controls::Mouse::PositionRawX - currentFrameBufferXOffset) * SGE::Display::Video::ResolutionX) / (currentFrameBufferX - currentFrameBufferXOffset * 2);
			SGE::Controls::Mouse::PositionY = ((SGE::Controls::Mouse::PositionRawY - currentFrameBufferYOffset) * SGE::Display::Video::ResolutionY) / (currentFrameBufferY - currentFrameBufferYOffset * 2);
		}
	}



	//
	//  Startup Function that performs all initializations to the system overall
	//  Needs to be called first.   Really...  Call it first...
	//
	void Startup(int windowX, int windowY, const char* gameTitle)
	{
		//
		//  Initialize GLFW
		//

		//
		//  If the initialization doesn't succeed
		//
		if (!glfwInit())
		{
			//
			//  Report the error
			//
			SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "GLFW initialization failure!\n");
		}

		//
		//  Set the GLFW Error Callback
		//
		glfwSetErrorCallback(SGE::Callbacks::GLFWError);

		//
		//  Start up the sound system
		//
		SGE::Sound::Start();

		//
		//  Check to make sure another window isn't active, we only want one window going at a time.
		//
		if (OSWindow != nullptr)
		{
			SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "There's already a game window open!\n");
			return;
		}

		//
		//  Otherwise, let's create this window.
		//
		OSWindow = glfwCreateWindow(windowX, windowY, gameTitle, NULL, NULL);

		if (OSWindow == nullptr)
		{
			//
			//  Huh, window creation failed...
			//
			SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "GLFW failed to create the main game window.\n");
		}

		//
		//Register all the major callbacks for SGE system
		//

		//
		//  Window Resize callback
		//
		glfwSetWindowSizeCallback(SGE::OSWindow, SGE::Callbacks::WindowResize);

		//
		//  Mouse scrool wheel callback
		//
		glfwSetScrollCallback(SGE::OSWindow, SGE::Callbacks::ScrollWheel);

		//
		//  Mouse button callback
		//
		glfwSetMouseButtonCallback(SGE::OSWindow, SGE::Callbacks::MouseButton);

		//
		//  Mouse cursor callback
		//
		glfwSetCursorPosCallback(SGE::OSWindow, SGE::Callbacks::Cursor);

		//
		//  Keyboard callback
		//
		glfwSetKeyCallback(SGE::OSWindow, SGE::Callbacks::Keyboard);

		//
		//  Fire up utility and other bits
		//
		SGE::Utility::Math::InitializeMath();
	}

	//
	//  Shutdown function that performs all the shutdown and termination bits for the system.
	//
	void Shutdown()
	{
		//
		//  Clean UP!
		//  Make sure the mainDisplay has stopped drawing.
		//
		SGE::Display::Close();

		//
		//  Check to see if there is an active window to close
		//
		if (OSWindow == nullptr)
		{
			//
			//  There's no window to close!
			//
			SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::SGE, "There is no active game window to close.\n");
		}

		//
		//  Close that window
		//
		glfwDestroyWindow(OSWindow);

		//
		//  Make sure the mainSoundSystem has stopped running.
		//
		SGE::Sound::Stop();

		//
		//  Terminate GLFW, if the game engine is done, so are we.
		//
		glfwTerminate();
	}


	void Launch(int displayX, int displayY, const char* gameTitle, int resolutionX, int resolutionY, void(*gameLogic)(bool &keepAlive), bool &keepAliveFlag)
	{
		//
		//  Start the Strange Game Engine
		//
		Startup(displayX, displayY, "Test Title");

		//
		//  Initialize Virtual Display
		//
		Display::Open(resolutionX, resolutionY);

		//
		//  Start drawing
		//
		Display::StartDrawing();

		//
		//  Launch the actual logic threads, before handling events.
		//
		std::thread gameLogicThread(gameLogic, std::ref(keepAliveFlag));

		//  Handle some events
		//  Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
		//  And we are running the event handler on the main thread, Mac OS sends events to the main thread.
		Controls::HandleEvents();

		//
		//  Interface handling over, window is closed stop the drawing loop if it already hasn't
		//
		Display::StopDrawing();

		//
		//  Signal Input Check to Stop
		//
		keepAliveFlag = false;

		//
		//  Wait for all the threads to join back
		//
		gameLogicThread.join();

		//
		//  Shutdown the Strange Game Engine
		//
		Shutdown();
	}
}