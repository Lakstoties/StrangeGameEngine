//
//  Include the Utility header
//
#include "include\SGE\utility.h"

//
//  Include the Inputs header
//
#include "include\SGE\inputs.h"

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
		//  Namespace for various timing elements
		//
		namespace Timer
		{

			//
			//  Crude and rough timer system.  Not really recommended to use for everything due to burning some serious CPU cycles when system clock interrupt intervals shift outside multiples of the desired wait period.
			//
			void AccurateWaitForMilliseconds(int milliseconds, bool superAccurate)
			{
				const int ATTEMPTED_MILLISECOND_QUANTUM = 10;

				//
				//  Figure out the destinated target time to sleep to
				//
				std::clock_t whenToContinue = std::clock() + (CLOCKS_PER_SEC * milliseconds) / 1000;

				#ifdef _WIN32
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds - SGE::System::OS::TARGET_OS_TIMER_RESOLUTION_MILLISECONDS));
				#else				

				//  Test the sleep waters
				std::clock_t timeSleepStart;
				std::clock_t timeSlept = 0;

				while ((std::clock() + timeSlept) < whenToContinue)
				{
					timeSleepStart = std::clock();

					//  Check the thread sleep quantum
					std::this_thread::sleep_for(std::chrono::milliseconds(ATTEMPTED_MILLISECOND_QUANTUM));

					//  How long did we actually sleep
					timeSlept = std::clock() - timeSleepStart;
				}
				#endif	

				//
				//  Figure out if we are short of the goal and busy wait to it
				//
				while (clock() < whenToContinue && superAccurate)
				{
					std::this_thread::yield();
				}
			}



			void TimerDelta::Start(float rateOfChange)
			{
				//
				//  Save the rate
				//
				Rate = rateOfChange;

				//
				//  Capture the start time
				//
				StartTime = std::clock();

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
					EndTime = std::clock();

					//
					//  Flag the timer has stopped
					//
					started = false;

					//
					//  Return the delta
					//
					return Rate * (EndTime - StartTime) / (float) CLOCKS_PER_SEC;
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
				return (unsigned int) (NTSC_TUNING / (offsetIncrement * SGE::Sound::SAMPLE_RATE * 2));
			}


			void ModulePlayer::Effect::Parse(unsigned short rawEffectData)
			{
				Type = (rawEffectData & 0x0F00) >> 8;
				X = (rawEffectData & 0x00F0) >> 4;
				Y = (rawEffectData & 0x000F);
			}


			void ModulePlayer::ModuleLocation::NextDivision()
			{
				if (!EndOfModule)
				// Increment to the next Division
				Division++;

				//  Check to see if we've reached the end
				if (Division >= 64)
				{
					// Next Position, reset the Divison
					Division = 0;

					if ((Position + 1) >= NumberOfPositions)
					{
						EndOfModule = true;
					}
					else
					{
						// Increment to the next Position
						Position++;
					}
				}
			}

			void ModulePlayer::ModuleLocation::SetDivision(unsigned char division)
			{
				Division = division;
			}

			void ModulePlayer::ModuleLocation::SetPosition(unsigned char position)
			{
				Position = position;
				Division = 0;
			}

			void ModulePlayer::ModuleLocation::Reset()
			{
				Position = 0;
				Division = 0;
				EndOfModule = false;
			}


			bool ModulePlayer::Load(char * filename)
			{
				//Load it up.
				modFile.LoadFile(filename);

				//If we get here, stuff is okay.
				return modFile.ProperlyLoaded;
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
					sampleMap[i] = i + startSample;
				}

				//Map up channels
				for (int i = 0; i < 4; i++)
				{
					channelMap[i] = i + startChannel;
				}

				//Temporary pointer for converted sample data
				short * temp = nullptr;


				//Convert samples over, load them up
				for (int i = 0; i < 31; i++)
				{
					if (modFile.samples[i].SampleLengthInBytes() > 2)
					{
						//Allocate memory to the size we need.
						temp = new SGE::Sound::sampleType[modFile.samples[i].SampleLengthInBytes()];

						//Convert the sample data
						modFile.samples[i].ConvertSampleTo16Bit(temp);

						//Load the convert sample data into the sample buffer
						SGE::Sound::SampleBuffers[sampleMap[i]].LoadRaw(modFile.samples[i].SampleLengthInBytes(), temp);

						//Get rid of the old buffer.
						delete[] temp;
					}

					//Apply repeat data
					SGE::Sound::SampleBuffers[sampleMap[i]].RepeatOffset = modFile.samples[i].repeatOffset;

					//Because of mod file weirdness check to make sure the duration is more than 1.
					SGE::Sound::SampleBuffers[sampleMap[i]].RepeatDuration = modFile.samples[i].repeatLength > 1 ? modFile.samples[i].repeatLength : 0;
				}



				//Since this a mod, configure the volume balances for the channels
				SGE::Sound::Channels[channelMap[0]].Pan = 0.50f;
				SGE::Sound::Channels[channelMap[1]].Pan = -0.50f;
				SGE::Sound::Channels[channelMap[2]].Pan = -0.50f;
				SGE::Sound::Channels[channelMap[3]].Pan = 0.50f;

				//If we get here, stuff is okay.
				return true;
			}

			bool ModulePlayer::Play()
			{
				//  Do we even have a file to play?
				if (!modFile.ProperlyLoaded)
				{
					return false;
				}

				//If the player thread is already active... don't pester it
				if (PlayerThreadActive)
				{
					return false;
				}

				//Set the player thread to be active
				PlayerThreadActive = true;

				//Launch the player thread
				if (playerThread == NULL)
				{
					playerThread = new std::thread(&ModulePlayer::PlayThread, this);
				}

				//If we get here, stuff is okay.
				return true;
			}

			bool ModulePlayer::Stop()
			{
				//
				//  Check to see if the player thread has already been terminated.
				//
				if (playerThread == NULL)
				{
					//
					//  Thread already taken care of, nothing should be player.
					//
					return true;
				}


				//Set the player thread to be inactive
				PlayerThreadActive = false;

				//Wait for the player thread to join
				if (playerThread->joinable())
				{
					playerThread->join();
				}

				//
				//  Delete the old thread
				//

				delete playerThread;

				playerThread = NULL;

				//If we get here, stuff is okay.
				return true;
			}

			void ModulePlayer::PlayThread()
			{
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
				std::time_t startTime;
				std::time_t deltaTime = 0;


				//
				//  Start playback processing
				//
				while (PlayerThreadActive)
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Starting to play: %s\n", modFile.title);
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: Song Positions: %d\n", modFile.songPositions);

					//
					//  Load up the Location data
					//
					Location.Reset();
					Location.NumberOfPositions = modFile.songPositions;

					//
					//  While we are not at the end of the song.
					//

					while (!Location.EndOfModule && PlayerThreadActive)
					{
						//Save the timer to help time the processing time for this division
						startTime = std::clock();

						//
						//  Set up the channels
						//
						SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Utility, "Mod Player: %s - Pattern: %d - Division: %d - Previous Time: %lld\n", modFile.title, Location.Position, Location.Division, deltaTime);

						//
						//  Check all the channels for any changes
						//
						for (int CurrentChannel = 0; CurrentChannel < 4; CurrentChannel++)
						{
							//
							//  Keep track to see if the channel needs to play again
							//
							channelPlays[CurrentChannel] = false;


							//
							//  Check for sample changes on each channel
							//

							//
							//  If the new sample is 0, then use the previous sample
							//  Hence, don't change it.
							//
							if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample == 0)
							{

							}

							//
							//  If the new sample is the same as the last
							//  Set the volume, and play it if it already isn't
							//
							else if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample == Channel[CurrentChannel].Sample)
							{
								//Set sample volume
								SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = float(modFile.samples[Channel[CurrentChannel].Sample - 1].volume) / 64.0f;

								if (!SGE::Sound::Channels[channelMap[CurrentChannel]].Playing)
								{
									channelPlays[CurrentChannel] = true;
								}
							}

							//
							//  Else this is a new sample, let's play it like a new one
							//
							else
							{
								// Set the sample
								Channel[CurrentChannel].Sample = modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].sample;

								//Otherwise, channel up the sample used, effectively reseting the channel to the sample settings.
								//Stop this channel
								SGE::Sound::Channels[channelMap[CurrentChannel]].Stop();

								//Switch to the sample
								SGE::Sound::Channels[channelMap[CurrentChannel]].SetSampleBuffer(sampleMap[Channel[CurrentChannel].Sample - 1]);

								//Set sample volume
								SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = float(modFile.samples[Channel[CurrentChannel].Sample - 1].volume) / 64.0f;

								channelPlays[CurrentChannel] = true;
							}


							//
							//  Check for any effect chanages on the channel.
							//

							//Parse out the effect
							Channel[CurrentChannel].Effect.Parse(modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].effect);


							//
							//  Check for period changes on each channel
							//

							//
							//  Check to see if period not zero
							//  If it is zero don't change the period used in this channel
							//  NOTE:  Effect 3 does some weird stuff, the period is target note to shift to but it does NOT trigger a play effect

							if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].period > 0)
							{
								//Set the period
								Channel[CurrentChannel].Period = modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].period;

								//
								//   Period Slides don't actually play the note, but use the period as a value for the effect
								//
								if (Channel[CurrentChannel].Effect.Type != 0x3 && Channel[CurrentChannel].Effect.Type != 0x5)
								{
									//If non-zero, change the period used
									//Changing periods, so stop the current stuff
									SGE::Sound::Channels[channelMap[CurrentChannel]].Stop();

									//Convert the period to offset timing interval in relation to system sampling rate
									//Using NTSC sampling
									SGE::Sound::Channels[channelMap[CurrentChannel]].offsetIncrement = PeriodToOffsetIncrement(Channel[CurrentChannel].Period);

									//Channel plays
									channelPlays[CurrentChannel] = true;

									//Check to see if we need to retrigger vibrato at the start of a new note
									if (SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Retriggers)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.CurrentWaveformPosition = 0;
									}
								}
							}

							//
							//Turn off any channel based rendering effects for now
							//If there's suppose to be an effect, it should be configured in the next heap of logic.
							//

							//Turn off Arpeggio
							SGE::Sound::Channels[channelMap[CurrentChannel]].Arpeggio.Enabled = false;

							//Turn off Volume Slide
							SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Enabled = false;

							//Turn off Vibrato
							SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Enabled = false;

							//Turn off Period Slide
							SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Enabled = false;

							//Turn off Retrigger Sample
							SGE::Sound::Channels[channelMap[CurrentChannel]].Retrigger.Enabled = false;

							//
							//Is there an effect at all?
							//If the whole effect value is 0, then there is no effect
							//
							if (modFile.patterns[modFile.patternTable[Location.Position]][Location.Division][CurrentChannel].effect != 0)
							{
								//Since the effect value is non-zero, there's some kind of effect data to be processed!
								//Let's find it!

								if (Channel[CurrentChannel].Effect.Type == 0x0)
								{
									//
									//  Configure Arpeggio or Effect 0 / 0x0
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].Arpeggio.Set(
										DEFAULT_SAMPLES_TICK,
										Channel[CurrentChannel].Effect.X,
										Channel[CurrentChannel].Effect.Y);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x1)
								{
									//
									//  Configure Slide Up or Effect 1 / 0x1
									//

									//
									//  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
										Channel[CurrentChannel].Period,
										DEFAULT_SAMPLES_TICK / 42,
										-(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
										NTSC_TUNING);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x2)
								{
									//
									//  Configure Slide Down or Effect 2 / 0x2
									//

									//
									//  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
										0,
										DEFAULT_SAMPLES_TICK / 42,
										(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
										NTSC_TUNING);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x3)
								{
									//
									//  Configure the Slide to Note of Effect 3 / 0x3
									//

									//
									//  NOTE:  Slides are divided by 42 to allow smoother transitions.  It is the answer.
									//

									//  See if there's a new slide
									if (Channel[CurrentChannel].Effect.X != 0 || Channel[CurrentChannel].Effect.Y != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
											Channel[CurrentChannel].Period,
											DEFAULT_SAMPLES_TICK / 42,
											(Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 42.0f,
											NTSC_TUNING);
									}

									// Or continue the previous one towards the current note
									else
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
											Channel[CurrentChannel].Period,
											SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.SampleInterval,
											SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Delta,
											NTSC_TUNING);
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x4)
								{
									//
									//  Configure the Vibrato Effect or Effect 4 / 0x4
									//

									//  If either the Effect X or Y is Zero, then use the previous Vibrato settings
									if (Channel[CurrentChannel].Effect.X == 0 || Channel[CurrentChannel].Effect.Y == 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Continue();
									}

									//  Otherwise it is new Vibrato!
									else
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Set(
											Channel[CurrentChannel].Effect.X / 16.0f,
											(Channel[CurrentChannel].Effect.Y * ticksADivision) / 64.0f);
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x5)
								{
									//
									//  Configure - Continue Slide to note, but do Volume Slide
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Set(
										Channel[CurrentChannel].Period,
										SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.SampleInterval,
										SGE::Sound::Channels[channelMap[CurrentChannel]].PeriodSlide.Delta,
										NTSC_TUNING);

									//  Check to see the rate we have to slide the volume up
									if (Channel[CurrentChannel].Effect.X != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											Channel[CurrentChannel].Effect.X / 64.0f);
									}
									//  Check to see the rate we have to slide the volume down
									//  Y is only paid attention if X is zero and is therefor assumed to be zero
									else if (Channel[CurrentChannel].Effect.Y != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											-Channel[CurrentChannel].Effect.Y / 64.0f);
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x6)
								{
									//
									//  Configure - Continue Vibrato, but do Volume Slide
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.Continue();
									//  Check to see the rate we have to slide the volume up
									if (Channel[CurrentChannel].Effect.X != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											Channel[CurrentChannel].Effect.X / 64.0f);
									}

									//  Check to see the rate we have to slide the volume down
									//  Y is only paid attention if X is zero and is therefor assumed to be zero
									else if (Channel[CurrentChannel].Effect.Y != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											-Channel[CurrentChannel].Effect.Y / 64.0f);
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x7)
								{
									//
									//  Place holder for Tremolo
									//
									SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", Channel[CurrentChannel].Effect.Type, Channel[CurrentChannel].Effect.X, Channel[CurrentChannel].Effect.Y);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x8)
								{
									//
									//  Place holder for Pan
									//  Rarely implemented
									//
									SGE::System::Message::Output(SGE::System::Message::Levels::Warning, SGE::System::Message::Sources::Utility, "Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", Channel[CurrentChannel].Effect.Type, Channel[CurrentChannel].Effect.X, Channel[CurrentChannel].Effect.Y);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0x9)
								{
									//
									//  Set sample offset
									//
									SGE::Sound::Channels[channelMap[CurrentChannel]].offset = (float)(Channel[CurrentChannel].Effect.X * 4096 + Channel[CurrentChannel].Effect.Y * 256) * 2;
								}
								else if (Channel[CurrentChannel].Effect.Type == 0xA)
								{
									//
									//  Volume Slide Effect
									//

									//  Check to see the rate we have to slide the volume up
									if (Channel[CurrentChannel].Effect.X != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											Channel[CurrentChannel].Effect.X / 64.0f);
									}

									//  Check to see the rate we have to slide the volume down
									//  Y is only paid attention if X is zero and is therefor assumed to be zero
									else if (Channel[CurrentChannel].Effect.Y != 0)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].VolumeSlide.Set(
											DEFAULT_SAMPLES_TICK,
											-Channel[CurrentChannel].Effect.Y / 64.0f);
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0xB)
								{
									//
									//  Configure for Jump position after this division or Effect 11 / 0xB
									//

									//  Flag a jump after this division
									postDivisionJump = true;

									//  Set target position
									postDivisionJumpTargetPosition = (Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y);
									//  Set target division
									postDivisionJumpTargetDivision = 0;
								}
								else if (Channel[CurrentChannel].Effect.Type == 0xC)
								{
									//
									//  Configure the Volume or Effect 12 / 0xC
									//

									//  Set the volume for the channel
									SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = (Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y) / 64.0f;

									//  If for some reason the channel volume is higher than 1.0f, set it back down.
									if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume > 1.0f)
									{
										SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 1.0f;
									}
								}
								else if (Channel[CurrentChannel].Effect.Type == 0xD)
								{
									//
									//  Configure pattern break or Effect 13 / 0xD
									//

									//  Flag a jump after this division
									postDivisionJump = true;

									//  Set target position
									postDivisionJumpTargetPosition = Location.Position + 1;

									//  Set target division
									postDivisionJumpTargetDivision = (Channel[CurrentChannel].Effect.X * 10 + Channel[CurrentChannel].Effect.Y);
								}
								else if (Channel[CurrentChannel].Effect.Type == 0xE)
								{
									//
									//  Configure an effect under this category.  There's a few of them shove under Effect 14 or 0xE
									//

									//
									//  Check the Effect's X setting to see what exact effect they want
									//

									if (Channel[CurrentChannel].Effect.X == 0x0)
									{
										//
										//  Place holder for setting filter
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x1)
									{
										//
										//  Place holder for Fineslide up
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x2)
									{
										//
										//  Place holder for Fineslide down
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x3)
									{
										//
										//  Place holder for Glissasndo On/Off
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x4)
									{
										//
										//  Set the Vibrato waveform
										//
										if (Channel[CurrentChannel].Effect.Y == 0)
										{
											// Use Sine waveform with retrigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseSineWaveform(true);
										}
										else if (Channel[CurrentChannel].Effect.Y == 1)
										{
											// Use Ramp Down waveform with retrigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseRampDownWaveform(true);
										}
										else if (Channel[CurrentChannel].Effect.Y == 2)
										{
											// Use Square waveform with trigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseSquareWaveform(true);
										}
										else if (Channel[CurrentChannel].Effect.Y == 3)
										{
											//
											//  Place holder for random selection with retrigger
											//
										}
										else if (Channel[CurrentChannel].Effect.Y == 4)
										{
											// Use Sine waveform without retrigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseSineWaveform(false);
										}
										else if (Channel[CurrentChannel].Effect.Y == 5)
										{
											// Use Ramp Down waveform without retrigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseRampDownWaveform(false);
										}
										else if (Channel[CurrentChannel].Effect.Y == 6)
										{
											// Use Square waveform without retrigger
											SGE::Sound::Channels[channelMap[CurrentChannel]].Vibrato.UseSquareWaveform(false);
										}
										else if (Channel[CurrentChannel].Effect.Y == 7)
										{
											//
											//  Place holder for random selection without retrigger
											//
										}
									}
									else if (Channel[CurrentChannel].Effect.X == 0x5)
									{
										//
										//  Place holder for Set finetune value
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x6)
									{
										//
										//  Place holder for Loop Pattern
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x7)
									{
										//
										//  Place holder for Set Tremolo Waveform
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x8)
									{
										//
										//  Unused
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0x9)
									{
										//
										//  Retrigger Sample
										//
										if (Channel[CurrentChannel].Effect.Y != 0)
										{
											SGE::Sound::Channels[channelMap[CurrentChannel]].Retrigger.Set(
												0,
												Channel[CurrentChannel].Effect.Y* DEFAULT_SAMPLES_TICK);
										}
									}
									else if (Channel[CurrentChannel].Effect.X == 0xA)
									{
										//
										//  Fine volume slide up
										//

										//  Increment current volume by y
										SGE::Sound::Channels[channelMap[CurrentChannel]].Volume += (Channel[CurrentChannel].Effect.Y) / 64.0f;

										//  Check to make sure the volume doesn't go over 1.0
										if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume > 1.0f)
										{
											SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 1.0f;
										}
									}
									else if (Channel[CurrentChannel].Effect.X == 0xB)
									{
										//
										//  Fine volume slide down
										//

										//  Increment current volume by y
										SGE::Sound::Channels[channelMap[CurrentChannel]].Volume -= (Channel[CurrentChannel].Effect.Y) / 64.0f;

										//  Check to make sure the volume doesn't go over 1.0
										if (SGE::Sound::Channels[channelMap[CurrentChannel]].Volume < 0.0f)
										{
											SGE::Sound::Channels[channelMap[CurrentChannel]].Volume = 0.0f;
										}
									}
									else if (Channel[CurrentChannel].Effect.X == 0xC)
									{
										//
										//  Cut sample
										//
										SGE::Sound::Channels[channelMap[CurrentChannel]].Cut.Set(Channel[CurrentChannel].Effect.Y * DEFAULT_SAMPLES_TICK);
									}
									else if (Channel[CurrentChannel].Effect.X == 0xD)
									{
										//
										//  Delay sample
										//
										SGE::Sound::Channels[channelMap[CurrentChannel]].Delay.Set(Channel[CurrentChannel].Effect.Y * DEFAULT_SAMPLES_TICK);
									}
									else if (Channel[CurrentChannel].Effect.X == 0xE)
									{
										//
										//  Place holder for delay pattern
										//
									}
									else if (Channel[CurrentChannel].Effect.X == 0xF)
									{
										//
										//  Place holder for invert loop
										//
									}
								}

								else if (Channel[CurrentChannel].Effect.Type == 0xF)
								{
									//
									//  Configure Ticks Per Division or Effect 15 / 0xF
									//

									//  If the value is 32 or below, consider it Ticks Per Division setting
									if (Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y <= 32)
									{
										ticksADivision =Channel[CurrentChannel].Effect.X * 16 + Channel[CurrentChannel].Effect.Y;
									}
									//  If the value is above 32, consider it setting the beats per minute
									else
									{

									}
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
								SGE::Sound::Channels[channelMap[c]].Play();
							}
						}
						//
						//  Wait for the next division
						//
						SGE::Utility::Timer::AccurateWaitForMilliseconds(DEFAULT_TICK_TIMING_MILLI * ticksADivision, true);


						//
						//  Calculate the delta time, post sleep
						//
						deltaTime = std::clock() - startTime;


						//
						//  Check for a post division jump
						//
						if (postDivisionJump)
						{
							//
							//  Set Position
							//
							Location.Position = postDivisionJumpTargetPosition;

							//
							//  Set Division
							//
							Location.Division = postDivisionJumpTargetDivision;

							//
							//  Reset Jump flag
							//
							postDivisionJump = false;
						}
						else
						{
							Location.NextDivision();
						}
					}
				}
			
				//
				//  Post Playback processing
				//

				//Stop all channels
				for (int c = 0; c < 4; c++)
				{
					SGE::Sound::Channels[channelMap[c]].Stop();
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