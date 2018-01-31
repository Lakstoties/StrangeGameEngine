//
//  Include the Utility header
//
#include "include\SGE\utility.h"

//
//  Include the System header
//
#include "include\SGE\system.h"


namespace SGE
{
	namespace Utility
	{
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
				int positionToJumpAfterDivision = -1;

				//
				bool channelPlays[4] = { false };


				//Default for most mods, can be changed.
				ticksADivision = DEFAULT_TICKS_A_DIVISION;

				std::chrono::time_point<std::chrono::steady_clock> startTime;
				std::chrono::nanoseconds deltaTime = std::chrono::nanoseconds(0);

				//
				//  Start playback processing
				//
				while (PlayerThreadActive)
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Starting to play: %s\n", modFile.title);
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Song Positions: %d\n", modFile.songPositions);

					for (int j = 0; j < modFile.songPositions && PlayerThreadActive; j++)
					{
						CurrentPosition = j;

						//Pull the current pattern from the current position
						CurrentPattern = modFile.patternTable[j];

						//Process through the divisions
						for (int i = 0; i < 64 && PlayerThreadActive; i++)
						{
							//Save the timer to help time the processing time for this division
							startTime = std::chrono::steady_clock::now();

							//Set the current division
							CurrentDivision = i;

							//Set up the channels
							SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: %s - Pattern: %d - Division: %d - Previous Time: %lld\n", modFile.title, j, i, deltaTime.count());

							//
							//  Check all the channels for any changes
							//
							for (int c = 0; c < 4; c++)
							{
								//
								//  Check for sample changes on each channel
								//

								//Keep track to see if the channel needs to play again
								channelPlays[c] = false;

								//Check to see if sample is not zero
								//If it is zero don't change the sample used in the channel
								if (modFile.patterns[CurrentPattern].division[i].channels[c].sample > 0 &&
									modFile.patterns[CurrentPattern].division[i].channels[c].sample != CurrentChannelSamples[c])
								{
									//Otherwise, channel up the sample used, effectively reseting the channel to the sample settings.
									//Stop this channel
									channelMap[c]->Stop();

									//Switch to the sample
									channelMap[c]->currentSampleBuffer = sampleMap[modFile.patterns[CurrentPattern].division[i].channels[c].sample - 1];

									//Set sample volume
									channelMap[c]->Volume = float(modFile.samples[modFile.patterns[CurrentPattern].division[i].channels[c].sample - 1].volume) / 64.0f;

									//Indicate Current Channel's Sample
									CurrentChannelSamples[c] = modFile.patterns[CurrentPattern].division[i].channels[c].sample;

									channelPlays[c] = true;
								}

								//If the sample is the same, just reset the volume.
								if (modFile.patterns[CurrentPattern].division[i].channels[c].sample == CurrentChannelSamples[c])
								{
									//Set sample volume
									channelMap[c]->Volume = float(modFile.samples[modFile.patterns[CurrentPattern].division[i].channels[c].sample - 1].volume) / 64.0f;

									if (!channelMap[c]->Playing)
									{
										channelPlays[c] = true;
									}
								}


								//
								//  Check for period changes on each channel
								//

								//Check to see if period not zero
								//If it is zero don't change the period used in this channel
								if (modFile.patterns[CurrentPattern].division[i].channels[c].period > 0)
								{
									//If non-zero, change the period used
									//Changing periods, so stop the current stuff
									channelMap[c]->Stop();

									//Convert the period to offset timing interval in relation to system sampling rate
									//Using NTSC sampling
									channelMap[c]->offsetIncrement = NTSC_TUNING /
										(float)(modFile.patterns[CurrentPattern].division[i].channels[c].period * SGE::Sound::SAMPLE_RATE * 2);

									//Channel plays
									channelPlays[c] = true;
								}


								//
								//  Check for any effect chanages on the channel.
								//

								//Parse out the effect
								effectTypeOnChannel[c] = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x0F00) >> 8;
								effectXOnChannel[c] = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x00F0) >> 4;
								effectYOnChannel[c] = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x000F);

								//
								//Turn off any channel based rendering effects for now
								//If there's suppose to be an effect, it should be configured in the next heap of logic.
								//

								//Turn off Arpeggio
								channelMap[c]->EnableArpeggio = false;

								//Turn off Volume Slide
								channelMap[c]->EnableVolumeSlide = false;

								//Turn off Vibrato
								channelMap[c]->EnableVibrato = false;

								//Check to see if we need to retrigger vibrato
								if (channelMap[c]->RetriggerVibrato)
								{
									channelMap[c]->currentVibratoWaveformPosition = 0;
								}


								//Is there an effect at all?
								//If the whole effect value is 0, then there is no effect
								if (modFile.patterns[CurrentPattern].division[i].channels[c].effect != 0)
								{
									//Since the effect value is non-zero, there's some kind of effect data to be processed!
									//Let's find it!

									switch (effectTypeOnChannel[c])
									{
										//Configure Arpeggio or Effect 0 / 0x0
									case 0x0:
										//Set rate the arpeggio effect will change states
										channelMap[c]->ArpeggioSampleInterval = DEFAULT_SAMPLES_TICK;

										//Set the semitones arpeggio will alternate between
										channelMap[c]->arpeggioSemitoneX = effectXOnChannel[c];
										channelMap[c]->arpeggioSemitoneY = effectYOnChannel[c];

										//Reset the state variables for the effect
										channelMap[c]->currentArpeggioSamples = 0;
										channelMap[c]->arpeggioState = 0;

										//Enable it and signal the sound system to start rendering it
										channelMap[c]->EnableArpeggio = true;

										//Found our effect.  Moving on!
										break;

										//Configure the Vibrato Effect or Effect 4 / 0x4
									case 0x4:
										//Set the Amplitude for the frequency shift y/16 semitones.
										//If 0, use previous settings
										if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->VibratoAmplitude = effectYOnChannel[c] / 16.0f;
										}

										//Set the cycle rate for the Vibrato so that (X * Ticks) / 64 cyckes occur in the division
										//If 0, use previous settings
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->VibratoCycles = (effectXOnChannel[c] * ticksADivision) / 64.0f;
										}

