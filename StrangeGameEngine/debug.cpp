#include "include\SGE\debug.h"
#include "include\SGE\render.h"
#include "include\SGE\controls.h"
#include "include\SGE\sound.h"
#include "include\SGE\utility.h"

namespace SGE
{
	namespace Debug
	{
		//
		//  Audio Level Meter
		//
		void DrawLevelMeter(char label[4], int xCornerPosition, int yCornerPosition, unsigned int sampleLevel, float channelVolume)
		{
			//Channel Label
			SGE::Render::DrawString(label, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition, yCornerPosition, SGE::Render::Colors::ColorMode8Bit[157]);

			//Green Lights: 11
			for (int i = 0; i < 11; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8,
					(sampleLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[40] : SGE::Render::Colors::ColorMode8Bit[22]);
			}

			//Yellow Lights: 4
			for (int i = 11; i < 15; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8,
					(sampleLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[226] : SGE::Render::Colors::ColorMode8Bit[58]);
			}

			//Sixthteenth Level Light
			SGE::Render::DrawBox(xCornerPosition + 115, yCornerPosition, 5, 8,
				(sampleLevel > (15 * SGE::Sound::SAMPLE_MAX_AMPLITUDE / 16)) ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);

			//Draw the Volume level indicator box
			SGE::Render::DrawRectangle(xCornerPosition + 24 + 6 * (int)(channelVolume * 15), yCornerPosition - 1, 7, 10, SGE::Render::Colors::ColorMode8Bit[255]);
		}

		void DrawFrequencyMeter(int xCornerPosition, int yCornerPosition, float channelFrequency)
		{
			char tempString[10];

			float calculatedPeriod = SGE::Utility::ModuleTrackerMusic::NTSC_TUNING / (SGE::Sound::SAMPLE_RATE * 2 * channelFrequency);

			//Draw Frequency bar
			SGE::Render::DrawBox(xCornerPosition + 1, yCornerPosition + 1, 300, 8, SGE::Render::Colors::ColorMode8Bit[237]);

			//Draw Border for Frequency Bar
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 302, 10, SGE::Render::Colors::ColorMode8Bit[28]);

			//Draw frequency indicator
			SGE::Render::DrawBox(xCornerPosition + 1 + 300 - ((calculatedPeriod / 1712) * 300), yCornerPosition + 1, 1, 8, SGE::Render::Colors::ColorMode8Bit[255]);

			//Write out current frequency
			sprintf(tempString, "%06.1f", calculatedPeriod);

			//Draw Background
			SGE::Render::DrawBox(xCornerPosition + 305, yCornerPosition, 57, 10, SGE::Render::Colors::ColorMode8Bit[237]);

			//Draw Number from string
			SGE::Render::DrawString(tempString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 305, yCornerPosition + 1, SGE::Render::Colors::ColorMode8Bit[157]);

		}

		void DrawChannelMonitor(int xCornerPosition, int yCornerPosition, unsigned int channelNumber)
		{
			const int CHANNEL_MONITOR_WIDTH = 18;
			const int CHANNEL_MONITOR_HEIGHT = 155;
			const int CHANNEL_BAR_WIDTH = 8;
			const int CHANNEL_BAR_SPACING = (CHANNEL_MONITOR_WIDTH - CHANNEL_BAR_WIDTH) / 2;
			const int CHANNEL_VOLUME_SPACING = 2;
			const int CHANNEL_VOLUME_WIDTH = CHANNEL_MONITOR_WIDTH - CHANNEL_VOLUME_SPACING * 2;

			//Check for a valid channel number
			if (channelNumber >= SGE::Sound::MAX_CHANNELS)
			{
				return;
			}

			char tempString[5];

			sprintf(tempString, "%02i", channelNumber);

			//Draw Test Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, CHANNEL_MONITOR_WIDTH, CHANNEL_MONITOR_HEIGHT, SGE::Render::Colors::ColorMode8Bit[2]);

			//Draw Pan gauge background
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition, CHANNEL_VOLUME_WIDTH, 8, SGE::Render::Colors::ColorMode8Bit[237]);

