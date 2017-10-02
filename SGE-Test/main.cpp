﻿#include "main.h"
#include <thread>


void InputTest(bool& testInputRunning)
{
	char* menuItemText[5] =
	{
		"abcdefghijklmnopqrstuvwxyz",
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		"01234567890!@#$%^&*()-_=+~`",
		"<>,.?/:;'\"{}[]|\\",
		"\xB0\xB1\xB2\x7F",
		
	};

	SGE::Menu testMenu(40, 50, 250, 100, 10, 12, 2, 5, menuItemText);

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

	//Generate Sine Wave some channel

	short* testSineWave = SGE::Sound::Generators::SineGenerator(1, SGE::Sound::SAMPLE_RATE, 5000);
	short* testTriangleWave = SGE::Sound::Generators::TriangleGenerator(1, SGE::Sound::SAMPLE_RATE, 5000);
	short* testPulseWave = SGE::Sound::Generators::PulseGenerator(1, 0.50, SGE::Sound::SAMPLE_RATE, 5000);


	SGE::Sound::WaveFile testFile;

	testFile.LoadFile("TestSample.wav");

	//Load samples into the Sound System
	SGE::Sound::SampleBuffers[0].Load(SGE::Sound::SAMPLE_RATE, testSineWave);
	SGE::Sound::SampleBuffers[1].Load(SGE::Sound::SAMPLE_RATE, testTriangleWave);
	SGE::Sound::SampleBuffers[2].Load(SGE::Sound::SAMPLE_RATE, testPulseWave);


	//Ready the channels
	for (int i = 0; i < 10; i++)
	{
		//Load SineWave in channel
		SGE::Sound::Channels[i].currentSampleBuffer = &SGE::Sound::SampleBuffers[0];

		//Set the pitches of the channel
		SGE::Sound::Channels[i].offsetIncrement = (SGE::Sound::MIDI_NOTE_FRENQUENCY[i + 60]);

		//Set the channel to loop
		SGE::Sound::Channels[i].Repeatable = true;

		//Set repeat duration
		SGE::Sound::Channels[i].repeatDuration = SGE::Sound::Channels[i].currentSampleBuffer->bufferSize;
	}

	bool previousReturnState = false;

	testMenu.CursorOn();

	//Create a previous keyboard state
	bool lastKeyboardState[SGE::Controls::NUMBER_OF_KEYS];

	//Capture keyboard state
	SGE::Controls::SaveKeyboardStatus(lastKeyboardState);


	//Load a bitmap
	SGE::Render::RenderBitmapFile testBitmap;

	testBitmap.LoadFile("TestImage.bmp");


	//Vector Point List
	SGE::Render::VertexPoint testVertexPoint[4]=
	{
		{10, 25},
		{125, 10},
		{125, 175},
		{30,125}
	};

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

	
	bool triangleFlip = false;


	int testingOffsetX = 10;
	int testingOffsetY = 10;


	while (testInputRunning)
	{
		//Lock the display refresh
		SGE::Display::BlockRefresh();

		//Blank the virtual display RAM
		//SGE::Render::Blank(targetDisplay, 0, 0, 0);
		SGE::Render::ZBlank();

		//Copy over background image
		//SGE::Render::DrawDataBlock(targetDisplay, 0, 0, testBitmap.image.width, testBitmap.image.height, testBitmap.image.imageData);


		if (triangleFlip)
		{
			//Draw Filled triangles
			SGE::Render::DrawFilledTriangles(10, 10, 15.0, letterSTriangleFill, 3 * 26, 128, 64, 0);

			//Draw VertexPoints
			SGE::Render::DrawVectorShape(testingOffsetX, testingOffsetY, 15.0, 28, letterSVectorPoints, 255, 128, 0);
		}
		else
		{
			//Draw Filled triangles
			SGE::Render::DrawFilledTriangles(10, 10, 15.0, letterSTriangleFill, 3 * 26, 128, 64, 0);

			SGE::Render::DrawVectorShape(testingOffsetX, testingOffsetY, 15.0, 28, letterSVectorPoints, 0, 255, 128);
		}

	
		//Draw Menu
		testMenu.Draw();
		
		//Unlock the display refresh
		SGE::Display::AllowRefresh();

		//Channel 0 Pan
		//Pan to Left

		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_O])
		{
			SGE::Sound::Channels[0].Pan -= .01f;
			SGE::Sound::Channels[0].Pan += .01f;
			printf("Debug - Channel 0 Pan: %f\n", SGE::Sound::Channels[0].Pan);
		}

		//Pan to Right
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_P])
		{
			SGE::Sound::Channels[0].Pan += .01f;
			SGE::Sound::Channels[0].Pan -= .01f;
			printf("Debug - Channel 0 Pan: %f\n", SGE::Sound::Channels[0].Pan);
		}


		//Sound Volume
		//Decrease volume
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_Q])
		{
			SGE::Sound::Channels[0].Volume -= .01f;
			printf("Debug - Channel 0 Volume Left: %f\n", SGE::Sound::Channels[0].Volume);
		}

		//Increase volume
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_W])
		{
			SGE::Sound::Channels[0].Volume += .01f;
			printf("Debug - Channel 0 Volume Left: %f\n", SGE::Sound::Channels[0].Volume);
		}


		//Pitch bend
		//Decrease pitch
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_A])
		{
			SGE::Sound::Channels[0].offsetIncrement -= .1f;

			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", SGE::Sound::Channels[0].offsetIncrement);
		}

		//Increase pitch
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_S])
		{
			SGE::Sound::Channels[0].offsetIncrement += .1f;
			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", SGE::Sound::Channels[0].offsetIncrement);

		}


		//System Master Volume
		//Decrease volume
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_Z])
		{
			SGE::Sound::MasterVolume -= .01f;
			printf("Debug - Master Volume: %f\n", SGE::Sound::MasterVolume);
		}

		//Increase volume
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_X])
		{
			SGE::Sound::MasterVolume += .01f;
			printf("Debug - Master Volume: %f\n", SGE::Sound::MasterVolume);
		}



		if (lastKeyboardState[SGE::Controls::Keymap::KEY_T] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_T])
		{

			if (lastKeyboardState[SGE::Controls::Keymap::KEY_T])
			{

			}
			else
			{
				if (triangleFlip)
				{
					triangleFlip = false;
				}
				else
				{
					triangleFlip = true;
				}
			}
		}


		//Sound keys stuff
		//Number Key 0
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_0] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_0])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_0])
			{
				SGE::Sound::Channels[0].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[0].Repeatable = true;
				SGE::Sound::Channels[0].Play();
			}
		}

		//Number Key 1
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_1] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_1])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_1])
			{
				SGE::Sound::Channels[1].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[1].Repeatable = true;
				SGE::Sound::Channels[1].Play();
			}
		}

		//Number Key 2
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_2] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_2])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_2])
			{
				SGE::Sound::Channels[2].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[2].Repeatable = true;
				SGE::Sound::Channels[2].Play();
			}
		}

		//Number Key 3
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_3] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_3])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_3])
			{
				SGE::Sound::Channels[3].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[3].Repeatable = true;
				SGE::Sound::Channels[3].Play();
			}
		}

		//Number Key 4
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_4] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_4])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_4])
			{
				SGE::Sound::Channels[4].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[4].Repeatable = true;
				SGE::Sound::Channels[4].Play();
			}
		}

		//Number Key 5
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_5] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_5])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_5])
			{
				SGE::Sound::Channels[5].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[5].Repeatable = true;
				SGE::Sound::Channels[5].Play();
			}
		}

		//Number Key 6
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_6] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_6])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_6])
			{
				SGE::Sound::Channels[6].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[6].Repeatable = true;
				SGE::Sound::Channels[6].Play();
			}
		}

		//Number Key 7
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_7] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_7])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_7])
			{
				SGE::Sound::Channels[7].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[7].Repeatable = true;
				SGE::Sound::Channels[7].Play();
			}
		}

		//Number Key 8
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_8] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_8])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_8])
			{
				SGE::Sound::Channels[8].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[8].Repeatable = true;
				SGE::Sound::Channels[8].Play();
			}
		}

		//Number Key 9
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_9] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_9])
		{
			if (!SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_9])
			{
				SGE::Sound::Channels[9].Repeatable = false;
			}
			else
			{
				SGE::Sound::Channels[9].Repeatable = true;
				SGE::Sound::Channels[9].Play();
			}
		}

		//Select box stuff
		//If the Down key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_DOWN] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_DOWN] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_DOWN])
		{
			testMenu.NextSelection();
		}

		//If the Up key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_UP] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_UP] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_UP])
		{
			testMenu.PreviousSelection();
		}

		//If the Left key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_LEFT] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_LEFT] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_LEFT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() - 1);
		}

		//If the Right key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_RIGHT] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_RIGHT] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_RIGHT])
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() + 1);
		}

		//If the Add key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_KP_ADD] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_KP_ADD] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_KP_ADD])
		{
			//Grab the current string and increment the number
			char * tempString = testMenu.GetMenuSelection();
			tempString[testMenu.GetCursorLocation()] = (tempString[testMenu.GetCursorLocation()] + 1) % 256;
			testMenu.SetMenuSelection(tempString);
			delete tempString;
		}

		//If the Subtract key state has changed and it wasn't pressed previously
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_KP_SUBTRACT] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_KP_SUBTRACT] &&
			!lastKeyboardState[SGE::Controls::Keymap::KEY_KP_SUBTRACT])
		{
			//Grab the current string and decrement the number
			char * tempString = testMenu.GetMenuSelection();
			tempString[testMenu.GetCursorLocation()] = (tempString[testMenu.GetCursorLocation()] - 1) % 256;
			testMenu.SetMenuSelection(tempString);
			delete tempString;
		}

		//Capture keyboard state
		SGE::Controls::SaveKeyboardStatus(lastKeyboardState);


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
