#include "include\SGE\system.h"
#include "sharedinternal.h"
#include <GLFW\glfw3.h>

namespace SGE
{
	namespace System
	{
		//GLFW Window pointer for the Strange Game Engine
		//Typically there is only one Window
		//Currently, there is no limitation on multiple windows, but that is outside the focus of the SGE
		//And SGE functions will check to make sure to not open another windows at the moment.
		extern GLFWwindow* mainWindow = nullptr;

		//Flag to keep track if the Strange Game Engine is started.
		bool systemStarted = false;
		
		//Callback to handle any error reporting from GLFW
		void GLFWErrorCallback(int error, const char* description)
		{
			//Dump the error info straight to the stderr
			fprintf(stderr, "GLFW Error: %s\n", description);
		}

		//Startup Function that performs all initializations to the system overall
		//Needs to be called first.   Really...  Call it first...
		void Startup()
		{
			//Initialize GLFW
			//If the initialization doesn't succeed
			if (!glfwInit())
			{
				//Report the error
				fprintf(stderr, "GLFW initialization failure!\n");
			}

			//Set the GLFW Error Callback
			glfwSetErrorCallback(GLFWErrorCallback);

			//Start up the sound system
			SGE::Sound::SoundSystem::Start();

			//Flag that the system has been started up properly... in theory...
			systemStarted = true;
		}

		//Shutdown function that performs all the shutdown and termination bits for the system.
		void Shutdown()
		{
			//Clean UP!
			//Make sure the mainDisplay has stopped drawing.
			SGE::VirtualDisplay::StopDrawing();

			//Make sure the mainSoundSystem has stopped running.
			SGE::Sound::SoundSystem::Stop();

			//Terminate GLFW, if the game engine is done, so are we.
			glfwTerminate();

			//Strange Game Engine should be shutdown now
			systemStarted = false;
		}






		//Function opens up the game window to the desired dimensions
		void OpenGameWindow(int windowX, int windowY, const char* gameTitle)
		{
			//Check to make sure another window isn't active, we only want one window going at a time.
			if (mainWindow != nullptr)
			{
				fprintf(stderr, "There's already a game window open!\n");
				return;
			}

			//Otherwise, let's create this window.
			mainWindow = glfwCreateWindow(windowX, windowY, gameTitle, NULL, NULL);

			if (mainWindow == nullptr)
			{
				//Huh, window creation failed...
				fprintf(stderr, "GLFW failed to create the main game window.\n");
			}
		}

		//Function closes the game window
		void CloseGameWindow()
		{
			//Check to see if there is an active window to close
			if (mainWindow == nullptr)
			{
				//There's no window to close!
				fprintf(stderr, "There is no active game window to close.\n");
			}

			//Close that window
			glfwDestroyWindow(mainWindow);
		}

		//Function creates a virtual display and binds it to the mainWindow
		void InitializeVirtualDisplay(int displayX, int displayY)
		{
			//Fire off display
			SGE::VirtualDisplay::Open(displayX, displayY);
		}


		//Tells the virtual display to start drawing
		void StartDrawing()
		{
			//Start drawing
			SGE::VirtualDisplay::StartDrawing();
		}


		//Tells the virtual display to stop drawing
		void StopDrawing()
		{
			SGE::VirtualDisplay::StopDrawing();
		}

		//This function should be called by the main thread (usually the one that hits the "int main" entry point
		//This function invokes the global event handler to handle events until told otherwise
		void HandleEvents()
		{
			return SGE::Controls::HandleEvents(mainWindow);
		}


		//This function looks for the main display window and sound system does a splash screen.
		void SplashScreen()
		{

		}
	}
}