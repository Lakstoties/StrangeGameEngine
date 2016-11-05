#include "include\SGE\sound.h"

namespace SGE
{
	namespace Sound
	{
		//A list of MIDI notes and their associated frequencies.
		const float MIDI_NOTE_FRENQUENCY[]=
		{
			//Octave -1
			8.175799f,		//C		000
			8.661957f,		//Db	001
			9.177024f,		//D		002
			9.722718f,		//Eb	003
			10.300861f,		//E		004
			10.913383f,		//F		005
			11.562325f,		//Gb	006
			12.249857f,		//G		007
			12.978271f,		//Ab	008
			13.750000f,		//A		009
			14.567617f,		//Bb	010
			15.433853f,		//B		011

			//Octave 0
			16.351599f,		//C		012
			17.323914f,		//Db	013
			18.354048f,		//D		014
			19.445436f,		//Eb	015
			20.601723f,		//E		016
			21.826765f,		//F		017
			23.124651f,		//Gb	018
			24.499714f,		//G		019
			25.956543f,		//Ab	020
			27.500000f,		//A		021 <- Start of what's on a standard piano
			29.135235f,		//Bb	022
			30.867706f,		//B		023

			//Octave 1
			32.703197f,		//C		024
			34.647827f,		//Db	025
			36.708096f,		//D		026
			38.890873f,		//Eb	027
			41.203445f,		//E		028
			43.653530f,		//F		029
			46.249302f,		//Gb	030
			48.999428f,		//G		031
			51.913086f,		//Ab	032
			55.000000f,		//A		033
			58.270470f,		//Bb	034
			61.735413f,		//B		035

			//Octave 2
			65.406395f,		//C		036
			69.295654f,		//Db	037
			73.416191f,		//D		038
			77.781746f,		//Eb	039
			82.406891f,		//E		040
			87.307060f,		//F		041
			92.498604f,		//Gb	042
			97.998856f,		//G		043
			103.826172f,	//Ab	044
			110.000000f,	//A		045
			116.540939f,	//Bb	046
			123.470825f,	//B		047

			//Octave 3
			130.812790f,	//C		048
			138.591309f,	//Db	049
			146.832382f,	//D		050
			155.563492f,	//Eb	051
			164.813782f,	//E		052
			174.614120f,	//F		053
			184.997208f,	//Gb	054
			195.997711f,	//G		055
			207.652344f,	//Ab	056
			220.000000f,	//A		057
			233.081879f,	//Bb	058
			246.941650f,	//B		059

			//Octave 4
			261.625580f,	//C		060
			277.182617f,	//Db	061
			293.664764f,	//D		062
			311.126984f,	//Eb	063
			329.627563f,	//E		064
			349.228241f,	//F		065
			369.994415f,	//Gb	066
			391.995422f,	//G		067
			415.304688f,	//Ab	068
			440.000000f,	//A		069
			466.163757f,	//Bb	070
			493.883301f,	//B		071

			//Octave 5
			523.251160f,	//C		072
			554.365234f,	//Db	073
			587.329529f,	//D		074
			622.253967f,	//Eb	075
			659.255127f,	//E		076
			698.456482f,	//F		077
			739.988831f,	//Gb	078
			783.990845f,	//G		079
			830.609375f,	//Ab	080
			880.000000f,	//A		081
			932.327515f,	//Bb	082
			987.766602f,	//B		083

			//Octave 6
			1046.502319f,	//C		084
			1108.730469f,	//Db	085
			1174.659058f,	//D		086
			1244.507935f,	//Eb	087
			1318.510254f,	//E		088
			1396.912964f,	//F		089
			1479.977661f,	//Gb	090
			1567.981689f,	//G		091
			1661.218750f,	//Ab	092
			1760.000000f,	//A		093
			1864.655029f,	//Bb	094
			1975.533203f,	//B		095

			//Octave 7
			2093.004639f,	//C		096
			2217.460938f,	//Db	097
			2349.318115f,	//D		098
			2489.015869f,	//Eb	099
			2637.020508f,	//E		100
			2793.825928f,	//F		101
			2959.955322f,	//Gb	102
			3135.963379f,	//G		103
			3322.437500f,	//Ab	104
			3520.000000f,	//A		105
			3729.310059f,	//Bb	106
			3951.066406f,	//B		107

			//Octave 8
			4186.009277f,	//C		108 <-- End of what's on a standard piano
			4434.921875f,	//Db	109
			4698.636230f,	//D		110
			4978.031738f,	//Eb	111
			5274.041016f,	//E		112
			5587.651855f,	//F		113
			5919.910645f,	//Gb	114
			6271.926758f,	//G		115
			6644.875000f,	//Ab	116
			7040.000000f,	//A		117
			7458.620117f,	//Bb	118
			7902.132812f,	//B		119

			//Octave 9
			8372.018555f,	//C		120
			8869.843750f,	//Db	121
			9397.272461f,	//D		122
			9956.063477f,	//Eb	123
			10548.082031f,	//E		124
			11175.303711f,	//F		125
			11839.821289f,	//Gb	126
			12543.853516f	//G		127
		};

