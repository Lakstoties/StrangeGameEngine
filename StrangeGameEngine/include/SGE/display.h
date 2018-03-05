#pragma once
//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Default frame rate for the frame rate limiter
	//
	const int DEFAULT_FRAME_RATE_LIMIT = 150;

	//
	//  Calculated delay in milliseconds to roughly meet frame rate limit
	//
	const int DEFAULT_FRAME_WAIT_MILLISECONDS = 1000 / DEFAULT_FRAME_RATE_LIMIT;
	
	//
	//  The Virtual Display class to simulate the display component of the Strange Game Engine
	//
	namespace Display
	{
		//
		//  Video namespace contain elements for virtual display used direct by the game
		//
		namespace Video
		{
			//
			//  Pixel data format
			//
			typedef unsigned int pixel;

			//
			//  The virtual video RAM.  Public accessible to allow other components to write to it directly.
			//
			extern pixel* RAM;

			//
			//  The virtual Video RAM size.
			//
			extern unsigned int RAMSize;

			//
			//  The virtual video horizontal resolution
			//
			extern int ResolutionX;

			//
			//  The virtual video vertical resolution
			//
			extern int ResolutionY;
		}

		//
		//  Flag to indicate the framebuffer window size has changed
		//
		extern bool FrameBufferChanged;

		//
		//  Flag to indicate the game resolution has changed
		//
		extern bool GameResolutionChanged;

		//
		//  Count of frames rendered
		//
		extern unsigned int FrameCount;

		//
		//  Rendering Delay
		//
		extern int RenderDelay;

		//
		//  Frame Delay
		//
		extern int FrameDelay;

		//
		//  Block display refresh.  Useful for preventing screen tearing from draw operations happening in the middle of a display refresh
		//
		void BlockRefresh();

		//
		//  Allow the virtual display fresh to allow drawing to continue.
		//
		void AllowRefresh();

		//
		//  Start the Display
		//
		void Open(int newVideoX, int newVideoY);

		//
		//  Stop the Display
		//
		void Close();

		//
		//  Starts the thread to grab data from the virtual video RAM and dumping it to the virtual display
		//
		void StartDrawing();

		//
		//  Stops that thread, and waits for it to join back up.
		//
		void StopDrawing();

		//
		//  Set framebuffer window size
		//
		void SetWindowSize(int width, int height);

		//
		//  Change game resolution
		//
		void ChangeGameResolution(int width, int height);

		//
		//  Hide the mouse cursor from the OS
		//
		void HideOSMouseCursor();

		//
		//  Show the mouse cursor from the OS
		//
		void ShowOSMouseCursor();
	};
}