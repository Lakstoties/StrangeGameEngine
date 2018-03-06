#pragma once
#include "utility.h"

namespace SGE
{
	namespace Debug
	{
		//
		//  Debug function that draws the classic 4 bit palette in a box for testing purposes.
		//
		void DrawColorPalette4bit(int xCornerPosition, int yCornerPosition);

		//
		// Debug funciton that draws classic 8 bit palette in a box for testing purposes.
		//
		void DrawColorPalette8bit(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that draws a keyboard to indicate the current status of all keyboard keys
		//
		void DrawKeyboardStatus(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that draws a box with the status elements for the mouse
		//
		void DrawMouseStatusBox(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that shows the current status of the module player
		//
		void DrawPlayerStatusBox(SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerToUse, int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that shows the currenrt status of all audio channels
		//
		void DrawAudioChannelStatusBox(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function to show audio levels
		//
		void DrawLevelMeter(char label[4], int xCornerPosition, int yCornerPosition, unsigned int sampleLevel, float channelVolume);
	}
}