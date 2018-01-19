#include "main.h"
#include <thread>
#include <string>

void DrawLevelMeter(char label[4], int xCornerPosition, int yCornerPosition, unsigned int sampleLevel, float channelVolume)
{
	//Channel Label
	SGE::Render::DrawString(label, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition, yCornerPosition, 128, 255, 128);

	//
	//Green Lights: 11
	//
	for (int i = 0; i < 11; i++)
	{
		if (sampleLevel > (unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16)
		{
			SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8, 16, 192, 16);
		}
		else
		{
			SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8, 16, 64, 16);
		}
	}

	//
	//Yellow Lights: 4
	//
	for (int i = 11; i < 15; i++)
	{
		if (sampleLevel > (unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16)
		{
			SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8, 192, 192, 16);
		}
		else
		{
			SGE::Render::DrawBox(xCornerPosition + 25 + i * 6, yCornerPosition, 5, 8, 64, 64, 16);
		}
	}

	//Sixthteenth Level Light
	if (sampleLevel > (15 * SGE::Sound::SAMPLE_MAX_AMPLITUDE / 16))
	{
		SGE::Render::DrawBox(xCornerPosition + 115, yCornerPosition, 5, 8, 192, 16, 16);
	}
	else
	{
		SGE::Render::DrawBox(xCornerPosition + 115, yCornerPosition, 5, 8, 64, 16, 16);
	}

	//Draw the Volume level indicator box
	SGE::Render::DrawRectangle(xCornerPosition + 24 +  6 * (int)(channelVolume * 15), yCornerPosition - 1, 7, 10, 192, 192, 192);



}
void DrawFrequencyMeter(int xCornerPosition, int yCornerPosition, float channelFrequency)
{
	char tempString[10];

	float calculatedPeriod = SGE::Sound::ModTracker::NTSC_TUNING / (SGE::Sound::SAMPLE_RATE * 2 * channelFrequency);

	//Draw Frequency bar
	SGE::Render::DrawBox(xCornerPosition + 1, yCornerPosition + 1, 300, 8, 64, 64, 64);

	//Draw Border for Frequency Bar
	SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 302, 10, 16, 64, 16);

	//Draw frequency indicator
	SGE::Render::DrawBox(xCornerPosition + 1 + 300 - ((calculatedPeriod / 1712) * 300), yCornerPosition + 1, 1, 8, 192, 192, 192);

	//Write out current frequency
	sprintf(tempString, "%06.1f", calculatedPeriod);

	//Draw Background
	SGE::Render::DrawBox(xCornerPosition + 305, yCornerPosition, 57, 10, 32, 32, 32);

	//Draw Number from string
	SGE::Render::DrawString(tempString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 305, yCornerPosition + 1, 128, 255, 128);

}

