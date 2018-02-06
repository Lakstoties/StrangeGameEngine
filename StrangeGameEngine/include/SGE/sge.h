#pragma once
#include "display.h"
#include "render.h"
#include "controls.h"
#include "sound.h"
#include "gui.h"
#include "fileformats.h"
#include "utility.h"
#include "system.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Function is used to launch all parts of the game
	//
	void Launch(int displayX, int displayY, const char* gameTitle, int resolutionX, int resolutionY, void(*gameLogic)(bool &keepAlive), bool &keepAliveFlag);


	//
	//  Perform basic initialization of the system overall
	//
	void Startup(int windowX, int windowY, const char* gameTitle);

	//
	//  Performs shutdown and termination operations
	//
	void Shutdown();
}
