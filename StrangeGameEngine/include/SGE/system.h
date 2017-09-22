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

		//Perform basic initialization of the system overall
		void Startup(int windowX, int windowY, const char* gameTitle);

		//Performs shutdown and termination operations
		void Shutdown();

		//Strange Game Engine Logo Splash Screen
		void SplashScreen();
	}
}