		//Precalculated PI to float precision
		const float PI_FLOAT = 3.14159265358979323846f;

		//Precalculated 2*PI to float precision 
		const float TWO_PI_FLOAT = 6.28318530717958647692f;

		//Precalculated PI/2 to float precision
		const float HALF_PI_FLOAT = PI_FLOAT / 2.0f;

		//Default constructor for a SoundChannel
		SoundChannel::SoundChannel()
		{
			//Most initialization done in declaration.

			//Reserved for any calculations
		}


		//Deconstructor for a SoundChannel
		SoundChannel::~SoundChannel()
		{
			//Check to see if there's pointer to some memory
			//If so, delete it.
			if (buffer != nullptr)
			{
				delete buffer;
			}
		}

		//Given arguments, render a number of samples asked and return a pointer to the buffer.
		void SoundChannel::RenderSamples(unsigned int numberOfSamples, int* sampleBuffer)
		{
			//Copy samples over to target buffer
			//We are going to assume the buffer pointer given to us is of no consequence and write over it.

			//Starting at the offset, copy over samples to the buffer.
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				if (statusState != STATUS_PLAYING_STATE)
				{
					sampleBuffer[i] = 0;
				}
				else
				{
					//If we are not using ADSR					
					if (!adsrActive)
					{
						//Copy source sample augmented by volume to the target buffer
						sampleBuffer[i] = int(buffer[offset] * volume);
					}
					//If we are using an ADSR envelope
					else
					{
						//Apply 
						sampleBuffer[i] = int (buffer[offset] * adsrLevel * volume);

						//Check to see which state we are in
						switch (adsrState)
						{
							//We are not active at all, stop playing!
						case ADSR_NONE_STATE:
							statusState = STATUS_LOADED_STATE;
							break;

							//If we are in the attack state
						case ADSR_ATTACK_STATE:

							//And we have a valid Attack Rate  (No negatives, please...)
							//And we the attack level doesn't exceed the maximum attack level
							//Zero just means we don't have a attack for all practical purposes.
							if (attackRate > 0 && adsrLevel < maxAttackLevel)
							{
								//Increase the adsr level by the attack rate
								adsrLevel += attackRate;

								//Kick it ot the next sample.
								break;
							}
							//If there's no valid attack rate, just transition to the next state.
							else
							{
								//Set the adsrLevel to the maximum;
								adsrLevel = maxAttackLevel;

								//Go to the next state
								adsrState = ADSR_DECAY_STATE;
							}

							//If we are in the decay state
						case ADSR_DECAY_STATE:

							//And we have a valid decay rate (No negatives, it's just confusing on the math...
							//And the adsrlevel isn't below the sustain level
							//Zero just means we don't have a decay for all practical purposes.
							if (decayRate > 0 && adsrLevel > sustainLevel)
							{
								//Decrease the adsr level by the decay rate
								adsrLevel -= decayRate;

								//Kick it to the next sample.
								break;
							}

							//If there's no valid decay rate, just transistion to the next state.
							else
							{
								//Set the adsrLevel to the sustain level
								adsrLevel = sustainLevel;

								//Got to the next state
								adsrState = ADSR_SUSTAIN_STATE;
							}
						
							//If we are in the sustain state
						case ADSR_SUSTAIN_STATE:

							//And we are presently triggered and have a valid sustainLevel
							//Continue

							//Other wise...
							if (!adsrTriggered || sustainLevel <= 0)
							{
								//Transition to the next state
								adsrState = ADSR_RELEASE_STATE;
							}

							break;

							//If we are in the release state
						case ADSR_RELEASE_STATE:

							//And we have a valid release rate
							//And the level is still valid
							if (releaseRate > 0.0 && adsrLevel > 0)
							{
								adsrLevel -= releaseRate;
							}

							//If there's no release rate, just transition to off.
							else
							{
								//Set the adsr level to 0
								adsrLevel = 0;

								//Change to the rest state
								adsrLevel = ADSR_NONE_STATE;

								//Hard stop the channel
								statusState = STATUS_LOADED_STATE;
							}

							break;
						}
					}

					//Calculate offset increment
					offsetIncrement += currentPitch;

					//Increment the offset by the pitch shift
					offset += unsigned int(offsetIncrement);

					//Get rid of the whole numbers and keep the change
					offsetIncrement -= int(offsetIncrement);

					//Check to see if offset has gotten to or past the end of the buffer
					if (offset >= bufferSize)
					{
						//If the sound channel loops
						if (loop)
						{
							offset = offset % bufferSize;
						}
						else
						{
							Stop();
						}
					}
				}
			}
		}