void DrawChannelMonitor(int xCornerPosition, int yCornerPosition, unsigned int channelNumber)
{
	const int CHANNEL_MONITOR_WIDTH = 20;
	const int CHANNEL_MONITOR_HEIGHT = 145;
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
	SGE::Render::DrawBox(xCornerPosition, yCornerPosition, CHANNEL_MONITOR_WIDTH, CHANNEL_MONITOR_HEIGHT, 0, 128, 0);

	//Draw Pan gauge background
	SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING, yCornerPosition, CHANNEL_VOLUME_WIDTH, 8, 64, 64, 64);

	//Draw Pan gauge itself
	SGE::Render::DrawBox(xCornerPosition + CHANNEL_VOLUME_SPACING + (CHANNEL_VOLUME_WIDTH / 2) + SGE::Sound::Channels[channelNumber].Pan * CHANNEL_VOLUME_WIDTH, yCornerPosition, 1, 8, 192, 192, 192);

	//Draw Meter Lights
	//
	//Green Lights: 11
	//
	for (int i = 0; i < 11; i++)
	{
		if (SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16)
		{
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5, 16, 192, 16);
		}
		else
		{
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5, 16, 64, 16);
		}
	}

	//
	//Yellow Lights: 4
	//
	for (int i = 11; i < 15; i++)
	{
		if (SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel >(unsigned int)(i * SGE::Sound::SAMPLE_MAX_AMPLITUDE) / 16)
		{
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5, 192, 192, 16);
		}
		else
		{
			SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 100 - i * 6, CHANNEL_BAR_WIDTH, 5, 64, 64, 16);
		}
	}

	//Sixthteenth Level Light
	if (SGE::Sound::Channels[channelNumber].LastRenderedAverageLevel > (15 * SGE::Sound::SAMPLE_MAX_AMPLITUDE / 16))
	{
		SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 10, CHANNEL_BAR_WIDTH, 5, 192, 16, 16);
	}
	else
	{
		SGE::Render::DrawBox(xCornerPosition + CHANNEL_BAR_SPACING, yCornerPosition + 10, CHANNEL_BAR_WIDTH, 5, 64, 16, 16);
	}

	//Draw the Volume level indicator box
	SGE::Render::DrawRectangle(xCornerPosition + CHANNEL_BAR_SPACING - 1, yCornerPosition + 99 - 6 * (int)(SGE::Sound::Channels[channelNumber].Volume * 15), CHANNEL_BAR_WIDTH + 2, 7, 192, 192, 192);

	//Draw Channel Label
	SGE::Render::DrawString(tempString, SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 108, 128, 255, 128);

	//
	//Draw Mode lights
	//

	//Is Arpeggio On?
	if (SGE::Sound::Channels[channelNumber].EnableArpeggio)
	{
		SGE::Render::DrawString((char*)"AR", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 118, 128, 255, 128);
	}
	else
	{
		SGE::Render::DrawString((char*)"AR", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 118, 64, 64, 64);
	}

	//Is Vibrato On?
	if (SGE::Sound::Channels[channelNumber].EnableVibrato)
	{
		SGE::Render::DrawString((char*)"VB", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 128, 128, 255, 128);
	}
	else
	{
		SGE::Render::DrawString((char*)"VB", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 128, 64, 64, 64);
	}

	//Is Volume Slide On?
	if (SGE::Sound::Channels[channelNumber].EnableVolumeSlide)
	{
		SGE::Render::DrawString((char*)"VS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 138, 128, 255, 128);
	}
	else
	{
		SGE::Render::DrawString((char*)"VS", SGE::Render::CHARACTER_8x8_ROM, 7, xCornerPosition + 3, yCornerPosition + 138, 64, 64, 64);
	}
}

void DrawAudioChannelStatusBox(int xCornerPosition, int yCornerPosition)
{
	//Draw Background Box

	//Draw Background Border

	//Draw bottom channel meters
	for (int i = 0; i < SGE::Sound::MAX_CHANNELS; i++)
	{
		DrawChannelMonitor(xCornerPosition + 5 + (20 * i), yCornerPosition, i);
	}
}

void DrawPlayerStatusBox(SGE::Sound::ModulePlayer* modulePlayerToUse, int xCornerPosition, int yCornerPosition)
{
	//Draw Player Status Boxes
	SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 510, 75, 0, 128, 0);
	SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 510, 75, 0, 064, 0);

	//Draw Song Status Row
	//Song Position
	//"Pos"
	SGE::Render::Draw8x8Character('P', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 05, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('o', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 11, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('s', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 17, yCornerPosition + 5, 128, 255, 128);

	//Position Box
	SGE::Render::DrawBox(xCornerPosition + 25, yCornerPosition + 4, 16, 10, 32, 32, 32);

	//Position Number
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentPosition / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 26, yCornerPosition + 5, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentPosition % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 32, yCornerPosition + 5, 0, 192, 0);

	//Pattern In Use
	//"Pat"
	SGE::Render::Draw8x8Character('P', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 44, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('a', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 50, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('t', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 56, yCornerPosition + 5, 128, 255, 128);

	//Pattern Box
	SGE::Render::DrawBox(xCornerPosition + 64, yCornerPosition + 4, 16, 10, 32, 32, 32);

	//Pattern Number
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentPattern / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 65, yCornerPosition + 5, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentPattern % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 71, yCornerPosition + 5, 0, 192, 0);

	//Division we are in Pattern
	//"Div"
	SGE::Render::Draw8x8Character('D', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 83, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('i', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 89, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('v', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 95, yCornerPosition + 5, 128, 255, 128);

	//Division Box
	SGE::Render::DrawBox(xCornerPosition + 103, yCornerPosition + 4, 16, 10, 32, 32, 32);

	//Division Number
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentDivision / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 104, yCornerPosition + 5, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentDivision % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 110, yCornerPosition + 5, 0, 192, 0);

	//Sample Block
	//"Smp"
	SGE::Render::Draw8x8Character('S', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 125, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('m', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 132, yCornerPosition + 5, 128, 255, 128);
	SGE::Render::Draw8x8Character('p', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 137, yCornerPosition + 5, 128, 255, 128);

	//Sample Boxes
	//Channel 1 Sample Box
	SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 15, 16, 10, 32, 32, 32);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[0] / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 128, yCornerPosition + 16, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[0] % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 135, yCornerPosition + 16, 0, 192, 0);

	//Channel 2 Sample Box
	SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 26, 16, 10, 32, 32, 32);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[1] / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 128, yCornerPosition + 27, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[1] % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 135, yCornerPosition + 27, 0, 192, 0);

	//Channel 3 Sample Box
	SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 37, 16, 10, 32, 32, 32);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[2] / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 128, yCornerPosition + 38, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[2] % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 135, yCornerPosition + 38, 0, 192, 0);

	//Channel 4 Sample Box
	SGE::Render::DrawBox(xCornerPosition + 127, yCornerPosition + 48, 16, 10, 32, 32, 32);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[3] / 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 128, yCornerPosition + 49, 0, 192, 0);
	SGE::Render::Draw8x8Character((modulePlayerToUse->CurrentChannelSamples[3] % 10) + 0x30, SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 135, yCornerPosition + 49, 0, 192, 0);

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
	SGE::Render::DrawString(modulePlayerToUse->modFile.header.title, SGE::Render::CHARACTER_8x8_ROM, 5, xCornerPosition + 5, yCornerPosition + 60, 128, 255, 128);
}

