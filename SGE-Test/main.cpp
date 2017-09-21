#include "main.h"


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


	SGE::Sound::SoundSystemWaveFile testFile;

	testFile.LoadFile("TestSample.wav");

	//Load samples into the Sound System
	SGE::Sound::SoundSystem::soundSamplesBuffers[0].LoadSoundBuffer(SGE::Sound::SAMPLE_RATE, testSineWave);
	SGE::Sound::SoundSystem::soundSamplesBuffers[1].LoadSoundBuffer(SGE::Sound::SAMPLE_RATE, testTriangleWave);
	SGE::Sound::SoundSystem::soundSamplesBuffers[2].LoadSoundBuffer(SGE::Sound::SAMPLE_RATE, testPulseWave);


	//Ready the channels
	for (int i = 0; i < 10; i++)
	{
		//Load SineWave in channel
		SGE::Sound::SoundSystem::soundChannels[i].currentSampleBuffer = &SGE::Sound::SoundSystem::soundSamplesBuffers[0];

		//Set the key of the channel
		SGE::Sound::SoundSystem::soundChannels[i].SetKey(1);

		//Set the pitches of the channel
		SGE::Sound::SoundSystem::soundChannels[i].SetPitch(SGE::Sound::MIDI_NOTE_FRENQUENCY[i + 60]);

		//Set the channel to loop
		SGE::Sound::SoundSystem::soundChannels[i].loop = true;

		//Set the envelope
		SGE::Sound::SoundSystem::soundChannels[i].useEnvelope = true;
		SGE::Sound::SoundSystem::soundChannels[i].numberOfEnvelopeEntries = 5;

		//Set the start
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableSampleIndex[0] = 0;
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableVolumeLevel[0] = 0;

		//Set the attack
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableSampleIndex[1] = 500;
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableVolumeLevel[1] = SGE::Sound::FIXED_POINT_BIAS;

		//Set the decay
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableSampleIndex[2] = 1000;
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableVolumeLevel[2] = 800;

		//Set the sustain
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableSampleIndex[3] = 2000;
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableVolumeLevel[3] = 800;
		SGE::Sound::SoundSystem::soundChannels[i].sustainEnabled = true;
		SGE::Sound::SoundSystem::soundChannels[i].sustainEntry = 3;

		//Set the release
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableSampleIndex[4] = 20000;
		SGE::Sound::SoundSystem::soundChannels[i].enveloptableVolumeLevel[4] = 0;

	}

	bool previousReturnState = false;

	testMenu.CursorOn();

	//Create a previous keyboard state
	SGE::Controls::SavedKeyboardState lastKeyboardState;


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
		SGE::VirtualDisplay::refreshHold.lock();

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
		SGE::VirtualDisplay::refreshHold.unlock();


		//Channel 0 Pan
		//Pan to Left
		if (SGE::Controls::keyboardStatus[GLFW_KEY_O])
		{
			SGE::Sound::SoundSystem::soundChannels[0].rightVolume -= 1;
			SGE::Sound::SoundSystem::soundChannels[0].leftVolume += 1;
			printf("Debug - Channel 0 Left Volume: %i  Right Volume: %i\n", SGE::Sound::SoundSystem::soundChannels[0].leftVolume, SGE::Sound::SoundSystem::soundChannels[0].rightVolume);
		}

		//Pan to Right
		if (SGE::Controls::keyboardStatus[GLFW_KEY_P])
		{
			SGE::Sound::SoundSystem::soundChannels[0].rightVolume += 1;
			SGE::Sound::SoundSystem::soundChannels[0].leftVolume -=1;
			printf("Debug - Channel 0 Left Volume: %i  Right Volume: %i\n", SGE::Sound::SoundSystem::soundChannels[0].leftVolume, SGE::Sound::SoundSystem::soundChannels[0].rightVolume);
		}


		//Sound Volume
		//Decrease volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_Q])
		{
			SGE::Sound::SoundSystem::soundChannels[0].volume -= 1;
			printf("Debug - Channel 0 Volume Left: %i\n", SGE::Sound::SoundSystem::soundChannels[0].volume);
		}

		//Increase volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_W])
		{
			SGE::Sound::SoundSystem::soundChannels[0].volume += 1;
			printf("Debug - Channel 0 Volume Left: %i\n", SGE::Sound::SoundSystem::soundChannels[0].volume);
		}


		//Pitch bend
		//Decrease pitch
		if (SGE::Controls::keyboardStatus[GLFW_KEY_A])
		{
			SGE::Sound::SoundSystem::soundChannels[0].SetPitch(SGE::Sound::SoundSystem::soundChannels[0].GetPitch() - 1);

			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", SGE::Sound::SoundSystem::soundChannels[0].GetPitch());
		}

		//Increase pitch
		if (SGE::Controls::keyboardStatus[GLFW_KEY_S])
		{
			SGE::Sound::SoundSystem::soundChannels[0].SetPitch(SGE::Sound::SoundSystem::soundChannels[0].GetPitch() + 1);
			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", SGE::Sound::SoundSystem::soundChannels[0].GetPitch());

		}


		//System Master Volume
		//Decrease volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_Z])
		{
			SGE::Sound::SoundSystem::masterVolume -= 1;
			printf("Debug - Master Volume: %i\n", SGE::Sound::SoundSystem::masterVolume);
		}

		//Increase volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_X])
		{
			SGE::Sound::SoundSystem::masterVolume += 1;
			printf("Debug - Master Volume: %i\n", SGE::Sound::SoundSystem::masterVolume);
		}



		if (lastKeyboardState.KeyChanged(GLFW_KEY_T))
		{

			if (lastKeyboardState.GetPreviousState(GLFW_KEY_M))
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


		//ADSR Toggle
		if (lastKeyboardState.KeyChanged(GLFW_KEY_M))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_M))
			{
				
			}
			else
			{
				for (int i = 0; i < SGE::Sound::MAX_CHANNELS; i++)
				{
					if (SGE::Sound::SoundSystem::soundChannels[i].useEnvelope)
					{
						SGE::Sound::SoundSystem::soundChannels[i].useEnvelope = false;
					}
					else
					{
						SGE::Sound::SoundSystem::soundChannels[i].useEnvelope = true;
					}
				}
			}
		}

		//Sound keys stuff
		//Number Key 0
		if (lastKeyboardState.KeyChanged(GLFW_KEY_0))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_0))
			{
				SGE::Sound::SoundSystem::soundChannels[0].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[0].Trigger();
			}
		}

		//Number Key 1
		if (lastKeyboardState.KeyChanged(GLFW_KEY_1))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_1))
			{
				SGE::Sound::SoundSystem::soundChannels[1].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[1].Trigger();
			}
		}

		//Number Key 2
		if (lastKeyboardState.KeyChanged(GLFW_KEY_2))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_2))
			{
				SGE::Sound::SoundSystem::soundChannels[2].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[2].Trigger();
			}
		}

		//Number Key 3
		if (lastKeyboardState.KeyChanged(GLFW_KEY_3))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_3))
			{
				SGE::Sound::SoundSystem::soundChannels[3].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[3].Trigger();
			}
		}

		//Number Key 4
		if (lastKeyboardState.KeyChanged(GLFW_KEY_4))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_4))
			{
				SGE::Sound::SoundSystem::soundChannels[4].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[4].Trigger();
			}
		}

		//Number Key 5
		if (lastKeyboardState.KeyChanged(GLFW_KEY_5))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_5))
			{
				SGE::Sound::SoundSystem::soundChannels[5].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[5].Trigger();
			}
		}

		//Number Key 6
		if (lastKeyboardState.KeyChanged(GLFW_KEY_6))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_6))
			{
				SGE::Sound::SoundSystem::soundChannels[6].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[6].Trigger();
			}
		}

		//Number Key 7
		if (lastKeyboardState.KeyChanged(GLFW_KEY_7))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_7))
			{
				SGE::Sound::SoundSystem::soundChannels[7].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[7].Trigger();
			}
		}

		//Number Key 8
		if (lastKeyboardState.KeyChanged(GLFW_KEY_8))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_8))
			{
				SGE::Sound::SoundSystem::soundChannels[8].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[8].Trigger();
			}
		}

		//Number Key 9
		if (lastKeyboardState.KeyChanged(GLFW_KEY_9))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_9))
			{
				SGE::Sound::SoundSystem::soundChannels[9].Release();
			}
			else
			{
				SGE::Sound::SoundSystem::soundChannels[9].Trigger();
			}
		}

		//Select box stuff
		//If the Down key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_DOWN) && !lastKeyboardState.GetPreviousState(GLFW_KEY_DOWN))
		{
			testMenu.NextSelection();
		}

		//If the Up key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_UP) && !lastKeyboardState.GetPreviousState(GLFW_KEY_UP))
		{
			testMenu.PreviousSelection();
		}

		//If the Left key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_LEFT) && !lastKeyboardState.GetPreviousState(GLFW_KEY_LEFT))
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() - 1);
		}

		//If the Right key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_RIGHT) && !lastKeyboardState.GetPreviousState(GLFW_KEY_RIGHT))
		{
			testMenu.MoveCursor(testMenu.GetCursorLocation() + 1);
		}

		//If the Add key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_KP_ADD) && !lastKeyboardState.GetPreviousState(GLFW_KEY_KP_ADD))
		{
			//Grab the current string and increment the number
			char * tempString = testMenu.GetMenuSelection();
			tempString[testMenu.GetCursorLocation()] = (tempString[testMenu.GetCursorLocation()] + 1) % 256;
			testMenu.SetMenuSelection(tempString);
			delete tempString;
		}

		//If the Subtract key state has changed and it wasn't pressed previously
		if (lastKeyboardState.KeyChanged(GLFW_KEY_KP_SUBTRACT) && !lastKeyboardState.GetPreviousState(GLFW_KEY_KP_SUBTRACT))
		{
			//Grab the current string and decrement the number
			char * tempString = testMenu.GetMenuSelection();
			tempString[testMenu.GetCursorLocation()] = (tempString[testMenu.GetCursorLocation()] - 1) % 256;
			testMenu.SetMenuSelection(tempString);
			delete tempString;
		}

		//Save the current keyboard state
		lastKeyboardState.SaveState();


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
	SGE::System::Startup();

	//Open Game Window
	SGE::System::OpenGameWindow(640, 480, "Test Title");

	//Initialize Virtual Display
	SGE::System::InitializeVirtualDisplay(320, 240);


	//Launch the actual logic threads, before handling events.

	//Input Test thread
	std::thread testInputThread(InputTest, std::ref(inputCheck));
	
	//Start drawing
	SGE::System::StartDrawing();

	//Handle some events
	//Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
	//And we are running the event handler on the main thread
	//Same OSes are weird about non-main threads handling keyboard/mouse input
	SGE::System::HandleEvents();
	
	//Interface handling over, window is closed stop the drawing loop if it already hasn't
	SGE::System::StopDrawing();


	//Signal Input Check to Stop
	inputCheck = false;
	
	//Wait for all the threads to join back
	testInputThread.join();

	//Shutdown the Strange Game Engine
	SGE::System::Shutdown();

	//If we got here... In theory everything went well...  In theory...
	return 0;
}
