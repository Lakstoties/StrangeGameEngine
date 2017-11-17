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

		//The framebuffer window horizontal resolution within OS window
		extern int FrameBufferX;

		//The framebuffer window vertical resolution within OS Window
		extern int FrameBufferY;

		//Flag to indicate the framebuffer window size has changed
		extern bool FrameBufferChanged;

		//The viewpoint window (within the framebuffer window) vertical resolution
		extern int ViewPortWindowX;

		//The viewpoint window (within the framebuffer window) horizontal resolution
		extern int ViewPortWindowY;

		//The viewpoint window (within the framebuffer window) X offset
		extern int ViewPortWindowOffsetX;

		//The viewpoint window (within the framebuffer window) Y offset
		extern int ViewPortWindowOffsetY;
	}

	//GLFW Window pointer for the Strange Game Engine
	extern GLFWwindow* mainWindow;
}


