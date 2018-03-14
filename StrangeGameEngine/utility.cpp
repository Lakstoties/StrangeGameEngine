//
//  Include the Utility header
//
#include "include\SGE\utility.h"

//
//  Include the Controls header
//
#include "include\SGE\controls.h"

//
//  Include the System header
//
#include "include\SGE\system.h"


//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace Utility
	{
		namespace Terminal
		{
			//
			//  ANSI compatible terminal methods
			//

			//
			//  Processing thread function
			//
			void TerminalWindow::processingThreadFunction()
			{
				//
				//  While we are still processing input to put into the text box
				//
				while (continueProcessing)
				{
					//
					//  Scan keyboard array for all active keys
					//
					for (int i = 0; i < SGE::Controls::Keyboard::NUMBER_OF_KEYS; i++)
					{
						//
						//  Send keypresses to terminal
						//
					}

					//
					//  Wait for the polling time
					//
					std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_POLLING_RATE_MILLISECONDS));
				}
			}

			void TerminalWindow::WriteToTerminal(char* inputBuffer, int bufferLength)
			{

			}

			//
			//  Constructor
			//
			TerminalWindow::TerminalWindow(int xPosition, int yPosition, unsigned int numberOfrows, unsigned int numberOfColumns)
			{
				terminalTextBox = new SGE::GUI::TextBox(numberOfrows, numberOfColumns, xPosition + BorderWidth, yPosition + BorderWidth);
			}

			//
			//  Destructor
			//
			TerminalWindow::~TerminalWindow()
			{
				delete terminalTextBox;
				StopPolling();
			}

			//
			//  Start polling input
			//
			void TerminalWindow::StartPolling()
			{
				//
				//  Set the flag to allow continued processing
				//
				continueProcessing = true;

				//
				//  Spawn off the processing thread
				//
				processingThread = std::thread(&TerminalWindow::processingThreadFunction, this);

			}

			//
			//  Stop polling input
			//
			void TerminalWindow::StopPolling()
			{
				//
				//  Signal stop
				//
				continueProcessing = false;

				//
				//  Wait for the thread to come back home and join us
				//
				if (processingThread.joinable())
				{
					processingThread.join();
				}
			}

			//
			//  Draw terminal window to screen
			//
			void TerminalWindow::Draw()
			{
				//
				//  Draw Borders
				//

				//
				//  Draw text box
				//
			}
		}




		namespace Math
		{
			//
			//  Precalculated Arrays for Common Math Functions
			//

			float SineDegree[PRECALCULATED_DEGREE_RESOLUTION];

			float CosineDegree[PRECALCULATED_DEGREE_RESOLUTION];

			void InitializeMath()
			{
				//Calculate radian to frequency step for a 1Hz full sine wave
				float periodStepping = TWO_PI_FLOAT / PRECALCULATED_DEGREE_RESOLUTION;

				//
				//  Precalculate the values for the arrays
				//

				for (int i = 0; i < PRECALCULATED_DEGREE_RESOLUTION; i++)
				{
					//
					//Generate Sine Values for Array
					//

					SineDegree[i] = sinf(periodStepping * i);

					//
					//Generate Cosine Value for Array
					//

					CosineDegree[i] = cosf(periodStepping * i);
				}
			}




			void RotatePointAroundPoint(int originalPointX, int originalPointY, int centerPointX, int centerPointY, int &rotatedPointX, int &rotatedPointY, float degrees)
			{
				//Scale degrees to precalculated array range
				int rotationIndex =	(int)(((degrees * PRECALCULATED_DEGREE_RESOLUTION) / 360) + PRECALCULATED_DEGREE_RESOLUTION) % PRECALCULATED_DEGREE_RESOLUTION;
					
								
				rotatedPointX = (int)(CosineDegree[rotationIndex] * (originalPointX - centerPointX) - SineDegree[rotationIndex] * (originalPointY - centerPointY) + centerPointX);
				rotatedPointY = (int)(SineDegree[rotationIndex] * (originalPointX - centerPointX) + CosineDegree[rotationIndex] * (originalPointY - centerPointY) + centerPointY);

			}
		}



		namespace Timer
		{

			void TimerDelta::Start(float rateOfChange)
			{
				//
				//  Save the rate
				//
				Rate = rateOfChange;

				//
				//  Capture the start time
				//
				StartTime = std::chrono::steady_clock::now();

				//
				//  Flag the timer has started
				//
				started = true;
			}

			float TimerDelta::Stop()
			{
				if (started)
				{
					//
					//  Capture the stop time
					//
					EndTime = std::chrono::steady_clock::now();

					//
					//  Flag the timer has stopped
					//
					started = false;

					//
					//  Return the delta
					//
					return Rate * std::chrono::duration<float>(EndTime - StartTime).count();
				}
				else
				{
					return 0.0f;
				}
			}
		}



		//
		//  Tracker Music Module components
		//
		namespace ModuleTrackerMusic
		{
			//
			//
			//  Module File Player
			//
			//

			float PeriodToOffsetIncrement(unsigned int period)
			{
				return  NTSC_TUNING / (float)(period * SGE::Sound::SAMPLE_RATE * 2);
			}

			unsigned int OffsetIncrementToPeriod(float offsetIncrement)
			{
				return (offsetIncrement * NTSC_TUNING) / (SGE::Sound::SAMPLE_RATE * 2);
			}

			float AmigaPeriodToSystemPeriod(float amigaPeriod)
			{
				return (float)((amigaPeriod * 2 * SGE::Sound::SAMPLE_RATE) / NTSC_TUNING);
			}

			float SystemPeriodToAmigaPeriod(float systemPeriod)
			{
				return (float)(systemPeriod * NTSC_TUNING) / (SGE::Sound::SAMPLE_RATE * 2);
			}


			bool ModulePlayer::Load(char * filename)
			{
				//Make sure we have a new module file
				modFile = SGE::FileFormats::ModuleFile();

				//Load it up.
				modFile.LoadFile(filename);

				//If we get here, stuff is okay.
				return true;
			}

			bool ModulePlayer::Connect(unsigned int startChannel, unsigned int startSample)
			{
				//Check to see if we have a valid start position when it comes to channel mapping
				if (startChannel > (SGE::Sound::MAX_CHANNELS - 4))
				{
					return false;
				}

				//Check to see if we have a valid start position when it comes to sample mapping
				if (startSample > (SGE::Sound::MAX_SAMPLE_BUFFERS - 31))
				{
					return false;
				}


				//Map up samples
				for (int i = 0; i < 31; i++)
				{
					sampleMap[i] = &SGE::Sound::SampleBuffers[i + startSample];
				}

				//Map up channels
				for (int i = 0; i < 4; i++)
				{
					channelMap[i] = &SGE::Sound::Channels[i + startChannel];
				}

				//Temporary pointer for converted sample data
				short * temp = nullptr;


				//Convert samples over, load them up
				for (int i = 0; i < 31; i++)
				{
					if (modFile.ConvertSampleSize(i) > 2)
					{
						//Allocate memory to the size we need.
						temp = new SGE::Sound::sampleType[modFile.ConvertSampleSize(i)];

						//Convert the sample data
						temp = modFile.ConvertSample(i);

						//Load the convert sample data into the sample buffer
						sampleMap[i]->Load(modFile.ConvertSampleSize(i), temp);

						//Get rid of the old buffer.
						delete temp;
					}

					//Apply repeat data
					sampleMap[i]->repeatOffset = modFile.samples[i].repeatOffset;

					//Because of mod file weirdness check to make sure the duration is more than 1.
					sampleMap[i]->repeatDuration = modFile.samples[i].repeatLength > 1 ? modFile.samples[i].repeatLength : 0;
				}



				//Since this a mod, configure the volume balances for the channels
				channelMap[0]->Pan = 0.50f;
				channelMap[1]->Pan = -0.50f;
				channelMap[2]->Pan = -0.50f;
				channelMap[3]->Pan = 0.50f;

				//If we get here, stuff is okay.
				return true;
			}

			bool ModulePlayer::Play()
			{
				//If the player thread is already active... don't pester it
				if (PlayerThreadActive)
				{
					return false;
				}

				//Set the player thread to be active
				PlayerThreadActive = true;

				//Launch the player thread
				playerThread = std::thread(&ModulePlayer::PlayThread, this);

				//If we get here, stuff is okay.
				return true;
			}

			bool ModulePlayer::Stop()
			{
				//Set the player thread to be inactive
				PlayerThreadActive = false;

				//Wait for the player thread to join
				if (playerThread.joinable())
				{
					playerThread.join();
				}

				//If we get here, stuff is okay.
				return true;
			}

			void ModulePlayer::PlayThread()
			{
				unsigned char effectTypeOnChannel[4] = { 0 };
				unsigned char effectXOnChannel[4] = { 0 };
				unsigned char effectYOnChannel[4] = { 0 };
				unsigned int postDivisionJumpTargetPosition = 0;
				unsigned int postDivisionJumpTargetDivision = 0;
				bool postDivisionJump = false;

				//
				bool channelPlays[4] = { false };


				//
				//  Default for most mods, can be changed.
				//
				ticksADivision = DEFAULT_TICKS_A_DIVISION;

				//
				//  Statistics information for tick rate
				//
				std::chrono::time_point<std::chrono::steady_clock> startTime;
				std::chrono::nanoseconds deltaTime = std::chrono::nanoseconds(0);

				//
				//  Dynamic Tick Rate Adjustment to factor in processing time.
				//

				std::chrono::nanoseconds deltaTimeAdjustment = std::chrono::nanoseconds(0);

				//
				//  Start playback processing
				//
				while (PlayerThreadActive)
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Starting to play: %s\n", modFile.title);
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Song Positions: %d\n", modFile.songPositions);

					for (CurrentPosition = 0; CurrentPosition < modFile.songPositions && PlayerThreadActive; CurrentPosition++)
					{
						//Pull the current pattern from the current position
						CurrentPattern = modFile.patternTable[CurrentPosition];

						//Process through the divisions
						for (CurrentDivision = 0; CurrentDivision < 64 && PlayerThreadActive; CurrentDivision++)
						{
							//Save the timer to help time the processing time for this division
							startTime = std::chrono::steady_clock::now();

							//
							//  Check for a post division jump
							//
							if (postDivisionJump)
							{
								//
								//  Set Position
								//
								CurrentPosition = postDivisionJumpTargetPosition;

								//
								//  Set Division
								//
								CurrentDivision = postDivisionJumpTargetDivision;

								//
								//  Set the Pattern
								//
								CurrentPattern = modFile.patternTable[CurrentPosition];

								//
								//  Reset Jump flag
								//
								postDivisionJump = false;
							}

							//
							//  Set up the channels
							//
							SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: %s - Pattern: %d - Division: %d - Previous Time: %lld\n", modFile.title, CurrentPosition, CurrentDivision, deltaTime.count());

							//
							//  Check all the channels for any changes
							//
							for (int c = 0; c < 4; c++)
							{
								//
								//  Check for sample changes on each channel
								//

								//
								//  Keep track to see if the channel needs to play again
								//
								channelPlays[c] = false;

								//Check to see if sample is not zero
								//If it is zero don't change the sample used in the channel
								if (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample > 0 &&
									modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample != CurrentChannelSamples[c])
								{
									//Otherwise, channel up the sample used, effectively reseting the channel to the sample settings.
									//Stop this channel
									channelMap[c]->Stop();

									//Switch to the sample
									channelMap[c]->currentSampleBuffer = sampleMap[modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample - 1];

									//Set sample volume
									channelMap[c]->Volume = float(modFile.samples[modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample - 1].volume) / 64.0f;

									//Indicate Current Channel's Sample
									CurrentChannelSamples[c] = modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample;

									channelPlays[c] = true;
								}

								//If the sample is the same, just reset the volume.
								if (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample == CurrentChannelSamples[c])
								{
									//Set sample volume
									channelMap[c]->Volume = float(modFile.samples[modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].sample - 1].volume) / 64.0f;

									if (!channelMap[c]->Playing)
									{
										channelPlays[c] = true;
									}
								}

								//
								//  Check for any effect chanages on the channel.
								//

								//Parse out the effect
								effectTypeOnChannel[c] = (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].effect & 0x0F00) >> 8;
								effectXOnChannel[c] = (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].effect & 0x00F0) >> 4;
								effectYOnChannel[c] = (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].effect & 0x000F);
								

								//
								//  Check for period changes on each channel
								//

								//
								//  Check to see if period not zero
								//  If it is zero don't change the period used in this channel
								//  NOTE:  Effect 3 does some weird stuff, the period is target note to shift to but it does NOT trigger a play effect

								if (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].period > 0)
								{
									//Save the period
									CurrentChannelPeriods[c] = modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].period;

									//
									//   Period Slides don't actually play the note, but use the period as a value for the effect
									//
									if (effectTypeOnChannel[c] != 0x3 && effectTypeOnChannel[c] != 0x5)
									{
										//If non-zero, change the period used
										//Changing periods, so stop the current stuff
										channelMap[c]->Stop();

										//Convert the period to offset timing interval in relation to system sampling rate
										//Using NTSC sampling
										channelMap[c]->offsetIncrement = PeriodToOffsetIncrement(modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].period);

										//Channel plays
										channelPlays[c] = true;
									}
								}

								//
								//Turn off any channel based rendering effects for now
								//If there's suppose to be an effect, it should be configured in the next heap of logic.
								//

								//Turn off Arpeggio
								channelMap[c]->arpeggioEnabled = false;

								//Turn off Volume Slide
								channelMap[c]->volumeSlideEnabled = false;

								//Turn off Vibrato
								channelMap[c]->vibratoEnabled = false;

								//Turn off Period Slide
								channelMap[c]->periodSlidEnabled = false;

								//Check to see if we need to retrigger vibrato
								if (channelMap[c]->vibratoRetriggers)
								{
									channelMap[c]->vibratoCurrentWaveformPosition = 0;
								}


								//
								//Is there an effect at all?
								//If the whole effect value is 0, then there is no effect
								//
								if (modFile.patterns[CurrentPattern].division[CurrentDivision].channels[c].effect != 0)
								{
									//Since the effect value is non-zero, there's some kind of effect data to be processed!
									//Let's find it!

									switch (effectTypeOnChannel[c])
									{
										//
										//  Configure Arpeggio or Effect 0 / 0x0
										//
									case 0x0:
										//  Set rate the arpeggio effect will change states
										channelMap[c]->arpeggioSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Set the semitones arpeggio will alternate between
										channelMap[c]->arpeggioSemitoneX = effectXOnChannel[c];
										channelMap[c]->arpeggioSemitoneY = effectYOnChannel[c];

										//  Reset the state variables for the effect
										channelMap[c]->arpeggioCurrentSamples = 0;
										channelMap[c]->arpeggioState = 0;

										//  Enable it and signal the sound system to start rendering it
										channelMap[c]->arpeggioEnabled = true;

										//  Effect found, move on.
										break;

										//
										//  Configure Slide Up or Effect 1 / 0x1
										//
									case 0x1:
										//  Set the target period
										channelMap[c]->periodTarget = 0;

										//  Set the number of samples for the period slide
										channelMap[c]->periodSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Calculate the total delta
										channelMap[c]->periodSlideDelta = -AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;

										//  Reset the state variables for the effect
										channelMap[c]->periodSlideCurrentSamples = 0;

										//  Enable the period slide
										channelMap[c]->periodSlidEnabled = true;

										//  Effect found, move on
										break;

										//
										//  Configure Slide Down or Effect 2 / 0x2
										//
									case 0x2:
										//  Set the target period
										channelMap[c]->periodTarget = 0;

										//  Set the number of samples for the period slide
										channelMap[c]->periodSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Calculate the delta
										channelMap[c]->periodSlideDelta = AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;

										//  Reset the state variables for the effect
										channelMap[c]->periodSlideCurrentSamples = 0;

										//  Enable the period slide
										channelMap[c]->periodSlidEnabled = true;

										//  Effect found, move on
										break;

										//
										//  Configure the Slide to Note of Effect 3 / 0x3
										//
									case 0x3:
										//  Set the target period
										channelMap[c]->periodTarget = AmigaPeriodToSystemPeriod(CurrentChannelPeriods[c]);

										//  Set the number of sampls for the period slide
										channelMap[c]->periodSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//
										//  Calculate the delta after determining direction of slide
										//

										//  If both X and Y are 0, then use previous slide stuff
										if (effectXOnChannel[c] != 0 || effectYOnChannel[c] != 0)
										{
											//Check to see which direction to move the period
											if (channelMap[c]->periodTarget > SystemPeriodToAmigaPeriod(1 / channelMap[c]->offsetIncrement))
											{
												channelMap[c]->periodSlideDelta = AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;
											}
											else
											{
												channelMap[c]->periodSlideDelta = -AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;
											}
										}

										//  Reset the state variables for the effect
										channelMap[c]->periodSlideCurrentSamples = 0;

										//  Enable the period slide
										channelMap[c]->periodSlidEnabled = true;

										//  Found the effect, move on.
										break;


										//
										//  Configure the Vibrato Effect or Effect 4 / 0x4
										//
									case 0x4:
										//  Set the Amplitude for the frequency shift y/16 semitones.
										//  If 0, use previous settings
										if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->vibratoAmplitude = effectYOnChannel[c] / 16.0f;
										}

										//  Set the cycle rate for the Vibrato so that (X * Ticks) / 64 cyckes occur in the division
										//  If 0, use previous settings
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->vibratoCycles = (effectXOnChannel[c] * ticksADivision) / 64.0f;
										}

										//  Enable Vibrato
										channelMap[c]->vibratoEnabled = true;

										//  Effect found, move on.
										break;

										//
										//  Configure - Continue Slide to note, but do Volume Slide
										//
									case 0x5:

										//
										//  Update the Slide to Note
										//
									
										//  Set the target period
										channelMap[c]->periodTarget = AmigaPeriodToSystemPeriod(CurrentChannelPeriods[c]);

										//  Set the number of sampls for the period slide
										channelMap[c]->periodSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//
										//  Calculate the delta
										//

										//  If both X and Y are 0, then use previous slide stuff
										if (effectXOnChannel[c] != 0 || effectYOnChannel[c] != 0)
										{
											//Check to see which direction to move the period

											if (channelMap[c]->periodTarget > SystemPeriodToAmigaPeriod(1 / channelMap[c]->offsetIncrement))
											{
												channelMap[c]->periodSlideDelta = AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;
											}
											else
											{
												channelMap[c]->periodSlideDelta = -AmigaPeriodToSystemPeriod(effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 2.0f;
											}
										}

										//
										//  Configure the volume slide
										//

										//  Set the number of samples that progress for each tick in the effect.
										channelMap[c]->volumeSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Check to see the rate we have to slide the volume up
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = effectXOnChannel[c] / 64.0f;
										}

										//  Check to see the rate we have to slide the volume down
										//  Y is only paid attention if X is zero and is therefor assumed to be zero
										else if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = -effectYOnChannel[c] / 64.0f;
										}

										//  Continue the period slide
										channelMap[c]->periodSlidEnabled = true;

										//  Enable Volume Slide
										channelMap[c]->volumeSlideEnabled = true;

										//  Effect found, move on.
										break;

										//
										//  Configure - Continue Vibrato, but do Volume Slide
										//
									case 0x06:
										//  Set the number of samples that progress for each tick in the effect.
										channelMap[c]->volumeSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Check to see the rate we have to slide the volume up
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = effectXOnChannel[c] / 64.0f;
										}

										//  Check to see the rate we have to slide the volume down
										//  Y is only paid attention if X is zero
										else if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = -effectYOnChannel[c] / 64.0f;
										}

										//  Continue Vibrato
										channelMap[c]->vibratoEnabled = true;

										//  Enable Volume Slide
										channelMap[c]->volumeSlideEnabled = true;

										//  Effect found, move on.
										break;

										//
										//  Configure Volume Slide or Effect 10 / 0xA
										//
									case 0xA:
										//  Set the number of samples that progress for each tick in the effect.
										channelMap[c]->volumeSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//  Check to see the rate we have to slide the volume up
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = effectXOnChannel[c] / 64.0f;
										}

										//  Check to see the rate we have to slide the volume down
										//  Y is only paid attention if X is zero and is therefor assumed to be zero
										else if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->volumeSlideRate = -effectYOnChannel[c] / 64.0f;
										}

										//  Enable Volume Slide
										channelMap[c]->volumeSlideEnabled = true;

										//  Effect found, move on.
										break;

										//
										//  Configure for Jump position after this division or Effect 11 / 0xB
										//
									case 0xB:
										//  Flag a jump after this division
										postDivisionJump = true;

										//  Set target position
										postDivisionJumpTargetPosition = (effectXOnChannel[c] * 16 + effectYOnChannel[c]);

										//  Set target division
										postDivisionJumpTargetDivision = 0;

										//  Effect found, move on.
										break;

										//
										//  Configure the Volume or Effect 12 / 0xC
										//
									case 0xC:
										//  Set the volume for the channel
										channelMap[c]->Volume = (effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 64.0f;

										//  If for some reason the channel volume is higher than 1.0f, set it back down.
										if (channelMap[c]->Volume > 1.0f)
										{
											channelMap[c]->Volume = 1.0f;
										}

										//  Effect found, move on.
										break;

										//
										//  Configure pattern break or Effect 13 / 0xD
										//
									case 0xD:
										//  Flag a jump after this division
										postDivisionJump = true;

										//  Set target position
										postDivisionJumpTargetPosition = CurrentPosition + 1;

										//  Set target division
										postDivisionJumpTargetDivision = (effectXOnChannel[c] * 10 + effectYOnChannel[c]);

										//  Effect found, move on.
										break;

										//
										//  Configure an effect under this category.  There's a few of them shove under Effect 14 or 0xE
										//
									case 0xE:
										//  Check the Effect's X setting to see what exact effect they want
										switch (effectXOnChannel[c])
										{
										//  Set the sound filter on/off
										case 0x0:
											//Not implemented.
											break;

										//  Set Vibrato Effect Waveform
										case 0x4:
											//  Set the waveform
											//  If Y is 0 or 4, set it to a sine waveform
											switch (effectYOnChannel[c])
											{
											//  If Y is 0, Sine waveform with Retrigger.
											case 0x0:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::Sine;
												channelMap[c]->vibratoRetriggers = true;
												break;

											//  If Y is 1, Ramp Down waveform with Retrigger.
											case 0x1:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::RampDown;
												channelMap[c]->vibratoRetriggers = true;
												break;

											//  If Y is 2, Square waveform with Retrigger.
											case 0x2:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::Square;
												channelMap[c]->vibratoRetriggers = true;
												break;

											//  If Y is 3, Random choice with Retrigger
											case 0x3:
												//  Not implemented
												channelMap[c]->vibratoRetriggers = true;
												break;

											//  If Y is 4, Sine waveform without Retrigger
											case 0x4:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::Sine;
												channelMap[c]->vibratoRetriggers = false;
												break;

											//  If Y is 5, Ramp Down waveform without Retrigger
											case 0x5:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::RampDown;
												channelMap[c]->vibratoRetriggers = false;
												break;

											//  If Y is 6, Square waveform without Retrigger
											case 0x6:
												channelMap[c]->vibratoWaveform = SGE::Sound::Waveforms::Square;
												channelMap[c]->vibratoRetriggers = false;
												break;

											//  If Y is 7, random choice without Retrigger
											case 0x7:
												//  Not implemented
												channelMap[c]->vibratoRetriggers = false;
												break;
											}

											//  Effect found, move on.
											break;

											//
											//  Not implemented effect
											//
										default:
											SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);

											break;
										}

										//  Found our effect.  Moving on!
										break;

										//
										//  Configure Ticks Per Division or Effect 15 / 0xF
										//
									case 0xF:
										//  Set change the ticks per division
										ticksADivision = effectXOnChannel[c] * 16 + effectYOnChannel[c];

										//  Effect found, move on.
										break;

										//
										//  If we are here, then we have found either an Unimplented or Unknown effect, Error Log it!
										//
									default:
										SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);
										break;
									}
								}
							}

							//
							//  Playback trigger point
							//  Positioned here to make sure all the settings are good across all the channels before triggering all of them.
							//  Otherwise, there's a slight chance for some very minor desychronization to occur.
							//

							//  Go through all the channels
							for (int c = 0; c < 4; c++)
							{
								//  If there was a sample or period mentioned, play it if it is not equal to 0
								if (channelPlays[c])
								{
									//  Play it
									channelMap[c]->Play();
								}
							}

							//
							//  Wait for the next division
							//
							std::this_thread::sleep_for(std::chrono::nanoseconds(ticksADivision * DEFAULT_TICK_TIMING_NANO) + deltaTimeAdjustment);

							//
							//  Calculate the delta time
							//
							deltaTime = std::chrono::steady_clock::now() - startTime;

							//
							//  Tweak adjustment time delta to tune it closer to ideal
							//
							deltaTimeAdjustment += (std::chrono::nanoseconds(ticksADivision * DEFAULT_TICK_TIMING_NANO) - deltaTime) / 10;

						}
					}
				}

				//
				//  Post Playback processing
				//

				//Stop all channels
				for (int c = 0; c < 4; c++)
				{
					channelMap[c]->Stop();
				}
			}


			//Constructor Stuff
			ModulePlayer::ModulePlayer()
			{

			}

			ModulePlayer::~ModulePlayer()
			{
				//Stop the player
				this->Stop();
			}
		}
	}
}