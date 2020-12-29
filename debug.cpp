#include "include\SGE\debug.h"
#include "include\SGE\render.h"
#include "include\SGE\inputs.h"
#include "include\SGE\sound.h"
#include "include\SGE\utility.h"

namespace SGE
{
	namespace Debug
	{
		//
		//  Status of Sample Buffers
		//
		void SGEAPI DrawSampleBuffersStatus(int xCornerPosition, int yCornerPosition)
		{
			const int BACKGROUND_HEIGHT = 55;
			const int BACKGROUND_WIDTH = 545;
			const int STATUS_SQUARE_SIZE = 6;
			const int STATUS_SQUARE_X_SPACING = 8;
			const int STATUS_SQUARE_Y_SPACING = 10;
			const int SIDE_SPACING = 20;
			const int TOP_SPACING = 10;

			char temp[5];

			//  Draw the background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, SGE::Render::Colors::ColorMode8Bit[2]);

			//  Draw the label

			//  Draw all the status lights
			for (int i = 0; i < 4; i++)
			{
				sprintf(temp, "%02X", i * 64);
				SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + TOP_SPACING - 2 + i * STATUS_SQUARE_Y_SPACING, SGE::Render::Colors::ColorMode8Bit[157]);

				for (int j = 0; j < 64; j++)
				{
					//Draw status box
					SGE::Render::DrawBox(
						xCornerPosition + SIDE_SPACING + j * STATUS_SQUARE_X_SPACING, 
						yCornerPosition + TOP_SPACING + i * STATUS_SQUARE_Y_SPACING, 
						STATUS_SQUARE_SIZE, 
						STATUS_SQUARE_SIZE,
						!SGE::Sound::SampleBuffers[j + i * 64] ? SGE::Render::Colors::ColorMode8Bit[52] : SGE::Render::Colors::ColorMode8Bit[40]);
				}
			}

			// Draw channel using indicator
			for (int i = 0; i < SGE::Sound::MAX_CHANNELS; i++)
			{
				if (SGE::Sound::Channels[i].CurrentSampleBufferSet)
				{
					SGE::Render::DrawRectangle(
						xCornerPosition + SIDE_SPACING - 1 + (SGE::Sound::Channels[i].CurrentSampleBuffer % 64) * STATUS_SQUARE_X_SPACING,
						yCornerPosition + TOP_SPACING - 1 + (SGE::Sound::Channels[i].CurrentSampleBuffer / 64) * STATUS_SQUARE_Y_SPACING,
						STATUS_SQUARE_SIZE + 2,
						STATUS_SQUARE_SIZE + 2,
						SGE::Sound::Channels[i].Playing ? SGE::Render::Colors::ColorMode8Bit[255] : SGE::Render::Colors::ColorMode8Bit[0]
					);
				}
			}
		}


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
			SGE::Render::DrawBox(xCornerPosition + 1 + 300 - (int)((calculatedPeriod / 1712) * 300), yCornerPosition + 1, 1, 8, SGE::Render::Colors::ColorMode8Bit[255]);

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
			const int CHANNEL_MONITOR_HEIGHT = 165;
			const int CHANNEL_BAR_WIDTH = 8;
			const int CHANNEL_BAR_SPACING = (CHANNEL_MONITOR_WIDTH - CHANNEL_BAR_WIDTH) / 2;
			const int CHANNEL_VOLUME_SPACING = 2;
			const int CHANNEL_VOLUME_WIDTH = CHANNEL_MONITOR_WIDTH - CHANNEL_VOLUME_SPACING * 2;

			//Check for a valid channel number
			if (channelNumber >= SGE::Sound::MAX_CHANNELS)
			{
				return;
			}

			char channelString[5];
			char sampleString[5];

			sprintf(channelString, "%02i", channelNumber);
			if (SGE::Sound::Channels[channelNumber].CurrentSampleBufferSet)
			{
				sprintf(sampleString, "%02X", SGE::Sound::Channels[channelNumber].CurrentSampleBuffer);
			}
			else
			{
				sprintf(sampleString, "--");
			}

			//Draw Test Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, CHANNEL_MONITOR_WIDTH, CHANNEL_MONITOR_HEIGHT, SGE::Render::Colors::ColorMode8Bit[2]);

