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
		GLFWwindow* mainWindow = nullptr;

		//Callback to handle any error reporting from GLFW
		void GLFWErrorCallback(int error, const char* description)
		{
			//Dump the error info straight to the stderr
			fprintf(stderr, "GLFW Error: %s\n", description);
		}

		//Startup Function that performs all initializations to the system overall
		//Needs to be called first.   Really...  Call it first...
		void Startup(int windowX, int windowY, const char* gameTitle)
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
			SGE::Sound::Start();

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

		//Shutdown function that performs all the shutdown and termination bits for the system.
		void Shutdown()
		{
			//Clean UP!
			//Make sure the mainDisplay has stopped drawing.
			SGE::Display::StopDrawing();

			//Check to see if there is an active window to close
			if (mainWindow == nullptr)
			{
				//There's no window to close!
				fprintf(stderr, "There is no active game window to close.\n");
			}

			//Close that window
			glfwDestroyWindow(mainWindow);

			//Make sure the mainSoundSystem has stopped running.
			SGE::Sound::Stop();

			//Terminate GLFW, if the game engine is done, so are we.
			glfwTerminate();
		}

		//This function looks for the main display window and sound system does a splash screen.
		void SplashScreen()
		{

		}
	}
}