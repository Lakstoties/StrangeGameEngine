#include "main.h"
#include <thread>


void InputTest(bool& testInputRunning)
{
	char* menuItemText[5] =
	{
		"Play: Hyper.mod",
		"Play: Yehat.mod",
		"Stop: Hyper.mod",
		"Stop: Yehat.mod",
		"Exit Demo",		
	};

	SGE::Menu testMenu(0, 175, 126, 64, 2, 12, 2, 5, menuItemText);

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

	//Load a module file
	SGE::Sound::ModuleFile testModule;
	testModule.LoadFile("hyper.mod");
	
	//Create buffers for converted module data
	short* moduleSample0 = testModule.ConvertSample(0);
	short* moduleSample1 = testModule.ConvertSample(1);
	short* moduleSample2 = testModule.ConvertSample(2);
	short* moduleSample3 = testModule.ConvertSample(3);
	short* moduleSample4 = testModule.ConvertSample(4);
	short* moduleSample5 = testModule.ConvertSample(5);
	short* moduleSample6 = testModule.ConvertSample(6);
	short* moduleSample7 = testModule.ConvertSample(7);
	short* moduleSample8 = testModule.ConvertSample(8);
	short* moduleSample9 = testModule.ConvertSample(9);

	//Load samples into the Sound System
	SGE::Sound::SampleBuffers[0].Load(testModule.samples[0].lengthInWords * 2, moduleSample0);
	SGE::Sound::SampleBuffers[1].Load(testModule.samples[1].lengthInWords * 2, moduleSample1);
	SGE::Sound::SampleBuffers[2].Load(testModule.samples[2].lengthInWords * 2, moduleSample2);
	SGE::Sound::SampleBuffers[3].Load(testModule.samples[3].lengthInWords * 2, moduleSample3);
	SGE::Sound::SampleBuffers[4].Load(testModule.samples[4].lengthInWords * 2, moduleSample4);
	SGE::Sound::SampleBuffers[5].Load(testModule.samples[5].lengthInWords * 2, moduleSample5);
	SGE::Sound::SampleBuffers[6].Load(testModule.samples[6].lengthInWords * 2, moduleSample6);
	SGE::Sound::SampleBuffers[7].Load(testModule.samples[7].lengthInWords * 2, moduleSample7);
	SGE::Sound::SampleBuffers[8].Load(testModule.samples[8].lengthInWords * 2, moduleSample8);
	SGE::Sound::SampleBuffers[9].Load(testModule.samples[9].lengthInWords * 2, moduleSample9);
	
	//Ready the channels
	for (int i = 0; i < 10; i++)
	{
		//Load SineWave in channel
		SGE::Sound::Channels[i].currentSampleBuffer = &SGE::Sound::SampleBuffers[i];

		//Set the pitches of the channel
		//SGE::Sound::Channels[i].offsetIncrement = (SGE::Sound::MIDI_NOTE_FRENQUENCY[i + 60]);
		SGE::Sound::Channels[i].offsetIncrement = 0.1879138321995465f;

		//Set the channel to loop
		SGE::Sound::Channels[i].Repeatable = false;

		//Set repeat duration
		SGE::Sound::Channels[i].repeatDuration = SGE::Sound::Channels[i].currentSampleBuffer->bufferSize;
	}

	//Create some players
	SGE::Sound::ModulePlayer modulePlayerTest;
	SGE::Sound::ModulePlayer modulePlayerTest2;
	
	//Load up the module files
	modulePlayerTest.Load("hyper.mod");
	modulePlayerTest2.Load("yehat.mod");

	//Connect up to the sound system
	modulePlayerTest.Connect(12, 64);
	modulePlayerTest2.Connect(24, 128);

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

		//Check to see if Enter is pressed and where the menu cursor is
		if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_ENTER])
		{
			//If the Enter key wasn't pressed before
			if (!lastKeyboardState[SGE::Controls::Keymap::KEY_ENTER])
			{
				//Check to see where the cursor is and perform the action
				
				//If Play: hyper.mod - Selection 0
				if (testMenu.selection == 0)
				{
					modulePlayerTest.Play();
				}

				//If Play: yehat.mod - Selection 1
				if (testMenu.selection == 1)
				{
					modulePlayerTest2.Play();
				}

				//If Stop: hyper.omd - Selection 2
				if (testMenu.selection == 2)
				{
					modulePlayerTest.Stop();
				}

				//If Stop: yehat.mod - Selection 3
				if (testMenu.selection == 3)
				{
					modulePlayerTest2.Stop();
				}

				//IF Exit Demo - Selection 4
				if (testMenu.selection == 4)
				{
					//This thread is done
					testInputRunning = false;

					//Tell the event handler we are done, too.
					SGE::Controls::ContinueToHandleEvents = false;
				}
			}
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


		//Sound keys stuff
		//Number Key 0
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_0] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_0])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_0])
			{
				SGE::Sound::Channels[0].Play();
			}
		}

		//Number Key 1
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_1] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_1])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_1])
			{
				SGE::Sound::Channels[1].Play();
			}
		}

		//Number Key 2
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_2] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_2])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_2])
			{
				SGE::Sound::Channels[2].Play();
			}
		}

		//Number Key 3
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_3] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_3])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_3])
			{
				SGE::Sound::Channels[3].Play();
			}
		}

		//Number Key 4
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_4] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_4])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_4])
			{

				SGE::Sound::Channels[4].Play();
			}
		}

		//Number Key 5
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_5] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_5])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_5])
			{
				SGE::Sound::Channels[5].Play();
			}
		}

		//Number Key 6
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_6] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_6])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_6])
			{
				SGE::Sound::Channels[6].Play();
			}
		}

		//Number Key 7
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_7] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_7])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_7])
			{
				SGE::Sound::Channels[7].Play();
			}
		}

		//Number Key 8
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_8] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_8])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_8])
			{
				SGE::Sound::Channels[8].Play();
			}
		}

		//Number Key 9
		if (lastKeyboardState[SGE::Controls::Keymap::KEY_9] != SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_9])
		{
			if (SGE::Controls::KeyboardStatus[SGE::Controls::Keymap::KEY_9])
			{
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