			SGE::Render::DrawString(sampleString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 1, SGE::Render::Colors::ColorMode8Bit[226]);

			//Draw Pan gauge background
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 10, CHANNEL_VOLUME_WIDTH, 8, SGE::Render::Colors::ColorMode8Bit[237]);

			//Draw Pan gauge itself
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING + (CHANNEL_VOLUME_WIDTH / 2) - (int) (SGE::Sound::Channels[channelNumber].Pan * CHANNEL_VOLUME_WIDTH), yCornerPosition + 10, 1, 8, SGE::Render::Colors::ColorMode8Bit[255]);

			//
			//  Draw Meter Lights
			//

			//Green Lights: 11
			for (int i = 0; i < 11; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 110 - i * 6, CHANNEL_BAR_WIDTH, 5,
					(SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[40] : SGE::Render::Colors::ColorMode8Bit[22]);
			}

			//Yellow Lights: 4
			for (int i = 11; i < 15; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 110 - i * 6, CHANNEL_BAR_WIDTH, 5,
					(SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16) ? SGE::Render::Colors::ColorMode8Bit[226] : SGE::Render::Colors::ColorMode8Bit[58]);
			}

			//Sixteenth Level Light
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 20, CHANNEL_BAR_WIDTH, 5,
				SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel > (15 * SGE::Sound::SAMPLE_MAX_AMPLITUDE / 16) ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);

			//Draw the Volume level indicator box
			SGE::Render::DrawRectangle(xCornerPosition + CHANNEL_BAR_SPACING - 1, yCornerPosition + 109 - 6 * (int)(SGE::Sound::Channels[channelNumber].Volume * 15), CHANNEL_BAR_WIDTH + 2, 7, SGE::Render::Colors::ColorMode8Bit[255]);

			//Draw Channel Label
			SGE::Render::DrawString(channelString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 118, SGE::Render::Colors::ColorMode8Bit[157]);

			//
			//Draw Mode lights
			//

			//Is Arpeggio On?
			SGE::Render::DrawString((char*)"AR", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 128,
				SGE::Sound::Channels[channelNumber].Arpeggio.Enabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Vibrato On?
			SGE::Render::DrawString((char*)"VB", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 138,
				SGE::Sound::Channels[channelNumber].Vibrato.Enabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Volume Slide On?
			SGE::Render::DrawString((char*)"VS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 148,
				SGE::Sound::Channels[channelNumber].VolumeSlide.Enabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);

			//Is Period Slide On?
			SGE::Render::DrawString((char*)"PS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition + 158,
				SGE::Sound::Channels[channelNumber].PeriodSlide.Enabled ? SGE::Render::Colors::ColorMode8Bit[157] : SGE::Render::Colors::ColorMode8Bit[237]);
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
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'P'], xCornerPosition + 05, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'o'], xCornerPosition + 11, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'s'], xCornerPosition + 17, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Position Box
			SGE::Render::DrawBox(xCornerPosition + 25, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Position Number
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Location.Position / 10) + 0x30], xCornerPosition + 26, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Location.Position % 10) + 0x30], xCornerPosition + 32, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Pattern In Use
			//"Pat"
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'P'], xCornerPosition + 44, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'a'], xCornerPosition + 50, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'t'], xCornerPosition + 56, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Pattern Box
			SGE::Render::DrawBox(xCornerPosition + 64, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Pattern Number
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->modFile.patternTable[modulePlayerToUse->Location.Position] / 10) + 0x30], xCornerPosition + 65, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->modFile.patternTable[modulePlayerToUse->Location.Position] % 10) + 0x30], xCornerPosition + 71, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Division we are in Pattern
			//"Div"
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'D'], xCornerPosition + 83, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'i'], xCornerPosition + 89, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'v'], xCornerPosition + 95, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Division Box
			SGE::Render::DrawBox(xCornerPosition + 103, yCornerPosition + 4, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);

			//Division Number
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Location.Division / 10) + 0x30], xCornerPosition + 104, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Location.Division % 10) + 0x30], xCornerPosition + 110, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[40]);

			//Sample Block
			//"Smp"
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'S'], xCornerPosition + 125, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'m'], xCornerPosition + 132, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'p'], xCornerPosition + 137, yCornerPosition + 5, SGE::Render::Colors::ColorMode8Bit[157]);

			//Sample Boxes
			//Channel 1 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 15, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[0].Sample / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 16, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[0].Sample % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 16, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 2 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 26, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[1].Sample / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 27, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[1].Sample % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 27, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 3 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 37, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[2].Sample / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 38, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[2].Sample % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 38, SGE::Render::Colors::ColorMode8Bit[40]);

			//Channel 4 Sample Box
			SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 48, 16, 10, SGE::Render::Colors::ColorMode8Bit[235]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[3].Sample / 10) + 0x30], xCornerPosition + 128, yCornerPosition + 49, SGE::Render::Colors::ColorMode8Bit[40]);
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(modulePlayerToUse->Channel[3].Sample % 10) + 0x30], xCornerPosition + 135, yCornerPosition + 49, SGE::Render::Colors::ColorMode8Bit[40]);

			//Draw Channel Meters
			DrawLevelMeter((char*)"CH1", xCornerPosition + 5, yCornerPosition + 16, SGE::Sound::Channels[modulePlayerToUse->channelMap[0]].LastRenderedAverageLevel, SGE::Sound::Channels[modulePlayerToUse->channelMap[0]].Volume);
			DrawLevelMeter((char*)"CH2", xCornerPosition + 5, yCornerPosition + 27, SGE::Sound::Channels[modulePlayerToUse->channelMap[1]].LastRenderedAverageLevel, SGE::Sound::Channels[modulePlayerToUse->channelMap[1]].Volume);
			DrawLevelMeter((char*)"CH3", xCornerPosition + 5, yCornerPosition + 38, SGE::Sound::Channels[modulePlayerToUse->channelMap[2]].LastRenderedAverageLevel, SGE::Sound::Channels[modulePlayerToUse->channelMap[2]].Volume);
			DrawLevelMeter((char*)"CH4", xCornerPosition + 5, yCornerPosition + 49, SGE::Sound::Channels[modulePlayerToUse->channelMap[3]].LastRenderedAverageLevel, SGE::Sound::Channels[modulePlayerToUse->channelMap[3]].Volume);

			//Draw Frequency Meters
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 15, SGE::Sound::Channels[modulePlayerToUse->channelMap[0]].currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 26, SGE::Sound::Channels[modulePlayerToUse->channelMap[1]].currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 37, SGE::Sound::Channels[modulePlayerToUse->channelMap[2]].currentOffsetIncrement);
			DrawFrequencyMeter(xCornerPosition + 145, yCornerPosition + 48, SGE::Sound::Channels[modulePlayerToUse->channelMap[3]].currentOffsetIncrement);

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
			sprintf(temp, "Mouse Position X: %i", SGE::Inputs::Mouse::PositionX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 05, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y
			sprintf(temp, "Mouse Position Y: %i", SGE::Inputs::Mouse::PositionY);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 15, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print String for Mouse's X Scroll
			sprintf(temp, "Scroll Position X: %i", SGE::Inputs::Mouse::ScrollX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 25, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y Scroll
			sprintf(temp, "Scroll Position Y: %i", SGE::Inputs::Mouse::ScrollY);
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
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'L'], xCornerPosition + 5, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 15,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Inputs::Mouse::Buttons[0] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Right Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'R'], xCornerPosition + 25, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 35,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Inputs::Mouse::Buttons[1] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Center Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'M'], xCornerPosition + 45, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 55,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Inputs::Mouse::Buttons[2] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #1
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'A'], xCornerPosition + 65, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 75,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Inputs::Mouse::Buttons[3] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #2
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)'B'], xCornerPosition + 85, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 95,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Inputs::Mouse::Buttons[4] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)
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
		// Debug function that draws classic 8 bit palette in a box for testing purposes.
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

			//Draw High-intensity Colors
			for (int i = 8; i < 16; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 13 + i * 31, yCornerPosition + 5, 30, 10, SGE::Render::Colors::ColorMode8Bit[i]);
			}

			//Draw the 216 Colors
			//These are arranges a 6 x 36 grid
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
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, keyWidth, keyHeight, SGE::Inputs::Keyboard::Status[key] ? SGE::Render::Colors::Named::White : SGE::Render::Colors::Named::Black);
			SGE::Render::DrawString(label, SGE::Render::CHARACTER_8x8_ROM, 5, xCornerPosition + 1, yCornerPosition + 5, SGE::Inputs::Keyboard::Status[key] ? SGE::Render::Colors::Named::Black : SGE::Render::Colors::Named::BrightWhite);
		}

		//
		//  Debug function that draws a keyboard to indicate the current status of all keyboard keys
		//
		void DrawKeyboardStatus(int xCornerPosition, int yCornerPosition)
		{
			const int DEFAULT_KEY_WIDTH = 19;
			const int DEFAULT_KEY_HEIGHT = 19;

			const int DEFAULT_KEY_X_SPACING = 20;
			const int DEFAULT_KEY_Y_SPACING = 20;

			const int DEFAULT_BORDER = 5;

			const int DEFAULT_Y_ROW_1 = 5;
			const int DEFAULT_Y_ROW_2 = 35;
			const int DEFAULT_Y_ROW_3 = 55;
			const int DEFAULT_Y_ROW_4 = 75;
			const int DEFAULT_Y_ROW_5 = 95;
			const int DEFAULT_Y_ROW_6 = 115;

			


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
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"Esc", SGE::Inputs::Keymap::ESCAPE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F1
			DrawKeyboardKey(xCornerPosition + 30, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F1", SGE::Inputs::Keymap::F1, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F2
			DrawKeyboardKey(xCornerPosition + 50, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F2", SGE::Inputs::Keymap::F2, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F3
			DrawKeyboardKey(xCornerPosition + 70, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F3", SGE::Inputs::Keymap::F3, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F4
			DrawKeyboardKey(xCornerPosition + 90, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F4", SGE::Inputs::Keymap::F4, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F5
			DrawKeyboardKey(xCornerPosition + 115, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F5", SGE::Inputs::Keymap::F5, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F6
			DrawKeyboardKey(xCornerPosition + 135, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F6", SGE::Inputs::Keymap::F6, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F7
			DrawKeyboardKey(xCornerPosition + 155, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F7", SGE::Inputs::Keymap::F7, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F8
			DrawKeyboardKey(xCornerPosition + 175, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F8", SGE::Inputs::Keymap::F8, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//  F9
			DrawKeyboardKey(xCornerPosition + 200, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F9", SGE::Inputs::Keymap::F9, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F10
			DrawKeyboardKey(xCornerPosition + 220, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F10", SGE::Inputs::Keymap::F10, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F11
			DrawKeyboardKey(xCornerPosition + 240, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F11", SGE::Inputs::Keymap::F11, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F12
			DrawKeyboardKey(xCornerPosition + 260, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"F12", SGE::Inputs::Keymap::F12, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Pnt
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"Pnt", SGE::Inputs::Keymap::PRINT_SCREEN, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  SLk
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"SLk", SGE::Inputs::Keymap::SCROLL_LOCK, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Pse
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + DEFAULT_Y_ROW_1, (char *)"Pse", SGE::Inputs::Keymap::PAUSE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);


			//
			//  Second Row - `, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, =, BckSpc, Ins, Hom, PgUp, NumLck, /, *, -
			//

			//  `
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"~ `", SGE::Inputs::Keymap::GRAVE_ACCENT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  1
			DrawKeyboardKey(xCornerPosition + 25, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"1 !", SGE::Inputs::Keymap::ONE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  2
			DrawKeyboardKey(xCornerPosition + 45, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"2 @", SGE::Inputs::Keymap::TWO, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  3
			DrawKeyboardKey(xCornerPosition + 65, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"3 #", SGE::Inputs::Keymap::THREE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  4
			DrawKeyboardKey(xCornerPosition + 85, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"4 $", SGE::Inputs::Keymap::FOUR, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  5
			DrawKeyboardKey(xCornerPosition + 105, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"5 %", SGE::Inputs::Keymap::FIVE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  6
			DrawKeyboardKey(xCornerPosition + 125, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"6 ^", SGE::Inputs::Keymap::SIX, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  7
			DrawKeyboardKey(xCornerPosition + 145, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"7 &", SGE::Inputs::Keymap::SEVEN, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  8
			DrawKeyboardKey(xCornerPosition + 165, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"8 *", SGE::Inputs::Keymap::EIGHT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  9
			DrawKeyboardKey(xCornerPosition + 185, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"9 (", SGE::Inputs::Keymap::NINE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  0
			DrawKeyboardKey(xCornerPosition + 205, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"0 )", SGE::Inputs::Keymap::ZERO, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  -
			DrawKeyboardKey(xCornerPosition + 225, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"- _", SGE::Inputs::Keymap::MINUS, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  =
			DrawKeyboardKey(xCornerPosition + 245, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"+ =", SGE::Inputs::Keymap::EQUAL, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  BkS  (Backspace) Part 1
			DrawKeyboardKey(xCornerPosition + 265, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"Bck Spc", SGE::Inputs::Keymap::BACKSPACE, DEFAULT_KEY_WIDTH + 21, DEFAULT_KEY_HEIGHT);

			//  Ins
			DrawKeyboardKey(xCornerPosition + 310, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"Ins", SGE::Inputs::Keymap::INSERT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Hom
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"Hom", SGE::Inputs::Keymap::HOME, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  PgU
			DrawKeyboardKey(xCornerPosition + 350, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"PgU", SGE::Inputs::Keymap::PAGE_UP, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  NmL  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"NmL", SGE::Inputs::Keymap::NUM_LOCK, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  /  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"/", SGE::Inputs::Keymap::KP_DIVIDE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  *  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"*", SGE::Inputs::Keymap::KP_MULTIPLY, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  -  (Keypad)
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + DEFAULT_Y_ROW_2, (char *)"-", SGE::Inputs::Keymap::KP_SUBTRACT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//
			//  Third Row
			//

			//  Tab
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"Tab", SGE::Inputs::Keymap::TAB, DEFAULT_KEY_WIDTH + 10, DEFAULT_KEY_HEIGHT);

			//  Q
			DrawKeyboardKey(xCornerPosition + 35, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"Q", SGE::Inputs::Keymap::Q, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  W
			DrawKeyboardKey(xCornerPosition + 55, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"W", SGE::Inputs::Keymap::W, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  E
			DrawKeyboardKey(xCornerPosition + 75, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"E", SGE::Inputs::Keymap::E, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  R
			DrawKeyboardKey(xCornerPosition + 95, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"R", SGE::Inputs::Keymap::R, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  T
			DrawKeyboardKey(xCornerPosition + 115, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"T", SGE::Inputs::Keymap::T, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Y
			DrawKeyboardKey(xCornerPosition + 135, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"Y", SGE::Inputs::Keymap::Y, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  U
			DrawKeyboardKey(xCornerPosition + 155, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"U", SGE::Inputs::Keymap::U, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  I
			DrawKeyboardKey(xCornerPosition + 175, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"I", SGE::Inputs::Keymap::I, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  O
			DrawKeyboardKey(xCornerPosition + 195, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"O", SGE::Inputs::Keymap::O, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  P
			DrawKeyboardKey(xCornerPosition + 215, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"P", SGE::Inputs::Keymap::P, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  [
			DrawKeyboardKey(xCornerPosition + 235, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"[", SGE::Inputs::Keymap::LEFT_BRACKET, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  ]
			DrawKeyboardKey(xCornerPosition + 255, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"]", SGE::Inputs::Keymap::RIGHT_BRACKET, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  "\"
			DrawKeyboardKey(xCornerPosition + 275, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"\\ |", SGE::Inputs::Keymap::BACKSLASH, DEFAULT_KEY_WIDTH + 11, DEFAULT_KEY_HEIGHT);

			//  Del
			DrawKeyboardKey(xCornerPosition + 310, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"Del", SGE::Inputs::Keymap::DELETE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  End
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"End", SGE::Inputs::Keymap::END, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  PgD
			DrawKeyboardKey(xCornerPosition + 350, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"PgD", SGE::Inputs::Keymap::PAGE_DOWN, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  7  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"7", SGE::Inputs::Keymap::KP_7, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  8  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"8", SGE::Inputs::Keymap::KP_8, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  9  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"9", SGE::Inputs::Keymap::KP_9, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  +  (Keypad)
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + DEFAULT_Y_ROW_3, (char *)"+", SGE::Inputs::Keymap::KP_ADD, DEFAULT_KEY_WIDTH, 2 * DEFAULT_KEY_HEIGHT + 1);

			//
			//  Fourth Row
			//

			//  Cap Lck
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"CapLck", SGE::Inputs::Keymap::CAPS_LOCK, DEFAULT_KEY_WIDTH + 15, DEFAULT_KEY_HEIGHT);

			//  A
			DrawKeyboardKey(xCornerPosition + 40, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"A", SGE::Inputs::Keymap::A, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  S
			DrawKeyboardKey(xCornerPosition + 60, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"S", SGE::Inputs::Keymap::S, DEFAULT_KEY_WIDTH , DEFAULT_KEY_HEIGHT);

			//  D
			DrawKeyboardKey(xCornerPosition + 80, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"D", SGE::Inputs::Keymap::D, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  F
			DrawKeyboardKey(xCornerPosition + 100, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"F", SGE::Inputs::Keymap::F, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  G
			DrawKeyboardKey(xCornerPosition + 120, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"G", SGE::Inputs::Keymap::G, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  H
			DrawKeyboardKey(xCornerPosition + 140, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"H", SGE::Inputs::Keymap::H, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  J
			DrawKeyboardKey(xCornerPosition + 160, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"J", SGE::Inputs::Keymap::J, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  K
			DrawKeyboardKey(xCornerPosition + 180, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"K", SGE::Inputs::Keymap::K, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  L
			DrawKeyboardKey(xCornerPosition + 200, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"L", SGE::Inputs::Keymap::L, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  ;
			DrawKeyboardKey(xCornerPosition + 220, yCornerPosition + DEFAULT_Y_ROW_4, (char *)";", SGE::Inputs::Keymap::SEMICOLON, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  '
			DrawKeyboardKey(xCornerPosition + 240, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"'", SGE::Inputs::Keymap::APOSTROPHE, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Enter
			DrawKeyboardKey(xCornerPosition + 260, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"Enter", SGE::Inputs::Keymap::ENTER, DEFAULT_KEY_WIDTH + 26, DEFAULT_KEY_HEIGHT);

			//  4  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"4", SGE::Inputs::Keymap::KP_4, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  5  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"5", SGE::Inputs::Keymap::KP_5, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  6  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_4, (char *)"6", SGE::Inputs::Keymap::KP_6, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//
			//  Fifth Row
			//

			//  Shift  (Left)
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"Shift", SGE::Inputs::Keymap::LEFT_SHIFT, DEFAULT_KEY_WIDTH + 25, DEFAULT_KEY_HEIGHT);

			//  Z
			DrawKeyboardKey(xCornerPosition + 50, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"Z", SGE::Inputs::Keymap::Z, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  X
			DrawKeyboardKey(xCornerPosition + 70, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"X", SGE::Inputs::Keymap::X, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  C
			DrawKeyboardKey(xCornerPosition + 90, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"C", SGE::Inputs::Keymap::C, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  V
			DrawKeyboardKey(xCornerPosition + 110, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"V", SGE::Inputs::Keymap::V, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  B
			DrawKeyboardKey(xCornerPosition + 130, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"B", SGE::Inputs::Keymap::B, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  N
			DrawKeyboardKey(xCornerPosition + 150, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"N", SGE::Inputs::Keymap::N, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  M
			DrawKeyboardKey(xCornerPosition + 170, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"M", SGE::Inputs::Keymap::M, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  ,
			DrawKeyboardKey(xCornerPosition + 190, yCornerPosition + DEFAULT_Y_ROW_5, (char *)",", SGE::Inputs::Keymap::COMMA, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  .
			DrawKeyboardKey(xCornerPosition + 210, yCornerPosition + DEFAULT_Y_ROW_5, (char *)".", SGE::Inputs::Keymap::PERIOD, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  /
			DrawKeyboardKey(xCornerPosition + 230, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"/", SGE::Inputs::Keymap::SLASH, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Shift  (Right)
			DrawKeyboardKey(xCornerPosition + 250, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"Shift", SGE::Inputs::Keymap::RIGHT_SHIFT, DEFAULT_KEY_WIDTH * 2 + 17, DEFAULT_KEY_HEIGHT);

			//  Up
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + DEFAULT_Y_ROW_5, (char *)" ^ ", SGE::Inputs::Keymap::UP, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  1  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"1", SGE::Inputs::Keymap::KP_1, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  2  (Keypad)
			DrawKeyboardKey(xCornerPosition + 395, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"2", SGE::Inputs::Keymap::KP_2, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  3  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"3", SGE::Inputs::Keymap::KP_3, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Enter  (Keypad)
			DrawKeyboardKey(xCornerPosition + 435, yCornerPosition + DEFAULT_Y_ROW_5, (char *)"Ent", SGE::Inputs::Keymap::KP_ENTER, DEFAULT_KEY_WIDTH, 2 * DEFAULT_KEY_HEIGHT + 1);

			//
			//  Sixth Row
			//

			//  Ctrl  (Left)
			DrawKeyboardKey(xCornerPosition + DEFAULT_BORDER, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Ctrl", SGE::Inputs::Keymap::LEFT_CONTROL, DEFAULT_KEY_WIDTH + 10, DEFAULT_KEY_HEIGHT);

			//  Win  (Left)
			DrawKeyboardKey(xCornerPosition + 35, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Win", SGE::Inputs::Keymap::LEFT_SUPER, DEFAULT_KEY_WIDTH + 5, DEFAULT_KEY_HEIGHT);

			//  Alt  (Left)
			DrawKeyboardKey(xCornerPosition + 60, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Alt", SGE::Inputs::Keymap::LEFT_ALT, DEFAULT_KEY_WIDTH + 5, DEFAULT_KEY_HEIGHT);

			//  Space
			DrawKeyboardKey(xCornerPosition + 85, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"   Space", SGE::Inputs::Keymap::SPACE, 6 * DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Alt  (Right)
			DrawKeyboardKey(xCornerPosition + 200, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Alt", SGE::Inputs::Keymap::RIGHT_ALT, DEFAULT_KEY_WIDTH + 5, DEFAULT_KEY_HEIGHT);

			//  Win  (Right)
			DrawKeyboardKey(xCornerPosition + 225, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Win", SGE::Inputs::Keymap::RIGHT_SUPER, DEFAULT_KEY_WIDTH + 5, DEFAULT_KEY_HEIGHT);

			//  Menu
			DrawKeyboardKey(xCornerPosition + 250, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Menu", SGE::Inputs::Keymap::MENU, DEFAULT_KEY_WIDTH + 5, DEFAULT_KEY_HEIGHT);
		
			//  Ctrl  (Right)
			DrawKeyboardKey(xCornerPosition + 275, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"Ctrl", SGE::Inputs::Keymap::RIGHT_CONTROL, DEFAULT_KEY_WIDTH + 10, DEFAULT_KEY_HEIGHT);

			//  Left
			DrawKeyboardKey(xCornerPosition + 310, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"<-", SGE::Inputs::Keymap::LEFT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Down
			DrawKeyboardKey(xCornerPosition + 330, yCornerPosition + DEFAULT_Y_ROW_6, (char *)" v ", SGE::Inputs::Keymap::DOWN, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  Right
			DrawKeyboardKey(xCornerPosition + 350, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"->", SGE::Inputs::Keymap::RIGHT, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);

			//  0  (Keypad)
			DrawKeyboardKey(xCornerPosition + 375, yCornerPosition + DEFAULT_Y_ROW_6, (char *)"0", SGE::Inputs::Keymap::KP_0, DEFAULT_KEY_WIDTH * 2 + 1, DEFAULT_KEY_HEIGHT);

			//  .  (Keypad)
			DrawKeyboardKey(xCornerPosition + 415, yCornerPosition + DEFAULT_Y_ROW_6, (char *)".", SGE::Inputs::Keymap::KP_DECIMAL, DEFAULT_KEY_WIDTH, DEFAULT_KEY_HEIGHT);
		}
	}
}