void DrawBufferedRow(unsigned int* buffer, unsigned int bufferSize, int xPosition)
{
	int copyStart = 0;
	int copyLength = bufferSize;

	//Check to see if we can draw this thing anyway.
	if ((xPosition + (int)bufferSize < 0) ||					//If the xPosition is so far off screen that nothing shows up
		(xPosition >= SGE::Display::Video::ResolutionX))
	{
		return;
	}

	//Prune to a valid starting point in the buffer
	if (xPosition < 0)
	{
		copyStart = -xPosition;
		copyLength = bufferSize + xPosition;
		xPosition = 0;
	}

	//Prune the amount of the buffer we are going to copy
	if (xPosition + bufferSize >= (unsigned int)SGE::Display::Video::ResolutionX)
	{
		copyLength = SGE::Display::Video::ResolutionX - xPosition - 1;
	}

	for (int i = 0; i < SGE::Display::Video::ResolutionY; i++)
	{
		//memcpy(&SGE::Display::VideoRAM[xPosition + i * SGE::Display::ResolutionX], &buffer[copyStart], copyLength);
		std::memmove(&SGE::Display::Video::RAM[xPosition + i * SGE::Display::Video::ResolutionX], &buffer[copyStart], copyLength);

	}
}



void DrawMouseStatusBox(int xCornerPosition, int yCornerPosition)
{
	char temp[100] = { 0 };
	//
	//Draw Background Box
	//
	SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 145, 58, 0, 128, 0);
	SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 145, 58, 0, 64, 0);

	//
	//Position Labels
	//

	//Print string for Mouse's X
	sprintf(temp, "Mouse Position X: %i", SGE::Controls::Mouse::PositionX);
	SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 05, 128, 255, 128);

	//Print string for Mouse's Y
	sprintf(temp, "Mouse Position Y: %i", SGE::Controls::Mouse::PositionY);
	SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 15, 128, 255, 128);

	//Print String for Mouse's X Scroll
	sprintf(temp, "Scroll Position X: %i", SGE::Controls::Mouse::ScrollX);
	SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 25, 128, 255, 128);

	//Print string for Mouse's Y Scroll
	sprintf(temp, "Scroll Position Y: %i", SGE::Controls::Mouse::ScrollY);
	SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 35, 128, 255, 128);

	//
	//
	//Print Mouse button array
	//
	//

	//
	//Left Mouse Button
	//

	//Draw Label
	SGE::Render::Draw8x8Character('L', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 5, yCornerPosition + 45, 128, 255, 128);

	//Draw Light
	SGE::Render::DrawBox(
		xCornerPosition + 15,							//X Upper Left Corner Position
		yCornerPosition + 45,							//Y Upper Left Corner Position
		5,												//Width
		8,												//Height
		SGE::Controls::Mouse::Buttons[0] ? 192 : 64,	//Red Color Value  (Changes if active or not)
		16,												//Green Color Value
		16);											//Blue Color Value

	//
	//Right Mouse Button
	//

	//Draw Label
	SGE::Render::Draw8x8Character('R', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 25, yCornerPosition + 45, 128, 255, 128);

	//Draw Light
	SGE::Render::DrawBox(
		xCornerPosition + 35,							//X Upper Left Corner Position
		yCornerPosition + 45,							//Y Upper Left Corner Position
		5,												//Width
		8,												//Height
		SGE::Controls::Mouse::Buttons[1] ? 192 : 64,	//Red Color Value  (Changes if active or not)
		16,												//Green Color Value
		16);											//Blue Color Value

	//
	//Center Mouse Button
	//

	//Draw Label
	SGE::Render::Draw8x8Character('M', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 45, yCornerPosition + 45, 128, 255, 128);

	//Draw Light
	SGE::Render::DrawBox(
		xCornerPosition + 55,							//X Upper Left Corner Position
		yCornerPosition + 45,							//Y Upper Left Corner Position
		5,												//Width
		8,												//Height
		SGE::Controls::Mouse::Buttons[2] ? 192 : 64,	//Red Color Value  (Changes if active or not)
		16,												//Green Color Value
		16);

	//
	//Extra Button #1
	//

	//Draw Label
	SGE::Render::Draw8x8Character('A', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 65, yCornerPosition + 45, 128, 255, 128);

	//Draw Light
	SGE::Render::DrawBox(
		xCornerPosition + 75,							//X Upper Left Corner Position
		yCornerPosition + 45,							//Y Upper Left Corner Position
		5,												//Width
		8,												//Height
		SGE::Controls::Mouse::Buttons[3] ? 192 : 64,	//Red Color Value  (Changes if active or not)
		16,												//Green Color Value
		16);

	//
	//Extra Button #2
	//

	//Draw Label
	SGE::Render::Draw8x8Character('B', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 85, yCornerPosition + 45, 128, 255, 128);

	//Draw Light
	SGE::Render::DrawBox(
		xCornerPosition + 95,							//X Upper Left Corner Position
		yCornerPosition + 45,							//Y Upper Left Corner Position
		5,												//Width
		8,												//Height
		SGE::Controls::Mouse::Buttons[4] ? 192 : 64,	//Red Color Value  (Changes if active or not)
		16,												//Green Color Value
		16);
}


