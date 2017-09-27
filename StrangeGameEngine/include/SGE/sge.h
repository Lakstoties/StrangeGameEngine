#pragma once
#include "display.h"
#include "render.h"
#include "controls.h"
#include "sound.h"
#include "gui.h"

namespace SGE
{
	//Perform basic initialization of the system overall
	void Startup(int windowX, int windowY, const char* gameTitle);

	//Performs shutdown and termination operations
	void Shutdown();
}