		void SoundChannel::Trigger()
		{
			//If ADSR active, set it up appropriately
			if (adsrActive)
			{
				adsrTriggered = true;
				adsrState = ADSR_ATTACK_STATE;
			}
			Play();
		}

		void SoundChannel::Release()
		{
			adsrTriggered = false;

			//If we currently aren't using ADSR envelopes
			//Just hard cut it.
			if (!adsrActive)
			{
				Stop();
			}
		}

		//Function signals the channel is playing, so it will be rendered.
		void SoundChannel::Play()
		{
			//If the channel is at least loaded
			if (statusState >= STATUS_LOADED_STATE)
			{
				//Set the flags properly
				statusState = STATUS_PLAYING_STATE;

				//Reset the play offset
				offset = 0;
				offsetIncrement = 0;
			}
		}

		//Load the SoundChannel with audio data from a sample source
		void SoundChannel::LoadSoundBuffer(unsigned int numOfSamples, short* samples)
		{
			//Check for valid sample amount
			if (numOfSamples < 0)
			{
				return;
			}
		
			//Check to see if the buffer is already in use by something
			if (buffer == nullptr)
			{
				delete buffer;
			}

			//Create the buffer
			buffer = new short[numOfSamples];

			//Make note of the size of the buffer
			bufferSize = numOfSamples;

			//Copy the buffer over
			memcpy(buffer, samples, sizeof(buffer[0]) * numOfSamples);

			//Set the buffer as loaded
			statusState = STATUS_LOADED_STATE;
		}

		//Set the key pitch for the channel audio
		void SoundChannel::SetKey(float pitch)
		{
			if (pitch > 0)
			{
				keyedPitch = pitch;
			}
		}

		//Stop the currently playing audio
		void SoundChannel::Stop()
		{
			//If the channel at least is loaded with some data
			if (statusState >= STATUS_LOADED_STATE)
			{
				//Set the flags
				statusState = STATUS_LOADED_STATE;

				//Reset the playing offset
				offset = 0;
				offsetIncrement = 0.0;
			}
		}

		//Get the keyed pitch
		float SoundChannel::GetKey()
		{
			return keyedPitch;
		}

		//Set the new pitch for the audio
		void SoundChannel::SetPitch(float newPitch)
		{
			if (newPitch > 0)
			{
				targetPitch = newPitch;
				currentPitch = targetPitch / keyedPitch;
			}		
		}

		//Get the new audio pitch
		float SoundChannel::GetPitch()
		{
			return targetPitch;
		}


