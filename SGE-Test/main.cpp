#include "main.h"
#include <thread>
#include <string>

void DrawBufferedRow(unsigned int* buffer, unsigned int bufferSize, int xPosition)
{
	int copyStart = 0;
	int copyLength = bufferSize;

	//Check to see if we can draw this thing anyway.
	if ((xPosition + (int)bufferSize < 0) ||					//If the xPosition is so far off screen that nothing shows up
		(xPosition >= SGE::Display::Video::X))
	{
		return;
	}

	//Prune to a valid starting point in the buffer
	if (xPosition < 0)
	{
		copyStart = -xPosition;
		copyLength = bufferSize - copyStart;
		xPosition = 0;
	}

	SGE::Display::Video::pixel *targetRAM = &SGE::Display::Video::RAM[xPosition];

	//Prune the amount of the buffer we are going to copy
	if (xPosition + bufferSize >= (unsigned int)SGE::Display::Video::X)
	{
		copyLength = (SGE::Display::Video::X - xPosition - 1) ;
	}

	//
	//  Copy first row over
	//
	//std::memcpy(targetRAM, buffer + copyStart, copyLength * sizeof(SGE::Display::Video::pixel));
	std::copy(buffer + copyStart, buffer + copyStart + copyLength, targetRAM);


	//
	//
	//
	for (int i = SGE::Display::Video::X; i < SGE::Display::Video::Y * SGE::Display::Video::X; i += SGE::Display::Video::X)
	{
		//std::memcpy(&targetRAM[i], targetRAM, copyLength * sizeof(SGE::Display::Video::pixel));
		std::copy(targetRAM, targetRAM + copyLength, targetRAM + i);

	}
}



