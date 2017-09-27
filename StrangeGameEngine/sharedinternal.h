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

	//GLFW Window pointer for the Strange Game Engine
	extern GLFWwindow* mainWindow;
}