		//Static callback function wrapper to call the callback of the particular SoundSystem object passed.
		int SoundSystem::PortAudioCallback(
			const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			return ((SoundSystem*)userData)->SoundSystemCallback(input, output, frameCount, timeInfo, statusFlags);
		}

		//Actual functional callback for this particular object
		int SoundSystem::SoundSystemCallback(const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags)
		{
			//Clear out the mixing buffers
			ClearMixingBuffers();

			//Recast the output buffers
			short* outputBufferLeft = ((short**)output)[0];
			short* outputBufferRight = ((short**)output)[1];


			//Check to see if the frame buffer size is bigger than the frame buffers already allocated
			if (frameCount > frameBufferSize)
			{
				DeleteFrameBuffers();
				GenerateFrameBuffers(frameCount);
				fprintf(stderr, "DEBUG:  Sound  System - Frame Buffer Size Increased. \n");
			}

			//Mix the samples into buffer
			//Go through each channel and add it's contents to the mixing buffer
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				//Go through all active channels and get samples from them
				soundChannels[i].RenderSamples(frameCount, renderedChannelBuffers[i]);

				for (unsigned int j = 0; j < frameCount; j++)
				{
					mixingFrameBufferLeft[j]  += int(renderedChannelBuffers[i][j] * soundChannels[i].pan);
					mixingFrameBufferRight[j] += int(renderedChannelBuffers[i][j] * (1.0f - soundChannels[i].pan));
				}
			}

			//Dump it into the output buffer
			//Adjust to master volume...
			//If the sample size exceeds the sample limit, just hard limit it.
			//Well... people...  Don't fry your damn audio.

			for (unsigned int i = 0; i < frameCount; i++)
			{
				//Apply master volume to the mixing frame buffers
				mixingFrameBufferLeft[i]  = int(mixingFrameBufferLeft[i]  * masterVolume);
				mixingFrameBufferRight[i] = int(mixingFrameBufferRight[i] * masterVolume);

				//Check for things hitting the upper and lower ends of the range
				//For the left channel
				if (mixingFrameBufferLeft[i] > SAMPLE_MAX_AMPLITUDE)
				{
					outputBufferLeft[i] = SAMPLE_MAX_AMPLITUDE;
				}
				else if (mixingFrameBufferLeft[i] < -SAMPLE_MAX_AMPLITUDE)
				{
					outputBufferLeft[i] = -SAMPLE_MAX_AMPLITUDE;
				}
				else
				{
					outputBufferLeft[i] = mixingFrameBufferLeft[i];
				}


				//For the right channel
				if (mixingFrameBufferRight[i] > SAMPLE_MAX_AMPLITUDE)
				{
					outputBufferRight[i] = SAMPLE_MAX_AMPLITUDE;
				}
				else if (mixingFrameBufferRight[i] < -SAMPLE_MAX_AMPLITUDE)
				{
					outputBufferRight[i] = -SAMPLE_MAX_AMPLITUDE;
				}
				else
				{
					outputBufferRight[i] = mixingFrameBufferRight[i];
				}
			}

