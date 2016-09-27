#pragma once
#include <GLFW\glfw3.h>
#include <thread>
#include <mutex>

//Use the SGE namespace to keep things seperated
namespace SGE
{
	//Default frame rate for the frame rate limiter
	const int DEFAULT_FRAME_RATE_LIMIT = 100;
	const int DEFAULT_FRAME_WAIT_MILLISECONDS = 1000 / DEFAULT_FRAME_RATE_LIMIT;


	//The Virtual Display class to simulate the display component of the Strange Game Engine
	class VirtualDisplay
	{
	private:

		//OS generated display window for the system.
		GLFWwindow* targetWindow;

		//Scaling factor used when the display window differs from the simulated game resolution
		float videoScaling;

		//C++ thread pointer to keep track of the spawned drawing thread.
		std::thread* drawingThread;

		//Boolean used to control whether the draw thread keeps on running or not.
		bool continueDrawing;

		//Main update thread used to refresh the display independently from the rest of the system
		void UpdateThread();

		//
		int frameWaitMilliseconds = DEFAULT_FRAME_WAIT_MILLISECONDS;

	public:

		//The virtual video RAM.  Public accessible to allow other components to write to it directly.
		unsigned int* virtualVideoRAM;

		//The virtual video horizontal resolution
		int virtualVideoX;

		//The virtual video vertical resolution
		int virtualVideoY;

		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to VRAM
		std::mutex refreshHold;

		//The default constructor!  Woo...
		VirtualDisplay(
			int newDisplayX,					//New display's X or width
			int newDisplayY,					//New display's Y or height
			GLFWwindow* newTargetWindow	//GLFW rendering window to use to output
		);

		//Deconstructor to handle some clean up bits
		~VirtualDisplay();

		//Starts the thread to grab data from the virtual video RAM and dumping it to the virtual display
		void StartDrawing();

		//Stops that thread, and waits for it to join back up.
		void StopDrawing();
	};
}