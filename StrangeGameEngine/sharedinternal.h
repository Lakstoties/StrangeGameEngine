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

	namespace System
	{
		//GLFW Window pointer for the Strange Game Engine
		//Typically there is only one Window
		//Currently, there is no limitation on multiple windows, but that is outside the focus of the SGE
		//And SGE functions will check to make sure to not open another windows at the moment.
		extern GLFWwindow* mainWindow;
	}
}