			//Return
			//If no major errors, continue the stream.
			return paContinue;
		}

		//Start the audio system.
		void SoundSystem::Start()
		{
			//Check to make sure the stream isn't already active
			if (!Pa_IsStreamActive(soundSystemStream))
			{
				//Start the stream
				portAudioError = Pa_StartStream(soundSystemStream);

				//Check for errors
				if (portAudioError != paNoError)
				{
					fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
				}
				else
				{
					printf("PortAudio Stream Started.\n");
				}
			}
		}

		//Stop the audio system
		void SoundSystem::Stop()
		{
			//Check to see if the stream isn't already stopped.
			if (!Pa_IsStreamStopped(soundSystemStream))
			{
				//Stop the PortAudio Stream
				portAudioError = Pa_StopStream(soundSystemStream);

				//Check for errors
				if (portAudioError != paNoError)
				{
					fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
				}
				else
				{
					printf("PortAudio Stream Stopped.\n");
				}
			}
		}


		//Sample Generation Tools
		//Generates a sine wave
		short* Generators::SineGenerator(float hertz, unsigned int samplesToGenerate, short amplitude)
		{
			//Create buffer to store samples in
			short* tempBuffer = new short[samplesToGenerate];

			//Calculate radian to frequency step
			float periodStepping = (TWO_PI_FLOAT * hertz) / SAMPLE_RATE;

			for (unsigned int i = 0; i < samplesToGenerate; i++)
			{
				tempBuffer[i] = short(amplitude * sinf(periodStepping * i));
			}

			//Return a pointer to what we have wrought
			return tempBuffer;
		}

		//Generates a pulse wave
		short* Generators::PulseGenerator(float hertz, float dutyCycle, unsigned int samplesToGenerate, short amplitude)
		{
			//Create buffer to store samples in
			short* tempBuffer = new short[samplesToGenerate];

			//Calculate radian to frequency step
			float periodStepping = (TWO_PI_FLOAT * hertz) / SAMPLE_RATE;

			//Variable to keep track of the current period of the current hertz
			float currentPeriod = 0;

			//Variable to keep track of the duty cycle length
			float dutyCycleLength;

			//Check for valid dutyCycle
			if (dutyCycle > 0 && dutyCycle < 1.0)
			{
				//If valid calculate the dutyCycleLength
				dutyCycleLength = dutyCycle * TWO_PI_FLOAT;

			}
			//If not a valid dutyCycle
			else
			{
				//Calculate dutyCycleLength using .50 dutyCycle or 50%
				dutyCycleLength = PI_FLOAT;
			}

			//Generate some pulses
			for (unsigned int i = 0; i < samplesToGenerate; i++)
			{
				//Check to see if we are within the duty cycle of the current hertz, given the current period.
				if (currentPeriod < dutyCycleLength)
				{
					tempBuffer[i] = amplitude;
				}
				else
				{
					tempBuffer[i] = -amplitude;
				}

				//Go to the next period step
				currentPeriod += periodStepping;

				//If we got through a whole hertz
				if (currentPeriod > TWO_PI_FLOAT)
				{
					//Roll it back over to the start of a new one.
					currentPeriod -= TWO_PI_FLOAT;
				}
			}
		
			//Return a pointer to what we have wrought
			return tempBuffer;
		}

		//Generates a triangle wave
		short* Generators::TriangleGenerator(float hertz, unsigned int samplesToGenerate, short amplitude)
		{
			//Create buffer to store samples in
			short* tempBuffer = new short[samplesToGenerate];

			//Calculate radian to frequency step
			float periodStepping = (TWO_PI_FLOAT * hertz) / SAMPLE_RATE;

			//Variable to keep track of the current period of the current hertz
			float currentPeriod = 0;

			//Generate some triangles
			for (unsigned int i = 0; i < samplesToGenerate; i++)
			{

				//If we are in the first incline
				if (currentPeriod < HALF_PI_FLOAT)
				{
					tempBuffer[i] = short((currentPeriod / HALF_PI_FLOAT) * amplitude);
				}

				//If we are in the decline (positive part)
				else if (currentPeriod < PI_FLOAT)
				{
					tempBuffer[i] = short(((PI_FLOAT - currentPeriod) / HALF_PI_FLOAT) * amplitude);
				}

				//If we are in the decline (negative part)
				else if (currentPeriod < (PI_FLOAT + HALF_PI_FLOAT))
				{
					tempBuffer[i] = short(((currentPeriod - PI_FLOAT - HALF_PI_FLOAT) / HALF_PI_FLOAT) * -amplitude);
				}
			
				//If we are in the second incline
				else if (currentPeriod < TWO_PI_FLOAT)
				{
					tempBuffer[i] = short(((TWO_PI_FLOAT - currentPeriod) / HALF_PI_FLOAT) * -amplitude);
				}

				//Go to the next period step
				currentPeriod += periodStepping;

				//If we got through a whole hertz
				if (currentPeriod > TWO_PI_FLOAT)
				{
					//Roll it back over to the start of a new one.
					currentPeriod -= TWO_PI_FLOAT;
				}
			}

			//Return a pointer to what we have wrought
			return tempBuffer;
		}

		//Generates a sawtooth wave
		short* Generators::SawtoothGenerator(float hertz, unsigned int samplesToGenerate, short amplitude)
		{
			return nullptr;
		}

		//Generates noise
		short* Generators::NoiseGenerator(unsigned int samplesToGenerate, short amplitude)
		{
			//Create buffer to store samples in
			short* tempBuffer = new short[samplesToGenerate];

			//Return a pointer to what we have wrought
			return tempBuffer;
		}

		//Default constructor for the SoundSystem
		SoundSystem::SoundSystem()
		{
			//Initialize PortAudio
			portAudioError = Pa_Initialize();

			//Check for any errors
			if (portAudioError != paNoError)
			{
				fprintf(stderr,"PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}

			//Setup Stream parameters
			//Grab the default output device
			outputParameters.device = Pa_GetDefaultOutputDevice();

			//If there is not default output device, Error
			if (outputParameters.device == paNoDevice)
			{
				fprintf(stderr, "PortAudio Error: No default output device.\n");
			}

			//Set channels
			outputParameters.channelCount = 2;
			//Set sample format
			outputParameters.sampleFormat = paInt16 | paNonInterleaved;
			//Set suggested latency
			outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
			//Set host API
			outputParameters.hostApiSpecificStreamInfo = NULL;
			


			//Open up PortAudio Stream
			portAudioError = Pa_OpenStream(
				&soundSystemStream,
				NULL,
				&outputParameters,
				SAMPLE_RATE,
				paFramesPerBufferUnspecified,
				paClipOff,
				&PortAudioCallback,
				this);

			//Check for any errors
			if (portAudioError != paNoError)
			{
				fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
			else
			{
				printf("PortAudio Stream Opened.\n");
			}

			//Generate the initial render frame buffers
			GenerateFrameBuffers(INITIAL_RENDER_FRAME_BUFFER_SIZE);
		}

		//Creates and sets up frame buffers for audio data
		void SoundSystem::GenerateFrameBuffers(unsigned long newFrameBufferSize)
		{
			//Set the new render frame buffer size
			frameBufferSize = newFrameBufferSize;

			//Generate new render frame buffers
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				renderedChannelBuffers[i] = (int*)malloc(frameBufferSize * sizeof(int));
			}

			//Generate new mixing frame buffers
			mixingFrameBufferLeft = (int*)malloc(frameBufferSize * sizeof(int));
			mixingFrameBufferRight = (int*)malloc(frameBufferSize * sizeof(int));
		}

		//Deletes and cleans up Frame buffers that were holding audio data
		void SoundSystem::DeleteFrameBuffers()
		{
			//Delete old render frame buffers
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				free(renderedChannelBuffers[i]);
			}

			//Delete old mixing frame buffers
			free(mixingFrameBufferLeft);
			free(mixingFrameBufferRight);
		}

		//Clears and zeros out data in the mixing buffers
		void SoundSystem::ClearMixingBuffers()
		{
			//Initialize the mixing buffers
			//Depending on the platform, possibly not needed, but some platforms don't promise zeroed memory upon allocation.
			memset(mixingFrameBufferLeft, 0, frameBufferSize * sizeof(int));
			memset(mixingFrameBufferRight, 0, frameBufferSize * sizeof(int));
		}

		//Deconstructor for the Soundsystem
		SoundSystem::~SoundSystem()
		{
			//Close the PortAudio stream
			portAudioError = Pa_CloseStream(soundSystemStream);

			if (portAudioError != paNoError)
			{
				fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
			else
			{
				printf("PortAudio Stream Closed.\n");
			}

			//Stream closed, it should be safe to nuke the render frame buffers
			DeleteFrameBuffers();

			//Terminate PortAudio
			//Check to see if the initialize succeeded, otherwise return
			if (paInitializeReturnedError)
			{
				return;
			}

			//Shut down Port Audio
			portAudioError = Pa_Terminate();

			//Check for any errors
			if (portAudioError != paNoError)
			{
				fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
		}

		//Default constructor for a SoundSystemWaveFile
		SoundSystemWaveFile::SoundSystemWaveFile()
		{
			audioData = nullptr;
			numberOfSamples = 0;
		}

		//Deconstructor for a SoundSystemWaveFile
		SoundSystemWaveFile::~SoundSystemWaveFile()
		{
			//If audio data actually got loaded, delete it!
			if (audioData != nullptr)
			{
				for (unsigned int i = 0; i < numberOfChannels; i++)
				{
					//Delete the buffer for that channel's data
					delete audioData[i];
				}

				//Finally delete the data for the array of buffer pointers
				delete audioData;
			}
		}

		//Load audio data from a file into a collection of audio buffers
		//Returns a 0 if all is well, something else if there is an error.
		int SoundSystemWaveFile::LoadFile(char* targetFilename)
		{
			FILE* soundFile;
			unsigned int readCount = 0;
			bool keepLookingThroughSubchunks = true;

			soundFile = fopen(targetFilename, "rb");

			//Check to see if the file is even there.
			if (soundFile == NULL)
			{
				fprintf(stderr, "Sound System Wave File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//Start parsing the file and check to see if it is valid or not

			//Try to read a RIFF header in
			readCount = fread(&waveFileHeader, 1, sizeof(waveFileHeader), soundFile);

			//Check to see if we actually read enough bytes to make up a proper wave file header
			if (readCount != sizeof(waveFileHeader))
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -2;
			}

			//Offset: 0		Size: 4		ChunkID:				Check the chunk ID to see if it says RIFF in ASCII
			if (memcmp("RIFF", &waveFileHeader.chunkID, 4) != 0)
			{
				//No RIFF header... Not the format we are looking for!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect header.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -10;
			}

			//Offset: 4		Size: 4		ChunkSize:				Check the chunk size and save it for double checking purposes
			printf("DEBUG: Sound System Wave File: %s - Chunk Size: %d\n", targetFilename, waveFileHeader.chunkSize);

			//Offset: 8		Size: 4		Format:		Check format for "WAVE"
			if (memcmp("WAVE", &waveFileHeader.format, 4) != 0)
			{
				//No WAVE format... Not the format we are looking for!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect Encoding Format.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -20;
			}

			//First chunk of data: "fmt "
			//Offset: 12	Size: 4		SubchunkID:				Look for the "fmt " (space is null)

			//Start going through the subchunks looking for the "fmt " subchunk
			while (keepLookingThroughSubchunks)
			{
				//Read in the header info
				readCount = fread(&subChunkHeader, 1, sizeof(subChunkHeader), soundFile);

				//Is this the "fmt " subchunk
				if (memcmp("fmt", &subChunkHeader.subChunkID, 3) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					if (fseek(soundFile, subChunkHeader.subChunkSize, SEEK_CUR) != 0)
					{
						fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

						//Close out the file
						fclose(soundFile);

						return -30;
					}
				}

				//If we hit end of file early
				if (readCount < sizeof(subChunkHeader))
				{
					fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

					//Close out the file
					fclose(soundFile);

					return -30;
				}
			}

			//Offset: 16	Size: 4		Subchunksize:			For PCM files, it should be 16, otherwise this is probably a different format
			if (subChunkHeader.subChunkSize != 16)
			{
				//The format chunk size should be 16 bytes, if not... This isn't right
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -40;
			}


			//Confirmed a good subChunkHeader, read in the data
			readCount = fread(&fmtSubChunkData, 1, sizeof(fmtSubChunkData), soundFile);

			//If we hit end of file early
			if (readCount < sizeof(fmtSubChunkData))
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - fmt subchunk too small.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -45;
			}


			//Offset: 20	Size: 2		Audio Format:			Should be 1 for PCM.  If not 1, then probably another format we aren't wanting.
			if (fmtSubChunkData.audioFormat != 1)
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -50;
			}

			//Offset: 22	Size: 2		Number of Channels:		1 = Mono, 2 = Stereo, and so forth.   We are looking for Mono channels currently,
			//May support more channels later
			printf("DEBUG: Sound System Wave File: %s - Number of Channels: %d\n", targetFilename, fmtSubChunkData.numberOfChannels);

			//Offset: 24	Size: 4		Sample Rate
			//Sample Rate of the data.  Currently looking for 44100.
			//May implement resampling in future, but not right now.
	
			if (fmtSubChunkData.sampleRate != 44100)
			{
				//Not 44100Hz sample rate...  Not exactly an error, but no support for other sample rates at the moment.
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" has an unsupported sample rate.  44100Hz Only.  Sorry.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -70;
			}


			//Offset: 28	Size: 4		Byte Rate
			//Equals to:  Sample Rate * Number of Channels * Bits Per Sample / 8
			//The number of bytes per second

			//Offset: 32	Size: 2		Block Alignment
			//Equals to:  Numbers of Channels * Bits Per Sample / 8
			//The number of bytes per frame/sample

			//Offset: 34	Size: 2		Bits Per Sample
			//Bits of data per sample
			//We presently want to see 16 bits.

			if (fmtSubChunkData.bitsPerSample != 16)
			{
				//Not 16 bit sample depth...  Not exactly an error, but no support for other bit rates at the moment.
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" has an unsupported bit rate.  16-bit Only.  Sorry.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -100;
			}


			//Reset the logic
			keepLookingThroughSubchunks = true;

			//Start going through the subchunks looking for the "data" subchunk
			while (keepLookingThroughSubchunks)
			{
				//Read in the header info
				readCount = fread(&subChunkHeader, 1, sizeof(subChunkHeader), soundFile);

				//Is this the "data" subchunk
				if (memcmp("data", &subChunkHeader.subChunkID, 4) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					if (fseek(soundFile, subChunkHeader.subChunkSize, SEEK_CUR) != 0)
					{
						fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

						//Close out the file
						fclose(soundFile);

						return -105;
					}
				}


				//If we hit end of file early
				if (readCount < sizeof(subChunkHeader))
				{
					fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

					//Close out the file
					fclose(soundFile);

					return -105;
				}
			}


			//If we hit end of file early
			if (readCount < sizeof(subChunkHeader))
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - data subchunk too small.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -105;
			}


			//Second chunk of data: "data"
			//Offset: 36	Size: 4		Subchunk 2 ID
			//ID to indicate the next subchunk of data, if it isn't "data" something is wrong
	
			if (memcmp("data", &subChunkHeader.subChunkID, 4) != 0)
			{
				//Where's the data ID... 
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect Data Chunk ID.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -110;
			}

			//Offset: 40	Size: 4		Subchunk 2 Size
			//Data that indicates the size of the data chunk
			//Equal to:  Number of Samples * Number of Channels * Bits Per Sample / 8
			//Indicates the amount to read after this chunk
	
			printf("DEBUG: Sound System Wave File: %s - Data Size: %d\n", targetFilename, subChunkHeader.subChunkSize);


			//Offset: 44	Size: *?	Data
			//Actual sound data
		
			//Calculate the number of Samples
			numberOfSamples = subChunkHeader.subChunkSize / fmtSubChunkData.blockAlignment;

			numberOfChannels = fmtSubChunkData.numberOfChannels;


			//Create buffers to hold the data
			//Create the pointer array for the channel buffers
			audioData = new short*[numberOfChannels];

			//Create short arrays to hold the channel data
			for (unsigned int i = 0; i < numberOfChannels; i++)
			{
				audioData[i] = new short[numberOfSamples];
			}

			//Read the data in
			readCount = 0;

			//For each sample
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//For each channel
				for (unsigned int j = 0; j < numberOfChannels; j++)
				{
					readCount += fread(&audioData[j][i], 2, 1, soundFile);
				}
			}

			printf("DEBUG: Sound System Wave File: %s - Data Bytes Read: %d\n", targetFilename, readCount * 2);

			//If we get here... It's all good!... Maybe... Hoepfully?

			//Close out the sound file
			fclose(soundFile);

			return 0;
		}
	
	}
}