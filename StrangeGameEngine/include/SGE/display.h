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
		namespace Video
		{
			//The virtual video RAM.  Public accessible to allow other components to write to it directly.
			extern unsigned int* RAM;

			//The virtual video Back Buffer.  Another chunk of video ram to use as needed that's the same size as the VideoRAM.
			extern unsigned int* BackBuffer;

			//A pixel buffers set to the width of the screen to allow a memory block to use for mass copying pixels to the screen.
			extern unsigned int* RowBuffer;

			//The virtual Video RAM size.
			extern unsigned int RAMSize;

			//The virtual video horizontal resolution
			extern int ResolutionX;

			//The virtual video vertical resolution
			extern int ResolutionY;
		}


		//The framebuffer window horizontal resolution within OS window
		extern int FrameBufferX;

		//The framebuffer window vertical resolution within OS Window
		extern int FrameBufferY;

		//Flag to indicate the framebuffer window size has changed
		extern bool FrameBufferChanged;

		//The viewport window (within the framebuffer window) vertical resolution
		extern int ViewPortWindowX;

		//The viewport window (within the framebuffer window) horizontal resolution
		extern int ViewPortWindowY;

		//The viewport window (within the framebuffer window) X offset
		extern int ViewPortWindowOffsetX;

		//The viewport window (within the framebuffer window) Y offset
		extern int ViewPortWindowOffsetY;

		//Flag to indicate the game resolution has changed
		extern bool GameResolutionChanged;

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

		//Set framebuffer window size
		void SetWindowSize(int width, int height);

		//Change game resolution
		void ChangeGameResolution(int width, int height);


	};
}