										//Enable Vibrato
										channelMap[c]->EnableVibrato = true;

										//Found our effect.  Moving on!
										break;

										//Configure Volume Slide or Effect 10 / 0xA
									case 0xA:
										//Set the number of samples that progress for each tick in the effect.
										channelMap[c]->VolumeSlideSampleInterval = DEFAULT_SAMPLES_TICK;

										//Check to see the rate we have to slide the volume up
										if (effectXOnChannel[c] != 0)
										{
											channelMap[c]->VolumeSlideRate = effectXOnChannel[c] / 64.0f;
										}

										//Check to see the rate we have to slide the volume down
										//Y is only paid attention if X is zero and is therefor assumed to be zero
										else if (effectYOnChannel[c] != 0)
										{
											channelMap[c]->VolumeSlideRate = -effectYOnChannel[c] / 64.0f;
										}

										//Enable Volume Slide
										channelMap[c]->EnableVolumeSlide = true;

										//Found our effect.  Moving on!
										break;

										//Configure for Jump position after this division or Effect 11 / 0xB
									case 0xB:
										//Set the Jump after this division
										positionToJumpAfterDivision = (effectXOnChannel[c] * 16 + effectYOnChannel[c]);

										//Found our effect.  Moving on!
										break;

										//Configure the Volume or Effect 12 / 0xC
									case 0xC:
										//Set the volume for the channel
										channelMap[c]->Volume = (effectXOnChannel[c] * 16 + effectYOnChannel[c]) / 64.0f;

										//Found our effect.  Moving on!
										break;

										//Configure an effect under this category.  There's a few of them shove under Effect 14 or 0xE
									case 0xE:
										//Check the Effect's X setting to see what exact effect they want
										switch (effectXOnChannel[c])
										{
											//Set the sound filter on/off
										case 0x0:
											//Not implemented.
											break;

											//Set Vibrato Effect Waveform
										case 0x4:
											//Set the waveform
											//If Y is 0 or 4, set it to a sine waveform
											switch (effectYOnChannel[c])
											{
												//If Y is 0, Sine waveform with Retrigger.
											case 0x0:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::Sine;
												channelMap[c]->RetriggerVibrato = true;
												break;

												//If Y is 1, Ramp Down waveform with Retrigger.
											case 0x1:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::RampDown;
												channelMap[c]->RetriggerVibrato = true;
												break;

												//If Y is 2, Square waveform with Retrigger.
											case 0x2:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::Square;
												channelMap[c]->RetriggerVibrato = true;
												break;

												//If Y is 3, Random choice with Retrigger
											case 0x3:

												channelMap[c]->RetriggerVibrato = true;
												break;

												//If Y is 4, Sine waveform without Retrigger
											case 0x4:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::Sine;
												channelMap[c]->RetriggerVibrato = false;
												break;

												//If Y is 5, Ramp Down waveform without Retrigger
											case 0x5:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::RampDown;
												channelMap[c]->RetriggerVibrato = false;
												break;

												//If Y is 6, Square waveform without Retrigger
											case 0x6:
												channelMap[c]->VibratoWaveform = SGE::Sound::Waveforms::Square;
												channelMap[c]->RetriggerVibrato = false;
												break;

												//If Y is 7, random choice without Retrigger
											case 0x7:

												channelMap[c]->RetriggerVibrato = false;
												break;
											}

											//Found the effect, move on!
											break;

											//Not implemented effect
										default:
											SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);

											break;
										}

										//Found our effect.  Moving on!
										break;

										//Configure Ticks Per Division or Effect 15 / 0xF
									case 0xF:
										//Set change the ticks per division
										ticksADivision = effectXOnChannel[c] * 16 + effectYOnChannel[c];

										//Found our effect.  Moving on!
										break;

										//If we are here, then we have found either an Unimplented or Unknown effect, Error Log it!
									default:
										SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);
										break;
									}
								}
							}

							//
							//  Playback trigger point
							//Positioned here to make sure all the settings are good across all the channels before triggering all of them.
							//Otherwise, there's a slight chance for some very minor desychronization to occur.
							//

							//Go through all the channels
							for (int c = 0; c < 4; c++)
							{
								//If there was a sample or period mentioned, play it if it is not equal to 0
								if (channelPlays[c])
								{
									//Play it
									channelMap[c]->Play();
								}
							}

							//Wait for the next division
							std::this_thread::sleep_for(std::chrono::nanoseconds(ticksADivision * DEFAULT_TICK_TIMING_NANO));

							//Post division checks

							//Check for a jump
							if (positionToJumpAfterDivision != -1)
							{
								//Set Position
								j = positionToJumpAfterDivision;

								//Reset Division
								i = 0;

								//Reset Jump flag
								positionToJumpAfterDivision = -1;
							}

							//Calculate the delta time
							deltaTime = std::chrono::steady_clock::now() - startTime;
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