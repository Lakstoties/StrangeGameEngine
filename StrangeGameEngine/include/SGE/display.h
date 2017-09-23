#pragma once

//Use the SGE namespace to keep things seperated
namespace SGE
{
	//Default frame rate for the frame rate limiter
	const int DEFAULT_FRAME_RATE_LIMIT = 100;
	const int DEFAULT_FRAME_WAIT_MILLISECONDS = 1000 / DEFAULT_FRAME_RATE_LIMIT;
	
	//The Virtual Display class to simulate the display component of the Strange Game Engine
	namespace Display
	{
		//Minium wait time before the next frame is allowed
		extern int frameWaitMilliseconds;

		//The virtual video RAM.  Public accessible to allow other components to write to it directly.
		extern unsigned int* VideoRAM;

		//The virtual video horizontal resolution
		extern int ResolutionX;

		//The virtual video vertical resolution
		extern int ResolutionY;

		//Block display refresh.  Useful for preventing screen tearing from draw operations happening in the middle of a display refresh
		void BlockRefresh();

		//Allow the virtual display fresh to allow drawing to continue.
		void AllowRefresh();

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