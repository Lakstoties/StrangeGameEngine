#include "main.h"


void TestPattern(bool& testPatternRunning, int displayX, int displayY, unsigned int targetVideoRAM[])
{
	int rColor = 0;
	int gColor = 0;
	int bColor = 0;
	int aColor = 255;

	while (testPatternRunning)
	{
		//Test Pattern!
		for (int i = 0; i < (displayX * displayY); i++)
		{
			rColor = (rColor + 1) % 256;
			gColor = (gColor + 3) % 256;
			bColor = (bColor + 5) % 256;

			targetVideoRAM[i] = SGE::Render::PackColors(rColor, gColor, bColor);

		}
		rColor++;
		gColor++;
		bColor++;

		//Wait a little after each iteration
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void AnimationPattern(bool& testPatternRunning, SGE::VirtualDisplay* targetDisplay)
{
	int shiftDraw = 0;
	unsigned int* testDataBlock;

	testDataBlock = new unsigned int[25 * 25];
	memset(testDataBlock, 255, 25 * 25 * 4);

	while (testPatternRunning)
	{
		//Test Pattern!

		//Lock the display refresh
		targetDisplay->refreshHold.lock();

		SGE::Render::ZBlank(targetDisplay);
		SGE::Render::DrawRow(targetDisplay, 10, 201, 400, 20, 50, 150);
		SGE::Render::DrawColumn(targetDisplay, 200, 20, 300, 150, 20, 50);
		SGE::Render::DrawRectangle(targetDisplay, 0, 0, 319 - shiftDraw, 239 - shiftDraw, 50, 150, 20);
		SGE::Render::DrawLine(targetDisplay, 50 + shiftDraw, 50, 200 - shiftDraw, 200, 75, 200, 200);
		SGE::Render::DrawDataBlock(targetDisplay, 0+shiftDraw, 25, 25, 25, testDataBlock);

		//Unlock the display refresh
		targetDisplay->refreshHold.unlock();

		shiftDraw = (shiftDraw + 1) % 239;

		//Wait a little after each iteration
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	delete[] testDataBlock;
}

void InputTest(bool& testInputRunning, SGE::VirtualDisplay* targetDisplay, SGE::SoundSystem* targetSoundSystem)
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

	short* testSineWave = targetSoundSystem->SineGenerator(1, SGE::SoundSystem::SAMPLE_RATE, 5000);
	short* testTriangleWave = targetSoundSystem->TriangleGenerator(1, SGE::SoundSystem::SAMPLE_RATE, 5000);
	short* testPulseWave = targetSoundSystem->PulseGenerator(1, 0.50, SGE::SoundSystem::SAMPLE_RATE, 5000);


	SGE::SoundSystemWaveFile testFile;

	testFile.LoadFile("TestSample.wav");


	//Ready the channels
	for (int i = 0; i < 10; i++)
	{
		//Load SineWave in channel
		//targetSoundSystem->soundChannels[i].LoadSoundBuffer(testFile.numberOfSamples, testFile.audioData[0]);
		targetSoundSystem->soundChannels[i].LoadSoundBuffer(SGE::SoundSystem::SAMPLE_RATE, testSineWave);


		//Set the key of the channel
		targetSoundSystem->soundChannels[i].SetKey(1);

		//Set the pitches of the channel
		targetSoundSystem->soundChannels[i].SetPitch(targetSoundSystem->MIDI_NOTE_FRENQUENCY[i + 60]);

		//Set the channel to loop
		targetSoundSystem->soundChannels[i].loop = true;


		//Set ADSR
		targetSoundSystem->soundChannels[i].attackRate = 0.001f;
		targetSoundSystem->soundChannels[i].decayRate = 0.01f;
		targetSoundSystem->soundChannels[i].sustainLevel = 0.90f;
		targetSoundSystem->soundChannels[i].releaseRate = .0001f;

		targetSoundSystem->soundChannels[i].adsrActive = true;
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

	
	bool triangleFlip = false;


	while (testInputRunning)
	{
		//Lock the display refresh
		targetDisplay->refreshHold.lock();

		//Blank the virtual display RAM
		SGE::Render::Blank(targetDisplay, 0, 0, 0);
		//SGE::Render::ZBlank(targetDisplay);

		//Copy over background image
		SGE::Render::DrawDataBlock(targetDisplay, 0, 0, testBitmap.image.width, testBitmap.image.height, testBitmap.image.imageData);


		//Draw alignment lines
		SGE::Render::DrawRow(targetDisplay, 0, 110, 10, 128, 128, 128);
		SGE::Render::DrawRow(targetDisplay, 0, 10, 110, 128, 128, 128);
		SGE::Render::DrawRow(targetDisplay, 0, 210, 10, 128, 128, 128);
		SGE::Render::DrawColumn(targetDisplay, 210, 0, 10, 128, 128, 128);



		//Draw VertexPoints
		SGE::Render::DrawVectorShape(targetDisplay, 0, 0, 10.0, 28, letterSVectorPoints, 255, 128, 0);

		if (triangleFlip)
		{
			//Draw filled triangle
			SGE::Render::DrawFilledTriangleTrue(targetDisplay, 10, 10, 1.0, { 0,0 }, { 0,200 }, { 200,0 }, 255, 0, 255);

			//Draw filled triangle
			SGE::Render::DrawFilledTriangleFast(targetDisplay, 10, 10, 1.0, { 0,0 }, { 0,200 }, { 200,0 }, 255, 255, 0);

		}
		else
		{
			//Draw filled triangle
			SGE::Render::DrawFilledTriangleFast(targetDisplay, 10, 10, 1.0, { 0,0 }, { 0,200 }, { 200,0 }, 255, 255, 0);

			//Draw filled triangle
			SGE::Render::DrawFilledTriangleTrue(targetDisplay, 10, 10, 1.0, { 0,0 }, { 0,200 }, { 200,0 }, 255, 0, 255);
		}





		
		//Draw Menu
		//testMenu.Draw(targetDisplay);
		
		//Unlock the display refresh
		targetDisplay->refreshHold.unlock();


		//Channel 0 Pan
		//Pan to Left
		if (SGE::Controls::keyboardStatus[GLFW_KEY_O])
		{
			targetSoundSystem->soundChannels[0].SetPan(targetSoundSystem->soundChannels[0].GetPan() - 0.01f);
			printf("Debug - Channel 0 Pan: %f\n", targetSoundSystem->soundChannels[0].GetPan());
		}

		//Pan to Right
		if (SGE::Controls::keyboardStatus[GLFW_KEY_P])
		{
			targetSoundSystem->soundChannels[0].SetPan(targetSoundSystem->soundChannels[0].GetPan() + 0.01f);
			printf("Debug - Channel 0 Pan: %f\n", targetSoundSystem->soundChannels[0].GetPan());
		}


		//Sound Volume
		//Decrease volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_Q])
		{
			targetSoundSystem->soundChannels[0].volume -= .001f;
			printf("Debug - Channel 0 Volume: %f\n", targetSoundSystem->soundChannels[0].volume);
		}

		//Increase volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_W])
		{
			targetSoundSystem->soundChannels[0].volume += .001f;
			printf("Debug - Channel 0 Volume: %f\n", targetSoundSystem->soundChannels[0].volume);
		}


		//Pitch bend
		//Decrease pitch
		if (SGE::Controls::keyboardStatus[GLFW_KEY_A])
		{
			targetSoundSystem->soundChannels[0].SetPitch(targetSoundSystem->soundChannels[0].GetPitch() - 1);

			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", targetSoundSystem->soundChannels[0].GetPitch());
		}

		//Increase pitch
		if (SGE::Controls::keyboardStatus[GLFW_KEY_S])
		{
			targetSoundSystem->soundChannels[0].SetPitch(targetSoundSystem->soundChannels[0].GetPitch() + 1);
			//DEBUG Output
			printf("Debug - Channel 0 - Pitch: %f\n", targetSoundSystem->soundChannels[0].GetPitch());

		}


		//System Master Volume
		//Decrease volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_Z])
		{
			targetSoundSystem->masterVolume -= .001f;
			printf("Debug - Master Volume: %f\n", targetSoundSystem->masterVolume);
		}

		//Increase volume
		if (SGE::Controls::keyboardStatus[GLFW_KEY_X])
		{
			targetSoundSystem->masterVolume += .001f;
			printf("Debug - Master Volume: %f\n", targetSoundSystem->masterVolume);
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
				for (int i = 0; i < targetSoundSystem->MAX_CHANNELS; i++)
				{
					if (targetSoundSystem->soundChannels[i].adsrActive)
					{
						targetSoundSystem->soundChannels[i].adsrActive = false;
					}
					else
					{
						targetSoundSystem->soundChannels[i].adsrActive = true;
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
				targetSoundSystem->soundChannels[0].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[0].Trigger();
			}
		}

		//Number Key 1
		if (lastKeyboardState.KeyChanged(GLFW_KEY_1))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_1))
			{
				targetSoundSystem->soundChannels[1].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[1].Trigger();
			}
		}

		//Number Key 2
		if (lastKeyboardState.KeyChanged(GLFW_KEY_2))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_2))
			{
				targetSoundSystem->soundChannels[2].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[2].Trigger();
			}
		}

		//Number Key 3
		if (lastKeyboardState.KeyChanged(GLFW_KEY_3))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_3))
			{
				targetSoundSystem->soundChannels[3].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[3].Trigger();
			}
		}

		//Number Key 4
		if (lastKeyboardState.KeyChanged(GLFW_KEY_4))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_4))
			{
				targetSoundSystem->soundChannels[4].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[4].Trigger();
			}
		}

		//Number Key 5
		if (lastKeyboardState.KeyChanged(GLFW_KEY_5))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_5))
			{
				targetSoundSystem->soundChannels[5].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[5].Trigger();
			}
		}

		//Number Key 6
		if (lastKeyboardState.KeyChanged(GLFW_KEY_6))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_6))
			{
				targetSoundSystem->soundChannels[6].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[6].Trigger();
			}
		}

		//Number Key 7
		if (lastKeyboardState.KeyChanged(GLFW_KEY_7))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_7))
			{
				targetSoundSystem->soundChannels[7].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[7].Trigger();
			}
		}

		//Number Key 8
		if (lastKeyboardState.KeyChanged(GLFW_KEY_8))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_8))
			{
				targetSoundSystem->soundChannels[8].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[8].Trigger();
			}
		}

		//Number Key 9
		if (lastKeyboardState.KeyChanged(GLFW_KEY_9))
		{
			if (lastKeyboardState.GetPreviousState(GLFW_KEY_9))
			{
				targetSoundSystem->soundChannels[9].Release();
			}
			else
			{
				targetSoundSystem->soundChannels[9].Trigger();
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
	bool patternCheck = false;
	bool animationCheck = false;
	bool inputCheck = true;

	//Check the arguments
	//If an argument got passed
	if (argc > 1)
	{
		//Check to see what it is
		
		//If "--PatternCheck" activate the patternCheck mode and disable the others.
		if (strncmp(argv[1], "--PatternCheck", 14) == 0)
		{
			inputCheck = false;
			patternCheck = true;
		}

		//If "--AnimationCheck" activate the animationCheck mode and disable the others.
		else if (strncmp(argv[1], "--AnimationCheck", 16) == 0)
		{
			inputCheck = false;
			animationCheck = true;
		}
	}



	//Start the Strange Game Engine
	SGE::System::Startup();

	//Open Game Window
	SGE::System::OpenGameWindow(640, 480, "Test Title");

	//Initialize Virtual Display
	SGE::System::InitializeVirtualDisplay(320, 240);


	//Launch the actual logic threads, before handling events.

	//Test pattern system thread
	std::thread testPatternThread(TestPattern, std::ref(patternCheck), SGE::System::mainDisplay->virtualVideoX, SGE::System::mainDisplay->virtualVideoY, SGE::System::mainDisplay->virtualVideoRAM);

	//Animation check thread
	std::thread testAnimationPatternThread(AnimationPattern, std::ref(animationCheck), SGE::System::mainDisplay);
	
	//Input Test thread
	std::thread testInputThread(InputTest, std::ref(inputCheck), SGE::System::mainDisplay, SGE::System::mainSoundSystem);


	
	//Start drawing
	SGE::System::StartDrawing();

	//Handle some events
	//Event handling has to be run from the main thread, hence why we are spawning the game logic off on it's own thread
	//And we are running the event handler on the main thread
	//Same OSes are weird about non-main threads handling keyboard/mouse input
	SGE::System::HandleEvents();
	
	//Interface handling over, window is closed stop the drawing loop if it already hasn't
	SGE::System::StopDrawing();


	//The program is pretty much done with the event handler quits, so start closing everything out.
	//Signal Test Pattern to Stop
	patternCheck = false;

	//Signal Animation Check to Stop
	animationCheck = false;

	//Signal Input Check to Stop
	inputCheck = false;
	
	//Wait for all the threads to join back
	testPatternThread.join();
	testAnimationPatternThread.join();
	testInputThread.join();

	//Shutdown the Strange Game Engine
	SGE::System::Shutdown();

	//If we got here... In theory everything went well...  In theory...
	return 0;
}
