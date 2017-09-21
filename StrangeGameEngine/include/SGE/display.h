#pragma once
#include <thread>
#include <mutex>

//Use the SGE namespace to keep things seperated
namespace SGE
{
	//Default frame rate for the frame rate limiter
	const int DEFAULT_FRAME_RATE_LIMIT = 100;
	const int DEFAULT_FRAME_WAIT_MILLISECONDS = 1000 / DEFAULT_FRAME_RATE_LIMIT;
	
	//The Virtual Display class to simulate the display component of the Strange Game Engine
	namespace VirtualDisplay
	{
		//Scaling factor used when the display window differs from the simulated game resolution
		extern float videoScaling;

		//Minium wait time before the next frame is allowed
		extern int frameWaitMilliseconds;

		//The virtual video RAM.  Public accessible to allow other components to write to it directly.
		extern unsigned int* virtualVideoRAM;

		//The virtual video horizontal resolution
		extern int virtualVideoX;

		//The virtual video vertical resolution
		extern int virtualVideoY;

		//A mutex that can be used to temporarily pause the drawing thread at a key point to allow the video ram to be updated fully.
		//Prevents flicking and tearing by the display thread from updating mid way through writes to VRAM
		extern std::mutex refreshHold;

		//Start the Display
		void Open(int newVideoX, int newVideoY);

		//Stop the Display
		void Close();

		//Starts the thread to grab data from the virtual video RAM and dumping it to the virtual display
		void StartDrawing();

		//Stops that thread, and waits for it to join back up.
		void StopDrawing();
	};
}