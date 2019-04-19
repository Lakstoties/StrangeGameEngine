#pragma once
#include "api.h"
#include <vector>
//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  The Virtual Display class to simulate the display component of the Strange Game Engine
	//
	namespace Display
	{
		//
		//  Callbacks namespace for callbacks related to Display
		//
		namespace Callbacks
		{
			//
			//  Function to register callbacks
			//
			void SGEAPI Register();
		}

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
			//extern SGEAPI pixel* RAM;
			extern SGEAPI std::vector<pixel> RAM;

			//
			//  The virtual video horizontal resolution
			//
			extern SGEAPI int X;

			//
			//  The virtual video vertical resolution
			//
			extern SGEAPI int Y;
		}

		//
		//  Flag to indicate the framebuffer window size has changed
		//
		extern SGEAPI bool FrameBufferChanged;

		//
		//  Flag to indicate the game resolution has changed
		//
		extern SGEAPI bool GameResolutionChanged;

		//
		//  Count of frames rendered
		//
		extern SGEAPI unsigned int FrameCount;

		//
		//  Video System Initialization
		//
		void SGEAPI Init(int windowX, int windowY, const char* gameTitle);

		//
		//  Block display refresh.  Useful for preventing screen tearing from draw operations happening in the middle of a display refresh
		//
		void SGEAPI BlockRefresh();

		//
		//  Allow the virtual display fresh to allow drawing to continue.
		//
		void SGEAPI AllowRefresh();

		//
		//  Start the Display
		//
		void SGEAPI Open(int newVideoX, int newVideoY);

		//
		//  Stop the Display
		//
		void SGEAPI Close();

		//
		//  Starts the thread to grab data from the virtual video RAM and dumping it to the virtual display
		//
		void SGEAPI StartDrawing();

		//
		//  Stops that thread, and waits for it to join back up.
		//
		void SGEAPI StopDrawing();

		//
		//  Set framebuffer window size
		//
		void SGEAPI SetWindowSize(int width, int height);

		//
		//  Change game resolution
		//
		void SGEAPI ChangeGameResolution(int width, int height);

		//
		//  Hide the mouse cursor from the OS
		//
		void SGEAPI HideOSMouseCursor();

		//
		//  Show the mouse cursor from the OS
		//
		void SGEAPI ShowOSMouseCursor();
	};
}