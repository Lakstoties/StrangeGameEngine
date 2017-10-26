#pragma once
#include <GLFW\glfw3.h>
namespace SGE
{

	//
	//
	//  Internal Bits for Control
	//
	//

	namespace Controls
	{
		//Method to handle polling events from desired Window
		//Should be called with main thread for compatibility with certain OSes that are strange with GUI I/O
		void HandleEvents(GLFWwindow* targetWindow);
	}

	namespace Display
	{
		//The virtual video horizontal resolution
		extern int ResolutionX;

		//The virtual video vertical resolution
		extern int ResolutionY;
	}

	//GLFW Window pointer for the Strange Game Engine
	extern GLFWwindow* mainWindow;
}