			//Draw Pan gauge itself
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING + (CHANNEL_VOLUME_WIDTH / 2) + SGE::Sound::Channels[channelNumber].Pan * CHANNEL_VOLUME_WIDTH, yCornerPosition, 1, 8, SGE::Render::Colors::ColorMode8Bit[255]);

			//
			//  Draw Meter Lights
			//

			//Green Lights: 11
			for (int i = 0; i < 11; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5,
					(SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[40] : SGE::Render::Colors::ColorMode8Bit[22]);
			}

			//Yellow Lights: 4
			for (int i = 11; i < 15; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5,
					(SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[226] : SGE::Render::Colors::ColorMode8Bit[58]);
			}

			//Sixthteenth Level Light
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 10, CHANNEL_BAR_WIDTH, 5,
				SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel > (15 * SGE::Sound::SAMPLE_MAX_AMPLITUDE / 16) ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);

			//Draw the Volume level indicator box
			SGE::Render::DrawRectangle(xCornerPosition + CHANNEL_BAR_SPACING - 1, yCornerPosition + 99 - 6 * (int)(SGE::Sound::Channels[channelNumber].Volume * 15), CHANNEL_BAR_WIDTH + 2, 7, SGE::Render::Colors::ColorMode8Bit[255]);

			//Draw Channel Label
			SGE::Render::DrawString(tempString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 108, SGE::Render::Colors::ColorMode8Bit[157]);

			//
			//Draw Mode lights
			//

			//Is Arpeggio On?
			SGE::Render::DrawString((char*)"AR", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 118,
				SGE::Sound::Channels[channelNumber].arpeggioEnabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Vibrato On?
			SGE::Render::DrawString((char*)"VB", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 128,
				SGE::Sound::Channels[channelNumber].vibratoEnabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Volume Slide On?
			SGE::Render::DrawString((char*)"VS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 138,
				SGE::Sound::Channels[channelNumber].volumeSlideEnabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Period Slide On?
			SGE::Render::DrawString((char*)"PS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 148,
				SGE::Sound::Channels[channelNumber].periodSlidEnabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);
		}

		//
		//  Status box that shows the current state for all audio channels
		//
		void DrawAudioChannelStatusBox(int xCornerPosition, int yCornerPosition)
		{
			//Draw Background Box

			//Draw Background Border

			//Draw bottom channel meters
			for (int i = 0; i < SGE::Sound::MAX_CHANNELS; i++)
			{
				DrawChannelMonitor(xCornerPosition + 5 + (18 * i), yCornerPosition, i);
			}
		}

		//
		//  Status box for module tracker file player
		//
		void DrawPlayerStatusBox(SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerToUse, int xCornerPosition, int yCornerPosition)
		{
			//Draw Player Status Boxes
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 510, 75, SGE::Render::Colors::ColorMode8Bit[2]);
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 510, 75, SGE::Render::Colors::ColorMode8Bit[22]);

			//Draw Song Status Row
			//Song Position
			//"Pos"
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'P'], xCornerPosition + 05, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'o'], xCornerPosition + 11, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'s'], xCornerPosition + 17, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Position Box
			SGE::Render::DrawBox(xCornerPosition + 25, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Position Number
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentPosition / 10) + 0x30], xCornerPosition + 26, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentPosition % 10) + 0x30], xCornerPosition + 32, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Pattern In Use
			//"Pat"
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'P'], xCornerPosition + 44, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'a'], xCornerPosition + 50, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'t'], xCornerPosition + 56, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Pattern Box
			SGE::Render::DrawBox(xCornerPosition + 64, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Pattern Number
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentPattern / 10) + 0x30], xCornerPosition + 65, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentPattern % 10) + 0x30], xCornerPosition + 71, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Division we are in Pattern
			//"Div"
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'D'], xCornerPosition + 83, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'i'], xCornerPosition + 89, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'v'], xCornerPosition + 95, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Division Box
			SGE::Render::DrawBox(xCornerPosition + 103, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Division Number
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentDivision / 10) + 0x30], xCornerPosition + 104, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentDivision % 10) + 0x30], xCornerPosition + 110, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Sample Block
			//"Smp"
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'S'], xCornerPosition + 125, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'m'], xCornerPosition + 132, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'p'], xCornerPosition + 137, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Sample Boxes
			//Channel 1 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 15, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[0] / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 16, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[0] % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 16, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 2 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 26, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[1] / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 27, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[1] % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 27, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 3 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 37, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[2] / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 38, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[2] % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 38, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 4 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 48, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[3] / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 49, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->CurrentChannelSamples[3] % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 49, SGE::Render::Colors::ColorMode8Bit[40]);

			//Draw Channel Meters
			DrawLevelMeter((char*)"CH1", xCornerPosition + 5, yCornerPosition + 16, modulePlayerToUse->channelMap[0]->LastRenderedAverageLevel, modulePlayerToUse->channelMap[0]->Volume);
			DrawLevelMeter((char*)"CH2", xCornerPosition + 5, yCornerPosition + 27, modulePlayerToUse->channelMap[1]->LastRenderedAverageLevel, modulePlayerToUse->channelMap[1]->Volume);
			DrawLevelMeter((char*)"CH3", xCornerPosition + 5, yCornerPosition + 38, modulePlayerToUse->channelMap[2]->LastRenderedAverageLevel, modulePlayerToUse->channelMap[2]->Volume);
			DrawLevelMeter((char*)"CH4", xCornerPosition + 5, yCornerPosition + 49, modulePlayerToUse->channelMap[3]->LastRenderedAverageLevel, modulePlayerToUse->channelMap[3]->Volume);

			//Draw Frequency Meters
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 15, modulePlayerToUse->channelMap[0]->currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 26, modulePlayerToUse->channelMap[1]->currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 37, modulePlayerToUse->channelMap[2]->currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 48, modulePlayerToUse->channelMap[3]->currentOffsetIncrement);

			//Draw Title of Track at the Bottom
			SGE::Render::DrawString(modulePlayerToUse->modFile.title, SGE::Render::CHARACTER_8x8_ROM, 5, xCornerPosition + 5, yCornerPosition + 60, SGE::Render::Colors::ColorMode8Bit[157]);
		}



		//
		//  Debug function that draws a box with the status elements for the mouse
		//
		void DrawMouseStatusBox(int xCornerPosition, int yCornerPosition)
		{
			char temp[100] = { 0 };
			//
			//Draw Background Box
			//
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 145, 58, SGE::Render::Colors::ColorMode8Bit[2]);
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 145, 58, SGE::Render::Colors::ColorMode8Bit[22]);

			//
			//Position Labels
			//

			//Print string for Mouse's X
			sprintf(temp, "Mouse Position X: %i", SGE::Controls::Mouse::PositionX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 05, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y
			sprintf(temp, "Mouse Position Y: %i", SGE::Controls::Mouse::PositionY);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 15, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print String for Mouse's X Scroll
			sprintf(temp, "Scroll Position X: %i", SGE::Controls::Mouse::ScrollX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 25, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y Scroll
			sprintf(temp, "Scroll Position Y: %i", SGE::Controls::Mouse::ScrollY);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 35, SGE::Render::Colors::ColorMode8Bit[157]);

			//
			//
			//Print Mouse button array
			//
			//

			//
			//Left Mouse Button
			//

			//Draw Label
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'L'], xCornerPosition + 5, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 15,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[0] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Right Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'R'], xCornerPosition + 25, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 35,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[1] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Center Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'M'], xCornerPosition + 45, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 55,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[2] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #1
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'A'], xCornerPosition + 65, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 75,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[3] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #2
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'B'], xCornerPosition + 85, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 95,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[4] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)
		}




		//
		//  Debug function that draws the classic 4 bit palette in a box for testing purposes.
		//
		void DrawColorPalette4bit(int xCornerPosition, int yCornerPosition)
		{
			const int width = 330;
			const int height = 30;


			//Draw Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::BrightBlack);

			//Draw Border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::White);

			//Draw Swatches
			for (int i = 0; i < 16; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 5 + i * 20, yCornerPosition + 5, 20, 20, SGE::Render::Colors::ColorMode4Bit[i]);
			}
		}

		//
		// Debug funciton that draws classic 8 bit palette in a box for testing purposes.
		//
		void DrawColorPalette8bit(int xCornerPosition, int yCornerPosition)
		{
			const int width = 513;
			const int height = 125;


			//Draw Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::BrightBlack);

			//Draw Border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::White);

			//Draw Standard Colors
			for (int i = 0; i < 8; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 05 + i * 31, yCornerPosition + 5, 30, 10, SGE::Render::Colors::ColorMode8Bit[i]);
			}

			//Draw High-intesity Colors
			for (int i = 8; i < 16; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 13 + i * 31, yCornerPosition + 5, 30, 10, SGE::Render::Colors::ColorMode8Bit[i]);
			}

			//Draw the 216 Colors
			//These are arranages a 6 x 36 grid
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 36; j++)
				{
					//Draw Color Block
					SGE::Render::DrawBox(xCornerPosition + 05 + j * 14, yCornerPosition + 20 + i * 14, 13, 13, SGE::Render::Colors::ColorMode8Bit[(i * 36) + 16 + j]);
				}
			}

			//Draw the Grayscale color
			for (int i = 0; i < 24; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 05 + i * 21, yCornerPosition + 110, 20, 10, SGE::Render::Colors::ColorMode8Bit[232 + i]);
			}

		}

		//
		//  Helper function to draw keyboard keys for the Keyboard Status function
		//
		void DrawKeyboardKey(const int xCornerPosition, const int yCornerPosition, const char* label, const int key, const int keyWidth, const int keyHeight)
		{
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, keyWidth, keyHeight, SGE::Controls::Keyboard::Status[key] ? SGE::Render::Colors::Named::White : SGE::Render::Colors::Named::Black);
			SGE::Render::DrawString(label, SGE::Render::CHARACTER_8x8_ROM, 5, xCornerPosition + 1, yCornerPosition + 5, SGE::Controls::Keyboard::Status[key] ? SGE::Render::Colors::Named::Black : SGE::Render::Colors::Named::BrightWhite);
		}

		//
		//  Debug function that draws a keyboard to indicate the current status of all keyboard keys
		//
		void DrawKeyboardStatus(int xCornerPosition, int yCornerPosition)
		{
			const int DEFAULT_KEY_WIDTH = 19;
			const int DEFAULT_KEY_HEIGHT = 19;

			//  Draw background box
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 500, 150, SGE::Render::Colors::Named::BrightBlack);

			//  Draw background border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 500, 150, SGE::Render::Colors::Named::White);

			//
			//  Draw keys
			//

			//
			//  First Row - Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,   PntScr, ScrLck,  Pause
			//

			//  Escape
			DrawKeyboardKey(xCornerPosition + 5, yCornerPosition + 5, (char *)"Esc", SGE::Controls::Keymap::KEY_ESCAPE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F1
			DrawKeyboardKey(xCornerPosition + 30, yCornerPosition + 5, (char *)"F1", SGE::Controls::Keymap::KEY_F1, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F2
			DrawKeyboardKey(xCornerPosition + 50, yCornerPosition + 5, (char *)"F2", SGE::Controls::Keymap::KEY_F2, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F3
			DrawKeyboardKey(xCornerPosition + 70, yCornerPosition + 5, (char *)"F3", SGE::Controls::Keymap::KEY_F3, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F4
			DrawKeyboardKey(xCornerPosition + 90, yCornerPosition + 5, (char *)"F4", SGE::Controls::Keymap::KEY_F4, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F5
			DrawKeyboardKey(xCornerPosition + 115, yCornerPosition + 5, (char *)"F5", SGE::Controls::Keymap::KEY_F5, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F6
			DrawKeyboardKey(xCornerPosition + 135, yCornerPosition + 5, (char *)"F6", SGE::Controls::Keymap::KEY_F6, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F7
			DrawKeyboardKey(xCornerPosition + 155, yCornerPosition + 5, (char *)"F7", SGE::Controls::Keymap::KEY_F7, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F8
			DrawKeyboardKey(xCornerPosition + 175, yCornerPosition + 5, (char *)"F8", SGE::Controls::Keymap::KEY_F8, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F9
			DrawKeyboardKey(xCornerPosition + 200, yCornerPosition + 5, (char *)"F9", SGE::Controls::Keymap::KEY_F9, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F10
			DrawKeyboardKey(xCornerPosition + 220, yCornerPosition + 5, (char *)"F10", SGE::Controls::Keymap::KEY_F10, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F11
			DrawKeyboardKey(xCornerPosition + 240, yCornerPosition + 5, (char *)"F11", SGE::Controls::Keymap::KEY_F11, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F12
			DrawKeyboardKey(xCornerPosition + 260, yCornerPosition + 5, (char *)"F12", SGE::Controls::Keymap::KEY_F12, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//
			//  Second Row - `, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, =, BckSpc, Ins, Hom, PgUp, NumLck, /, *, -
			//

			//  `
			DrawKeyboardKey(xCornerPosition + 5, yCornerPosition + 25, (char *)"~ `", SGE::Controls::Keymap::KEY_GRAVE_ACCENT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  1
			DrawKeyboardKey(xCornerPosition + 25, yCornerPosition + 25, (char *)"1 !", SGE::Controls::Keymap::KEY_1, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  2
			DrawKeyboardKey(xCornerPosition + 45, yCornerPosition + 25, (char *)"2 @", SGE::Controls::Keymap::KEY_2, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  3
			DrawKeyboardKey(xCornerPosition + 65, yCornerPosition + 25, (char *)"3 #", SGE::Controls::Keymap::KEY_3, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  4
			DrawKeyboardKey(xCornerPosition + 85, yCornerPosition + 25, (char *)"4 $", SGE::Controls::Keymap::KEY_4, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  5
			DrawKeyboardKey(xCornerPosition + 105, yCornerPosition + 25, (char *)"5 %", SGE::Controls::Keymap::KEY_5, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  6
			DrawKeyboardKey(xCornerPosition + 125, yCornerPosition + 25, (char *)"6 ^", SGE::Controls::Keymap::KEY_6, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  7
			DrawKeyboardKey(xCornerPosition + 145, yCornerPosition + 25, (char *)"7 &", SGE::Controls::Keymap::KEY_7, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  8
			DrawKeyboardKey(xCornerPosition + 165, yCornerPosition + 25, (char *)"8 *", SGE::Controls::Keymap::KEY_8, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  9
			DrawKeyboardKey(xCornerPosition + 185, yCornerPosition + 25, (char *)"9 (", SGE::Controls::Keymap::KEY_9, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  0
			DrawKeyboardKey(xCornerPosition + 205, yCornerPosition + 25, (char *)"0 )", SGE::Controls::Keymap::KEY_0, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  -
			DrawKeyboardKey(xCornerPosition + 225, yCornerPosition + 25, (char *)"- _", SGE::Controls::Keymap::KEY_MINUS, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  =
			DrawKeyboardKey(xCornerPosition + 245, yCornerPosition + 25, (char *)"+ =", SGE::Controls::Keymap::KEY_EQUAL, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  BkS  (Backspace) Part 1
			DrawKeyboardKey(xCornerPosition + 266, yCornerPosition + 25, (char *)"Bck Spc", SGE::Controls::Keymap::KEY_BACKSPACE, 39, DEFAULT_KEY_HEIGHT);

			//  Ins
			DrawKeyboardKey(xCornerPosition + 310, yCornerPosition + 25, (char *)"Ins", SGE::Controls::Keymap::KEY_INSERT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Hom
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + 25, (char *)"Hom", SGE::Controls::Keymap::KEY_HOME, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  PgU
			DrawKeyboardKey(xCornerPosition + 350, yCornerPosition + 25, (char *)"PgU", SGE::Controls::Keymap::KEY_PAGE_UP, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  NmL  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + 25, (char *)"NmL", SGE::Controls::Keymap::KEY_NUM_LOCK, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  /  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + 25, (char *)"/", SGE::Controls::Keymap::KEY_KP_DIVIDE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  *  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + 25, (char *)"*", SGE::Controls::Keymap::KEY_KP_MULTIPLY, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  -  (Keypad)
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + 25, (char *)"-", SGE::Controls::Keymap::KEY_KP_SUBTRACT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//
			//  Third Row
			//

			//  Tab
			DrawKeyboardKey(xCornerPosition + 5, yCornerPosition + 45, (char *)"Tab", SGE::Controls::Keymap::KEY_TAB, DEFAULT_KEY_WIDTH + 10, DEFAULT_KEY_HEIGHT);

			//  Q
			DrawKeyboardKey(xCornerPosition + 35, yCornerPosition + 45, (char *)"Q", SGE::Controls::Keymap::KEY_Q, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  W
			DrawKeyboardKey(xCornerPosition + 55, yCornerPosition + 45, (char *)"W", SGE::Controls::Keymap::KEY_W, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  E
			DrawKeyboardKey(xCornerPosition + 75, yCornerPosition + 45, (char *)"E", SGE::Controls::Keymap::KEY_E, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  R
			DrawKeyboardKey(xCornerPosition + 95, yCornerPosition + 45, (char *)"R", SGE::Controls::Keymap::KEY_R, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  T
			DrawKeyboardKey(xCornerPosition + 115, yCornerPosition + 45, (char *)"T", SGE::Controls::Keymap::KEY_T, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Y
			DrawKeyboardKey(xCornerPosition + 135, yCornerPosition + 45, (char *)"Y", SGE::Controls::Keymap::KEY_Y, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  U
			DrawKeyboardKey(xCornerPosition + 155, yCornerPosition + 45, (char *)"U", SGE::Controls::Keymap::KEY_U, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  I
			DrawKeyboardKey(xCornerPosition + 175, yCornerPosition + 45, (char *)"I", SGE::Controls::Keymap::KEY_I, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  O
			DrawKeyboardKey(xCornerPosition + 195, yCornerPosition + 45, (char *)"O", SGE::Controls::Keymap::KEY_O, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  P
			DrawKeyboardKey(xCornerPosition + 215, yCornerPosition + 45, (char *)"P", SGE::Controls::Keymap::KEY_P, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  [
			DrawKeyboardKey(xCornerPosition + 235, yCornerPosition + 45, (char *)"[", SGE::Controls::Keymap::KEY_LEFT_BRACKET, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  ]
			DrawKeyboardKey(xCornerPosition + 255, yCornerPosition + 45, (char *)"]", SGE::Controls::Keymap::KEY_RIGHT_BRACKET, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  "\"
			DrawKeyboardKey(xCornerPosition + 275, yCornerPosition + 45, (char *)"\\", SGE::Controls::Keymap::KEY_BACKSLASH, DEFAULT_KEY_WIDTH + 10, DEFAULT_KEY_HEIGHT);

			//  Del
			DrawKeyboardKey(xCornerPosition + 310, yCornerPosition + 45, (char *)"Del", SGE::Controls::Keymap::KEY_DELETE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  End
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + 45, (char *)"End", SGE::Controls::Keymap::KEY_END, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  PgD
			DrawKeyboardKey(xCornerPosition + 350, yCornerPosition + 45, (char *)"PgD", SGE::Controls::Keymap::KEY_PAGE_DOWN, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  7  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + 45, (char *)"7", SGE::Controls::Keymap::KEY_KP_7, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  8  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + 45, (char *)"8", SGE::Controls::Keymap::KEY_KP_8, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  9  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + 45, (char *)"9", SGE::Controls::Keymap::KEY_KP_9, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  +  (Keypad)
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + 45, (char *)"+", SGE::Controls::Keymap::KEY_KP_ADD, DEFAULT_KEY_WIDTH, 2 * DEFAULT_KEY_HEIGHT);

			//
			//  Fourth Row
			//

			//  Cap Lck

			//  A

			//  S

			//  D

			//  F

			//  G

			//  H

			//  J

			//  K

			//  L

			//  ;

			//  '

			//  Enter

			//  4  (Keypad)

			//  5  (Keypad)

			//  6  (Keypad)

			//
			//  Fifth Row
			//

			//  Shift  (Left)

			//  Z

			//  X

			//  C

			//  V

			//  B

			//  N

			//  M

			//  ,

			//  .

			//  /

			//  Shift  (Right)

			//  Up

			//  1  (Keypad)

			//  2  (Keypad)

			//  3  (Keypad)

			//  Enter  (Keypad)

			//
			//  Sixth Row
			//

			//  Ctrl  (Left)

			//  Win  (Left)

			//  Alt  (Left)

			//  Space

			//  Alt  (Right)

			//  Win  (Right)

			//  Menu
		
			//  Ctrl  (Right)

			//  Left

			//  Down

			//  Right

			//  0  (Keypad)

			//  .  (Keypad)


			
		}

	}
}