void InputTest(bool& testInputRunning)
{
	char* menuItemText[7] =
	{
		(char*)"Play: Player 1",
		(char*)"Play: Player 2",
		(char*)"Play: Player 3",
		(char*)"Stop: Player 1",
		(char*)"Stop: Player 2",
		(char*)"Stop: Player 3",
		(char*)"Exit Demo",		
	};

	//Create the test menu
	SGE::Menu testMenu(0, 350, 150, 88, 2, 12, 2, 7, menuItemText);

	//Current Selection
	testMenu.selection = 0;

	//Background Color
	testMenu.backgroundColor = SGE::Render::PackColors(0, 128, 0);

	//Border Color
	testMenu.borderColor = SGE::Render::PackColors(0, 64, 0);

	//Highlight Color
	testMenu.highlightColor = SGE::Render::PackColors(0, 255, 0);

	//Regular Text Color
	testMenu.regularColor = SGE::Render::PackColors(64, 64, 64);

	//Highlight Text Color
	testMenu.highlightTextColor = SGE::Render::PackColors(0, 0, 0);

	// Create a wave file to load
	SGE::FileFormats::Wave* testWaveFile = new SGE::FileFormats::Wave("TestSample.wav");

	// Load wave into Sample Buffer
	SGE::Sound::SampleBuffers[0].LoadRaw(testWaveFile->numberOfSamples, testWaveFile->audioData[0]);

	// Set the Channel to use this sample
	SGE::Sound::Channels[0].SetSampleBuffer(0);
	SGE::Sound::Channels[0].Play();
	
	
	//Create some players
	SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerTest = new SGE::Utility::ModuleTrackerMusic::ModulePlayer();
	SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerTest2 = new SGE::Utility::ModuleTrackerMusic::ModulePlayer();
	SGE::Utility::ModuleTrackerMusic::ModulePlayer* modulePlayerTest3 = new SGE::Utility::ModuleTrackerMusic::ModulePlayer();
	
	//Load up the module files
	modulePlayerTest->Load((char*)"aurora.mod");
	modulePlayerTest2->Load((char*)"enigma.mod");
	modulePlayerTest3->Load((char*)"stardstm.mod");

	//Connect up to the sound system
	modulePlayerTest->Connect(12, 64);
	modulePlayerTest2->Connect(24, 128);
	modulePlayerTest3->Connect(28, 192);

	testMenu.CursorOn();

	//Create a previous keyboard state
	bool lastKeyboardState[SGE::Inputs::Keyboard::NUMBER_OF_KEYS];

	//Capture keyboard state
	SGE::Inputs::Keyboard::SaveStatus(lastKeyboardState);

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
	unsigned int redWaveBuffer[8] =		{ 0x00000011, 0x00000022, 0x00000044, 0x00000088, 0x000000DD, 0x000000EE, 0x000000FF, 0x000000FF };
	unsigned int greenWaveBuffer[8] =	{ 0x00001100, 0x00002200, 0x00004400, 0x00008800, 0x0000DD00, 0x0000EE00, 0x0000FF00, 0x0000FF00 };
	unsigned int blueWaveBuffer[8] =	{ 0x00110000, 0x00220000, 0x00440000, 0x00880000, 0x00DD0000, 0x00EE0000, 0x00FF0000, 0x00FF0000 };

	int currentWaveXRed = -8;

	float currentWaveXGreen = -8;

	int currentWaveXBlue = -8;

	SGE::Utility::Timer::TimerDelta greenWaveDelta;


	//
	//  Bouncing Test Image
	//
	SGE::Utility::Timer::TimerDelta bouncingImageTravelDelta;

	//Load a bitmap
	SGE::FileFormats::Bitmap testBitmap("TestImage.bmp");

	float bouncingImageX = 700;
	float bouncingImageY = 200;

	int bouncingImageXDelta = -2;
	int bouncingImageYDelta = -1;

	float bouncingImageTimeDelta;

	int bouncingImageXDeltaFlip = 100;
	int bouncingImageYDeltaFlip = 50;


	//
	//  Simple Filled Trinagle Test
	//
	SGE::Render::VertexPoint spinningVertexO1 = { 100, -112 };
	SGE::Render::VertexPoint spinningVertexO2 = { -100, -112 };
	SGE::Render::VertexPoint spinningVertexO3 = { 0, 112 };
	SGE::Render::VertexPoint spinningVertex1 = { 50, 50 };
	SGE::Render::VertexPoint spinningVertex2 = { 50, 150 };
	SGE::Render::VertexPoint spinningVertex3 = { 150, 100 };
	int spinningVertexCX = 1100;
	int spinningVertexCY = 400;

	float spinningDegree = 0;

	//
	//  Hide the OS cursor
	//

	SGE::Display::HideOSMouseCursor();

	//
	//  Create a TextBox
	//
	SGE::GUI::TextBox terminal(25, 80, 645, 525);

	terminal.CurrentCharacterROM = (unsigned long long *)&SGE::Render::CHARACTER_VGA_8x16_ROM;
	terminal.CurrentCellSize = SGE::GUI::TextBox::CellSize::Mode8x16;

	std::chrono::nanoseconds deltaTime = std::chrono::nanoseconds(0);

	std::chrono::time_point<std::chrono::steady_clock> startTime;


	while (testInputRunning)
	{
		//
		//  Lock current time
		//
		startTime = std::chrono::steady_clock::now();


		//
		//  Lock the display refresh
		//
		SGE::Display::BlockRefresh();

		//
		//  Blank the virtual display RAM
		//
		SGE::Render::ZBlank();

		//  Stop to advanced the green wave
		currentWaveXGreen += greenWaveDelta.Stop();

		//  Start back up again
		greenWaveDelta.Start(60);

		//
		//  Background Waves
		//

		//  Based on Processing Loop
		DrawBufferedRow(redWaveBuffer, 8, currentWaveXRed);

		//  Based on time delta
		DrawBufferedRow(greenWaveBuffer, 8, (int)currentWaveXGreen);

		//  Base on number of frames rendered
		DrawBufferedRow(blueWaveBuffer, 8, currentWaveXBlue);



		//  Advance blue wave
		currentWaveXBlue = (SGE::Display::FrameCount % (SGE::Display::Video::X + 8)) - 8;


		currentWaveXRed++;

		if (currentWaveXGreen > SGE::Display::Video::X + 8)
		{
			currentWaveXGreen = -8;
		}


		if (currentWaveXRed > SGE::Display::Video::X + 8)
		{
			currentWaveXRed = -8;
		}

		//
		//  Bouncing Test Graphic
		//
		SGE::Render::DrawDataBlock((int)bouncingImageX, (int)bouncingImageY, testBitmap.width, testBitmap.height, testBitmap.imageData);

		//
		//  Delta changes
		//

		bouncingImageTimeDelta = bouncingImageTravelDelta.Stop();
		bouncingImageTravelDelta.Start(60.0f);

		bouncingImageX += bouncingImageXDelta * bouncingImageTimeDelta;
		bouncingImageY += bouncingImageYDelta * bouncingImageTimeDelta;

		//
		//  Check for delta flip due to going too far off the screen
		//

		//  For X Delta
		if (bouncingImageX < -bouncingImageXDeltaFlip || (bouncingImageX + testBitmap.width - SGE::Display::Video::X) > bouncingImageXDeltaFlip)
		{
			bouncingImageXDelta = -bouncingImageXDelta;
			SGE::Sound::Channels[0].Play();
		}

		//  For Y Delta
		if (bouncingImageY < -bouncingImageYDeltaFlip || (bouncingImageY + testBitmap.height - SGE::Display::Video::Y) > bouncingImageYDeltaFlip)
		{
			bouncingImageYDelta = -bouncingImageYDelta;
			SGE::Sound::Channels[0].Play();
		}




		//Draw Filled triangles
		SGE::Render::DrawFilledTriangles(10, 10, 15.0, letterSTriangleFill, 3 * 26, 0x00408000);

		//Draw VertexPoints
		SGE::Render::DrawVectorShape(testingOffsetX, testingOffsetY, 15.0, 28, letterSVectorPoints, 0x0080FF00);

		//Info up in the corner
		SGE::Render::DrawString8x16((char*)"Strange Game Engine Demo   Version: 0.05", SGE::Render::CHARACTER_VGA_8x16_ROM, 8, 160, 5, 0x0080FF80);
		SGE::Render::DrawString((char*)"\"It actually works!\" Ed.", SGE::Render::CHARACTER_CGA_8x8_ROM, 7, 160, 30, 0x0080FF80);

		//
		//Draw Mouse Status Box
		//

		SGE::Debug::DrawMouseStatusBox(0, 270);


		//Draw Menu
		testMenu.Draw();

		//DRaw Player status boxes
		//One for the hyper.mod
		SGE::Debug::DrawPlayerStatusBox(modulePlayerTest, 160, 40);

		//One for the yehat.mod
		SGE::Debug::DrawPlayerStatusBox(modulePlayerTest2, 160, 120);

		//One for the stardstm.mod
		SGE::Debug::DrawPlayerStatusBox(modulePlayerTest3, 160, 200);

		//Draw Master Volume Meters
		//Draw Box
		SGE::Render::DrawBox(160, 280, 150, 30, 0x00008000);
		SGE::Render::DrawRectangle(160, 280, 150, 30, 0x00004000);

		//Draw meters
		SGE::Debug::DrawLevelMeter((char*)"M-L", 165, 285, SGE::Sound::MasterVolumeAverageLeftLevel, SGE::Sound::MasterVolume);
		SGE::Debug::DrawLevelMeter((char*)"M-R", 165, 297, SGE::Sound::MasterVolumeAverageRightLevel, SGE::Sound::MasterVolume);

		SGE::Debug::DrawAudioChannelStatusBox(0, 550);

		//
		//  Draw Color Test Palettes
		//

		//4-bit Color Palette Test
		SGE::Debug::DrawColorPalette4bit(700, 10);

		//8-bit Color Palette Test
		SGE::Debug::DrawColorPalette8bit(700, 50);

		//
		//  Draw Keyboard Status
		//
		SGE::Debug::DrawKeyboardStatus(160, 315);

		//
		//  Draw Sample Buffer Status
		//
		SGE::Debug::DrawSampleBuffersStatus(5, 475);


		//
		//  Draw spinning Triangle 
		//

		//  Draw the current triangle
		SGE::Render::DrawFilledTriangleFast(spinningVertexCX, spinningVertexCY, spinningVertex1, spinningVertex2, spinningVertex3, SGE::Render::Colors::Named::BrightWhite);

		//  Increment the degree
		spinningDegree += (float)0.1;
		if (spinningDegree > 360)
		{
			spinningDegree -= 360;
		}

		//  Rotate the points
		SGE::Utility::Math::RotatePointAroundPoint(spinningVertexO1.x, spinningVertexO1.y, 0, 0, spinningVertex1.x, spinningVertex1.y, spinningDegree);
		SGE::Utility::Math::RotatePointAroundPoint(spinningVertexO2.x, spinningVertexO2.y, 0, 0, spinningVertex2.x, spinningVertex2.y, spinningDegree);
		SGE::Utility::Math::RotatePointAroundPoint(spinningVertexO3.x, spinningVertexO3.y, 0, 0, spinningVertex3.x, spinningVertex3.y, spinningDegree);

		//
		//  Draw TextBox
		//
		terminal.Draw();

		//
		//  Determine a new random thing to put in the TextBox
		//

		int targetTerminalRow = rand() % 25;
		int targetTerminalColumn = rand() % 80;

		int targetTerminalBackgroundColor = rand() % 256;
		int targetTerminalForegroundColor = rand() % 256;

		int targetTerminalCharacter = rand() % 256;


		//
		//  Put new random  thing into TextBox
		//

		//  Set Background Color
		terminal(targetTerminalRow, targetTerminalColumn).BackColor = SGE::Render::Colors::ColorMode8Bit[targetTerminalBackgroundColor];

		//  Set Foreground Color
		terminal(targetTerminalRow, targetTerminalColumn).ForeColor = SGE::Render::Colors::ColorMode8Bit[targetTerminalForegroundColor];

		//  Set Character
		terminal(targetTerminalRow, targetTerminalColumn).Character = targetTerminalCharacter;

		//
		//  Draw Mouse Cursor
		//
		SGE::Render::DrawMouseSimpleCursor(5, SGE::Render::Colors::Named::BrightWhite);

		//
		//Unlock the display refresh
		//
		SGE::Display::AllowRefresh();

		//Check to see if Enter is pressed and where the menu cursor is
		if (SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_ENTER])
		{
			//If the Enter key wasn't pressed before
			if (!lastKeyboardState[SGE::Inputs::Keymap::KEY_ENTER])
			{
				//Check to see where the cursor is and perform the action

				switch (testMenu.selection)
				{
				case 0:
					modulePlayerTest->Play();
					break;

				case 1:
					modulePlayerTest2->Play();
					break;

				case 2:
					modulePlayerTest3->Play();
					break;

				case 3:
					modulePlayerTest->Stop();
					break;

				case 4:
					modulePlayerTest2->Stop();
					break;

				case 5:
					modulePlayerTest3->Stop();
					break;

				case 6:
					testInputRunning = false;
					SGE::Inputs::ContinueToHandleEvents = false;
				}
			}
		}

		//System Master Volume
		//Decrease volume
		if (SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_Z])
		{
			SGE::Sound::MasterVolume -= .01f;
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Sound, "Master Volume: %f\n", SGE::Sound::MasterVolume);
		}

		//Increase volume
		if (SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_X])
		{
			SGE::Sound::MasterVolume += .01f;
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Sound, "Master Volume: %f\n", SGE::Sound::MasterVolume);
		}


		//Select box stuff
		//If the Down key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Inputs::Keymap::KEY_DOWN] != SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_DOWN] &&
			!lastKeyboardState[SGE::Inputs::Keymap::KEY_DOWN])
		{
			testMenu.NextSelection();
		}

		//If the Up key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Inputs::Keymap::KEY_UP] != SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_UP] &&
			!lastKeyboardState[SGE::Inputs::Keymap::KEY_UP])
		{
			testMenu.PreviousSelection();
		}

		//If the Left key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Inputs::Keymap::KEY_LEFT] != SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_LEFT] &&
			!lastKeyboardState[SGE::Inputs::Keymap::KEY_LEFT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() - 1);
		}

		//If the Right key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Inputs::Keymap::KEY_RIGHT] != SGE::Inputs::Keyboard::Status[SGE::Inputs::Keymap::KEY_RIGHT] &&
			!lastKeyboardState[SGE::Inputs::Keymap::KEY_RIGHT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() + 1);
		}

		//Capture keyboard state
		SGE::Inputs::Keyboard::SaveStatus(lastKeyboardState);

		//
		//  Check time it took to process everything
		//
		deltaTime = std::chrono::steady_clock::now() - startTime;


		//
		//  Wait a little after each iteration
		//
		//std::this_thread::sleep_for(std::chrono::milliseconds(15));
		SGE::Utility::Timer::AccurateWaitForMilliseconds(10, false);

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

	//
	//  Launch the game
	//
	SGE::Launch(1280, 720, "Test Title", 1280, 720, InputTest, inputCheck);

	//
	//If we got here... In theory everything went well...  In theory...
	//
	return 0;
}