void InputTest(bool& testInputRunning)
{
	char* menuItemText[7] =
	{
		(char*)"Play: Hyper.mod",
		(char*)"Play: Yehat.mod",
		(char*)"Play: Stardstm.mod",
		(char*)"Stop: Hyper.mod",
		(char*)"Stop: Yehat.mod",
		(char*)"Stop: Stardstm.mod",
		(char*)"Exit Demo",		
	};

	//Create the test menu
	SGE::Menu testMenu(0, 350, 150, 88, 2, 12, 2, 7, menuItemText);

	//Current Selection
	testMenu.selection = 0;

	//Background Color
	testMenu.backgroundRColor = 0;
	testMenu.backgroundGColor = 128;
	testMenu.backgroundBColor = 0;

	//Border Color
	testMenu.borderRColor = 0;
	testMenu.borderGColor = 64;
	testMenu.borderBColor = 0;

	//Highlight Color
	testMenu.highlightRColor = 0;
	testMenu.highlightGColor = 255;
	testMenu.highlightBColor = 0;

	//Regular Text Color
	testMenu.textRColor = 64;
	testMenu.textGColor = 64;
	testMenu.textBColor = 64;

	//Highlight Text Color
	testMenu.highlightTextRColor = 0;
	testMenu.highlightTextGColor = 0;
	testMenu. highlightTextBColor = 0;
	
	//Create some players
	SGE::Sound::ModulePlayer modulePlayerTest;
	SGE::Sound::ModulePlayer modulePlayerTest2;
	SGE::Sound::ModulePlayer modulePlayerTest3;
	
	//Load up the module files
	modulePlayerTest.Load((char*)"hyper.mod");
	modulePlayerTest2.Load((char*)"yehat.mod");
	modulePlayerTest3.Load((char*)"stardstm.mod");

	//Connect up to the sound system
	modulePlayerTest.Connect(12, 64);
	modulePlayerTest2.Connect(24, 128);
	modulePlayerTest3.Connect(28, 192);

	testMenu.CursorOn();

	//Create a previous keyboard state
	bool lastKeyboardState[SGE::Controls::Keyboard::NUMBER_OF_KEYS];

	//Capture keyboard state
	SGE::Controls::Keyboard::SaveStatus(lastKeyboardState);


	//Load a bitmap
	SGE::Render::RenderBitmapFile testBitmap;

	testBitmap.LoadFile((char*)"TestImage.bmp");

	//Vector Point List
	SGE::Render::VertexPoint letterSVectorPoints[28]=
	{
		{2,0},
		{7,0},
		{9,2},
		{9,4},
		{7,4},
		{7,3},
		{6,2},
		{3,2},
		{2,3},
		{2,5},
		{3,6},
		{7,6},
		{9,8},
		{9,12},
		{7,14},
		{2,14},
		{0,12},
		{0,10},
		{2,10},
		{2,11},
		{3,12},
		{6,12},
		{7,11},
		{7,9},
		{6,8},
		{2,8},
		{0,6},
		{0,2}
	};

	SGE::Render::VertexPoint letterSTriangleFill[26 * 3]=
	{
		{2,0}, {7,0}, {3,2},	//Triangle 1
		{3,2}, {7,0}, {6,2},	//Triangle 2
		{6,2}, {7,0}, {9,2},	//Triangle 3
		{9,2}, {7,3}, {6,2},	//Triangle 4
		{9,2}, {7,3}, {9,4},	//Triangle 5
		{9,4}, {7,4}, {7,3},	//Triangle 6
		{2,0}, {0,2}, {3,2},	//Triangle 7
		{0,2}, {3,2}, {2,3},	//Triangle 8
		{0,2}, {2,3}, {0,6},	//Triangle 9
		{0,6}, {2,3}, {2,5},	//Triangle 10
		{2,5}, {0,6}, {2,8},	//Triangle 11
		{2,5}, {3,6}, {2,8},	//Triangle 12
		{2,8}, {3,6}, {6,8},	//Triangle 13
		{6,8}, {3,6}, {7,6},	//Triangle 14
		{6,8}, {7,6}, {9,8},	//Triangle 15
		{6,8}, {9,8}, {7,9},	//Triangle 16
		{7,9}, {9,8}, {9,12},	//Triangle 17
		{7,9}, {9,12}, {7,11},	//Triangle 18
		{7,11}, {9,12}, {7,14},	//Triangle 19
		{7,11}, {7,14}, {6,12},	//Triangle 20
		{6,12}, {7,14}, {2,14},	//Triangle 21
		{6,12}, {2,14}, {3,12},	//Triangle 22
		{3,12}, {2,14}, {0,12},	//Triangle 23
		{3,12}, {0,12}, {2,11},	//Triangle 24
		{2,11}, {0,12}, {0,10},	//Triangle 25
		{2,11}, {0,10}, {2,10}	//Triangle 26
	};

	int testingOffsetX = 10;
	int testingOffsetY = 10;

	//Color Buffer for wave
	unsigned int colorWaveBuffer[8] = { 0x000000FF,	0x000000FF,	0x00000E0,	0x000000D0, 0x00000080, 0x00000040, 0x00000020, 0x00000010 };

	int currentWaveX = -8;



	while (testInputRunning)
	{
		//
		//Lock the display refresh
		//
		SGE::Display::BlockRefresh();

		//Blank the virtual display RAM
		SGE::Render::ZBlank();
		
		//Copy over background image
		//SGE::Render::DrawDataBlock(targetDisplay, 0, 0, testBitmap.image.width, testBitmap.image.height, testBitmap.image.imageData);

		//Background Wave
		DrawBufferedRow(colorWaveBuffer, 8, currentWaveX);
		
		currentWaveX++;

		if (currentWaveX > SGE::Display::Video::ResolutionX + 8)
		{
			currentWaveX = -8;
		}


		//Draw Filled triangles
		SGE::Render::DrawFilledTriangles(10, 10, 15.0, letterSTriangleFill, 3 * 26, 0, 128, 64);

		//Draw VertexPoints
		SGE::Render::DrawVectorShape(testingOffsetX, testingOffsetY, 15.0, 28, letterSVectorPoints, 0, 128, 255);

		//Info up in the corner
		SGE::Render::DrawString((char*)"Strange Game Engine Demo", SGE::Render::CHARACTER_8x8_ROM, 6, 160, 10, 128, 255, 128);
		SGE::Render::DrawString((char*)"Version: 0.01", SGE::Render::CHARACTER_8x8_ROM, 6, 160, 20, 128, 255, 128);
		SGE::Render::DrawString((char*)"\"It actually works!\" Ed.", SGE::Render::CHARACTER_8x8_ROM, 6, 160, 30, 128, 255, 128);

		//
		//Draw Mouse Status Box
		//

		DrawMouseStatusBox(0, 270);

	
		//Draw Menu
		testMenu.Draw();

		//DRaw Player status boxes
		//One for the hyper.mod
		DrawPlayerStatusBox(&modulePlayerTest, 160, 40);

		//One for the yehat.mod
		DrawPlayerStatusBox(&modulePlayerTest2, 160, 120);

		//One for the stardstm.mod
		DrawPlayerStatusBox(&modulePlayerTest3, 160, 200);

		//Draw Master Volume Meters
		//Draw Box
		SGE::Render::DrawBox(160, 280, 150, 30, 0, 128, 0);
		SGE::Render::DrawRectangle(160, 280, 150, 30, 0, 64, 0);

		//Draw meters
		DrawLevelMeter((char*)"M-L", 165, 285, SGE::Sound::MasterVolumeAverageLeftLevel, SGE::Sound::MasterVolume);
		DrawLevelMeter((char*)"M-R", 165, 297, SGE::Sound::MasterVolumeAverageRightLevel, SGE::Sound::MasterVolume);

		DrawAudioChannelStatusBox(50, 500);


		//
		//Unlock the display refresh
		//
		SGE::Display::AllowRefresh();

		//Check to see if Enter is pressed and where the menu cursor is
		if (SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_ENTER])
		{
			//If the Enter key wasn't pressed before
			if (!lastKeyboardState[SGE::Controls::Keymap::KEY_ENTER])
			{
				//Check to see where the cursor is and perform the action
				
				switch (testMenu.selection)
				{
				case 0:
					modulePlayerTest.Play();
					break;

				case 1:
					modulePlayerTest2.Play();
					break;

				case 2:
					modulePlayerTest3.Play();
					break;

				case 3:
					modulePlayerTest.Stop();
					break;

				case 4:
					modulePlayerTest2.Stop();
					break;

				case 5:
					modulePlayerTest3.Stop();
					break;

				case 6:
					testInputRunning = false;
					SGE::Controls::ContinueToHandleEvents = false;
				}
			}
		}

		//System Master Volume
		//Decrease volume
		if (SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_Z])
		{
			SGE::Sound::MasterVolume -= .01f;
			printf("Debug - Master Volume: %f\n", SGE::Sound::MasterVolume);
		}

		//Increase volume
		if (SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_X])
		{
			SGE::Sound::MasterVolume += .01f;
			printf("Debug - Master Volume: %f\n", SGE::Sound::MasterVolume);
		}

	
		//Select box stuff
		//If the Down key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_DOWN] != SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_DOWN] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_DOWN])
		{
			testMenu.NextSelection();
		}

		//If the Up key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_UP] != SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_UP] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_UP])
		{
			testMenu.PreviousSelection();
		}

		//If the Left key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_LEFT] != SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_LEFT] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_LEFT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() - 1);
		}

		//If the Right key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_RIGHT] != SGE::Controls::Keyboard::Status[SGE::Controls::Keymap::KEY_RIGHT] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_RIGHT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() + 1);
		}

		//Capture keyboard state
		SGE::Controls::Keyboard::SaveStatus(lastKeyboardState);

		//Wait a little after each iteration
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


