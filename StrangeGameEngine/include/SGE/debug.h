#pragma once
#include "api.h"
#include "utility.h"

namespace SGE
{
	namespace Debug
	{
		//
		//  Debug function that draws the classic 4 bit palette in a box for testing purposes.
		//
		void SGEAPI DrawColorPalette4bit(int xCornerPosition, int yCornerPosition);

		//
		// Debug funciton that draws classic 8 bit palette in a box for testing purposes.
		//
		void SGEAPI DrawColorPalette8bit(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that draws a keyboard to indicate the current status of all keyboard keys
		//
		void SGEAPI DrawKeyboardStatus(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that draws a box with the status elements for the mouse
		//
		void SGEAPI DrawMouseStatusBox(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that shows the current status of the module player
		//
		void SGEAPI DrawPlayerStatusBox(SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerToUse, int xCornerPosition, int yCornerPosition);

		//
		//  Debug function that shows the currenrt status of all audio channels
		//
		void SGEAPI DrawAudioChannelStatusBox(int xCornerPosition, int yCornerPosition);

		//
		//  Debug function to show audio levels
		//
		void SGEAPI DrawLevelMeter(char label[4], int xCornerPosition, int yCornerPosition, unsigned int sampleLevel, float channelVolume);

		//
		//  Debug function to show the status of Sample Buffers
		//
		void SGEAPI DrawSampleBuffersStatus(int xCornerPosition, int yCornerPosition);
	}
}