#pragma once
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