//The starting point for all bad ideas...
int main(int argc, char *argv[])
{
	bool inputCheck = true;

	//Check the arguments
	//If an argument got passed
	if (argc > 1)
	{
		//Check to see what it is
	
	}
		
	//Start the Strange Game Engine
	SGE::Startup(640, 480, "Test Title");

	//Initialize Virtual Display
	SGE::Display::Open(320, 240);

	//Change the the Window size
	SGE::Display::SetWindowSize(1280, 720);

	//Change game resolution
	SGE::Display::ChangeGameResolution(1280, 720);


	//Launch the actual logic threads, before handling events.
	//Input Test thread
	std::thread testInputThread(InputTest, std::ref(inputCheck));
	
	//Start drawing
	SGE::Display::StartDrawing();

	//Handle some events
	//Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
	//And we are running the event handler on the main thread
	//Same OSes are weird about non-main threads handling keyboard/mouse input
	SGE::Controls::HandleEvents();
	
	//Interface handling over, window is closed stop the drawing loop if it already hasn't
	SGE::Display::StopDrawing();

	//Signal Input Check to Stop
	inputCheck = false;
	
	//Wait for all the threads to join back
	testInputThread.join();

	//Shutdown the Strange Game Engine
	SGE::Shutdown();

	//If we got here... In theory everything went well...  In theory...
	return 0;
}
