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
		//  Register any System callbacks
		//
		SGE::System::Callbacks::Register();


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
		//  Register all the major callbacks for SGE system
		//

		//  Register Display Callbacks
		SGE::Display::Callbacks::Register();

		//  Register Inputs Callbacks
		SGE::Inputs::Callbacks::Register();




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
		//  Start the message system
		//
		SGE::System::Message::StartMessageSystem();

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
		Inputs::HandleEvents();

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

		//
		//  Stop the Message System
		//
		SGE::System::Message::StopMessageSystem();
	}
}