#pragma once
#include <GLFW\glfw3.h>
#include "display.h"
#include "render.h"
#include "controls.h"
#include "sound.h"
#include "gui.h"

namespace SGE
{
	namespace System
	{
		//Default frame rate for the frame rate limiter
		const int DEFAULT_FRAME_RATE_LIMIT = 100;
		const int DEFAULT_FRAME_WAIT_MILLISECONDS = 1000 / DEFAULT_FRAME_RATE_LIMIT;

		//Flag to indicate if the Strange Game Engine has been initialized by the Startup function
		//Declared extern to keep the linker from freaking out over duplicate definitions.
		//Officially declared in system.cpp
		extern bool systemStarted;

		//GLFW Window pointer for the Strange Game Engine
		//Typically there is only one Window
		//Currently, there is no limitation on multiple windows, but that is outside the focus of the SGE
		//And SGE functions will check to make sure to not open another windows at the moment.
		//Extern to keep the linker from being confused, defined in system.cpp
		extern GLFWwindow* mainWindow;

		//Main virtual display for the Strange Game Engine
		//Typically there is only one display
		//Currently, there is no limitation on multiple displays, but each virtual display should only be bound to only one GLFWWindow.
		//extern to keep the linker from being confused, defined in system.cpp
		extern SGE::VirtualDisplay* mainDisplay;

		//Main sound system for the Strange Game Engine
		//Typically there is only one sound system
		//Currently, there is no limitation on multiple sound systems, but some platforms and APIs don't support multiple PortAudio streams
		//that would be created by multiple sound systems.
		//Extern to keep the linker from being confused, defined in system.cpp
		extern SGE::Sound::SoundSystem* mainSoundSystem;

		//Static method for the GLFW error callback
		void GLFWErrorCallback(int error, const char* description);

		//Opens a game window of X and Y size with the specified title
		//Only one may be open at any given time.
		void OpenGameWindow(int windowX, int windowY, const char* gameTitle);

		//Closes the currently active game window
		void CloseGameWindow();

		//Initialized the virtual display and binds it to the mainWindow
		void InitializeVirtualDisplay(int virtualDisplayX, int virtualDisplayY);
		
		//Perform basic initialization of the system overall
		void Startup();

		//Performs shutdown and termination operations
		void Shutdown();
				
		//Tells the mainDisplay to start drawing
		void StartDrawing();

		//Tells the mainDisplay to stop drawing
		void StopDrawing();

		//Called from the main thread to perform event handling
		void HandleEvents();

		//Strange Game Engine Logo Splash Screen
		void SplashScreen();
	}
}
