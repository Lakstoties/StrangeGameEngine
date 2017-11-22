#include "include\SGE\sound.h"
#include <portaudio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <random>
#include <fstream>
#include <algorithm>
#include <cstring>

namespace SGE
{
	namespace Sound
	{
		//
		//
		//  Sound Channel Definitions
		//
		//

		//Given arguments, render a number of samples asked and return a pointer to the buffer.
		void SoundChannel::Render(unsigned int numberOfSamples, int* sampleBuffer)
		{
			unsigned int currentSampleAverage = 0;

			//Starting at the offset, copy over samples to the buffer.
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//If we are currently not playing
				//And there's actually something to play.
				if (!Playing || currentSampleBuffer->bufferSize == 0 || currentSampleBuffer->buffer == nullptr)
				{
					//Nothing playing, 0 out the samples
					sampleBuffer[i] = 0;
				}
				else
				{
					//
					//  Grab the sample for the offset
					//

					//Copy the sample from the source buffer to the target buffer and adjusted the volume.
					//If the volume effect is in use, use that volume value.
					sampleBuffer[i] = int(currentSampleBuffer->buffer[(unsigned int)offset] *  Volume);

					//Add to the acculumator for the average
					//Negate negatives since we are only interested in overall amplitude
					currentSampleAverage += abs(sampleBuffer[i]);

					//
					//  Calculate the next offset based on the appropriate increment
					//

					//Check to see if the Arpeggio Effect is in effect
					if (EnableArpeggio)
					{
						//Check the state of the Arpeggio effect
						if (currentArpeggioSamples > ArpeggioSampleInterval)
						{
							//Reset and roll the counter
							currentArpeggioSamples %= ArpeggioSampleInterval;

							//Transition the state
							//Increment and then modulus to the states around.
							++arpeggioState %= 3;

							//Based on that state alter the argpeggio offset increment

							//If at the base state
							if (arpeggioState == 0)
							{
								arpeggioOffsetIncrement = offsetIncrement;
							}

							//Otherwise we are doing semitones
							else
							{
								//Caclulate the semitone using the current offset
								//Check to see if state 1 or 2 and use X and Y values accordingly
								arpeggioOffsetIncrement = float(offsetIncrement * pow(SEMITONE_MULTIPLIER, arpeggioState == 1 ? arpeggioSemitoneX : arpeggioSemitoneY));
							}
						}

						//Advance the number of samples
						currentArpeggioSamples++;
					}

					if (EnableVolumeSlide)
					{
						//Check the state of Volume Slide Effect
						if (currentVolumeSlideSamples > VolumeSlideSampleInterval)
						{
							//Reset and roll the counter
							currentVolumeSlideSamples %= VolumeSlideSampleInterval;

							//Apply the slide
							Volume += VolumeSlideRate;

							//Check to make sure it did not exit normal ranges
							//Short circuit logic
							(Volume < 0.0f) && (Volume = 0.0f);
							(Volume > 1.0f) && (Volume = 1.0f);
						}
						currentVolumeSlideSamples++;
					}

					//
					//  Increment Offset  Appropriately
					//
					offset += EnableArpeggio ? arpeggioOffsetIncrement : offsetIncrement;


					//
					//  Check for repeating loops
					//

					//Check to see if this same is suppose to repeat and is set to do so... correctly
					if ((currentSampleBuffer->repeatDuration > 0) && ((unsigned int)offset >= (currentSampleBuffer->repeatOffset + currentSampleBuffer->repeatDuration)))
					{
						//Rewind back by the repeatDuration.
						offset -= currentSampleBuffer->repeatDuration;
					}


					//
					//  Check to see if it's gone pass the valid bufferSize
					//

					//If not repeatable and the offset has gone past the end of the buffer
					if ((unsigned int)offset >= currentSampleBuffer->bufferSize)
					{
						//Fuck this shit we're out!
						Stop();
					}
				}
			}

			//Update the sample level average
			LastRenderedAverageLevel = currentSampleAverage / numberOfSamples;
		}


		//Function signals the channel is playing, so it will be rendered.
		void SoundChannel::Play()
		{
			//If the channel is at least loaded
			if (currentSampleBuffer != nullptr)
			{
				//Reset the play offset
				offset = 0;

				//Set the flags properly
				Playing = true;
			}
		}

		//Stop the currently playing audio
		void SoundChannel::Stop()
		{
			//If the channel at least is loaded with some data
			if (currentSampleBuffer != nullptr)
			{
				//Set the flags
				Playing = false;

				//Reset the playing offset
				offset = 0;
			}
		}


		//
		//
		// Sound Sample Buffer Defintions
		//
		//

		//Create a blank buffer of a certain sample size
		int SoundSampleBuffer::CreateBlankBuffer(unsigned int numOfSamples)
		{
			//Reset the buffer
			ResetBuffer();

			//Set the buffer size
			bufferSize = numOfSamples;

			//Get some new some ram for the buffer
			buffer = new short[bufferSize];

			//Zero out the buffer to make sure it is clean
			//Some OSes don't make the ram is clean when given
			ZeroBuffer();

			//Everything thing should be okay.
			return 0;
		}

		//Create a blank buffer, and then load data into it.
		int SoundSampleBuffer::Load(unsigned int numOfSamples, short *samples)
		{
			//Get a clean buffer
			CreateBlankBuffer(numOfSamples);

			//memcpy over the data into the buffer
			//std::memcpy(buffer, samples, sizeof(short) * bufferSize);
			//std::copy(samples, samples + numOfSamples, buffer);
			std::memmove(buffer, samples, sizeof(short) * bufferSize);

			//Everything should have gone okay...
			return 0;
		}

		//Zero out a buffer completely
		int SoundSampleBuffer::ZeroBuffer()
		{
			//Check to make sure there's actually a buffer to zero out
			if (buffer == nullptr)
			{
				//Hey, there's no buffer to zero out!
				return -1;
			}

			//Otherwise memset the bitch.
			std::memset(buffer, 0, sizeof(short) * bufferSize);

			//Everything happened okay in theory
			return 0;
		}

		//Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
		int SoundSampleBuffer::ResetBuffer()
		{
			//Delete the buffer
			delete[] buffer;

			//Reset the buffer size
			bufferSize = 0;

			//Everything happened okay in theory
			return 0;
		}

		//Destructor to make sure the buffer memory is freed upon destruction to prevent memory leaks.
		SoundSampleBuffer::~SoundSampleBuffer()
		{
			//Reset the buffer which will free the memory.
			ResetBuffer();
		}


		//
		//
		//  Generators Definitions
		//
		//

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

		//Varibles to hold average volume levels from the last callback from PortAudio
		unsigned int MasterVolumeAverageLeftLevel = 0;
		unsigned int MasterVolumeAverageRightLevel = 0;


		//All the sound samples in the system
		SoundSampleBuffer SampleBuffers[Sound::MAX_SAMPLE_BUFFERS];

		//All the system's sound channels
		SoundChannel Channels[MAX_CHANNELS];

		//Initialize size for the Render Frame Buffers
		const unsigned long INITIAL_RENDER_FRAME_BUFFER_SIZE = 1024;

		//Master volume for system
		float MasterVolume = 1.0f;

		//All the sound channel target render buffers
		int* renderedChannelBuffers[MAX_CHANNELS] = { nullptr };

		//32-bit mixing buffers
		int* mixingFrameBufferRight = nullptr;		//Mixing buffer for Right Channel
		int* mixingFrameBufferLeft  = nullptr;		//Mixing buffer for Left Channel

		//A thread reserved to run management functions for the sound system
		std::thread SoundManagerThread;

		//Current Frame Buffer Sizes
		unsigned long frameBufferSize = 0;

		//Creates and sets up frame buffers for audio data
		void GenerateFrameBuffers(unsigned long newFrameBufferSize)
		{
			//Delete any old buffers
			delete[] mixingFrameBufferLeft;
			delete[] mixingFrameBufferRight;

			//Set the new render frame buffer size
			frameBufferSize = newFrameBufferSize;

			//Generate new mixing frame buffers
			mixingFrameBufferLeft  = new int[frameBufferSize];
			mixingFrameBufferRight = new int[frameBufferSize];

			//Generate new render frame buffers
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				//Delete any old buffer
				delete[] renderedChannelBuffers[i];
				renderedChannelBuffers[i] = new int[frameBufferSize];
			}
		}

		//Deletes and cleans up Frame buffers that were holding audio data
		void DeleteFrameBuffers()
		{
			delete[] mixingFrameBufferLeft;
			delete[] mixingFrameBufferRight;

			//Reset the pointer values to nullptr
			mixingFrameBufferLeft = nullptr;
			mixingFrameBufferRight = nullptr;

			//Delete old render frame buffers
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				delete[] renderedChannelBuffers[i];
				renderedChannelBuffers[i] = nullptr;
			}
		}

		//Flag to indicate if this object's attempt to initialize port audio threw an error
		bool paInitializeReturnedError = false;

		//PortAudio Error variable
		PaError portAudioError;

		//PortAudio Stream
		PaStream *soundSystemStream;

		//PortAudio Stream Parameters
		PaStreamParameters outputParameters;

		//Static callback function wrapper to call the callback of the particular SoundSystem object passed.
		int PortAudioCallback(
			const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData)
		{
			//Statistics stuff
			unsigned int currentLeftAverageLevel = 0;
			unsigned int currentRightAverageLevel = 0;

			//Recast the output buffers
			short* outputBufferLeft = ((short**)output)[0];
			short* outputBufferRight = ((short**)output)[1];


			//Check to see if the frame buffer size is bigger than the frame buffers already allocated
			if (frameCount > frameBufferSize)
			{
				DeleteFrameBuffers();
				GenerateFrameBuffers(frameCount);
				fprintf(stderr, "DEBUG:  Sound  System - Frame Buffer Size Increased to: %lu\n", frameBufferSize);
			}

			//Initialize the mixing buffers
			//Depending on the platform, possibly not needed, but some platforms don't promise zeroed memory upon allocation.
			std::memset(mixingFrameBufferLeft,  0, sizeof(int) * frameBufferSize);
			std::memset(mixingFrameBufferRight, 0, sizeof(int) * frameBufferSize);

			//Go through each channel and render samples
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				Channels[i].Render(frameCount, renderedChannelBuffers[i]);
			}

			//Dump it into the output buffer
			//Adjust to master volume...
			//If the sample size exceeds the sample limit, just hard limit it.
			//Well... people...  Don't fry your damn audio.
			for (unsigned int i = 0; i < frameCount; i++)
			{
				//Down mix all the samples from all the channels
				for (unsigned int j = 0; j < MAX_CHANNELS; j++)
				{
					mixingFrameBufferLeft[i]  += int(renderedChannelBuffers[j][i] * (0.5f + Channels[j].Pan));
					mixingFrameBufferRight[i] += int(renderedChannelBuffers[j][i] * (0.5f - Channels[j].Pan));
				}

				//Apply master volume to the mixing frame buffers
				mixingFrameBufferLeft[i]  = int (mixingFrameBufferLeft[i]  * MasterVolume);
				mixingFrameBufferRight[i] = int (mixingFrameBufferRight[i] * MasterVolume);

				//Check for things hitting the upper and lower ends of the range
				//For the left channel
				outputBufferLeft[i] = (-SAMPLE_MAX_AMPLITUDE <= mixingFrameBufferLeft[i] && mixingFrameBufferLeft[i] <= SAMPLE_MAX_AMPLITUDE) ?		//Check to see if the sample value is within valid range
					mixingFrameBufferLeft[i] :																										//If so, use that value
					(mixingFrameBufferLeft[i] > 0) ?																								//Otherwise use a max range value, check to see if positive
					SAMPLE_MAX_AMPLITUDE :																											//Use positive value
					-SAMPLE_MAX_AMPLITUDE;																											//Use negative value

				//For the right channel
				outputBufferRight[i] = (-SAMPLE_MAX_AMPLITUDE <= mixingFrameBufferRight[i] && mixingFrameBufferRight[i] <= SAMPLE_MAX_AMPLITUDE) ?	//Check to see if the sample value is within valid range
					mixingFrameBufferRight[i] :																										//If so, use that value
					(mixingFrameBufferRight[i] > 0) ?																								//Otherwise use a max range value, check to see if positive
					SAMPLE_MAX_AMPLITUDE :																											//Use positive value
					-SAMPLE_MAX_AMPLITUDE;																											//Use negative value

				//Sum up the levels
				//Negate negative values since we are interested in overall amplitude and not phasing
				//For the left channel
				currentLeftAverageLevel += abs(outputBufferLeft[i]);

				//For the right channel
				currentRightAverageLevel += abs(outputBufferRight[i]);
			}

			//Report stats
			MasterVolumeAverageLeftLevel = currentLeftAverageLevel / frameCount;
			MasterVolumeAverageRightLevel = currentRightAverageLevel / frameCount;

			//Return
			//If no major errors, continue the stream.
			return paContinue;
		}

		//Start the audio system.
		void Start()
		{
			//Initialize PortAudio
			portAudioError = Pa_Initialize();

			//Check for any errors
			if (portAudioError != paNoError)
			{
				fprintf(stderr, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
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
				NULL);


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
		void Stop()
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




		//
		//
		//  sound System Module File Definitions
		//
		//

		ModuleFile::ModuleFile()
		{

		}


		ModuleFile::~ModuleFile()
		{
			for (int i = 0; i < 31; i++)
			{
				delete[] samples[i].data;
			}
		}

		int ModuleFile::LoadFile(char* targetFilename)
		{
			FILE* moduleFile;
			size_t readCount = 0;
			size_t totalReadCount = 0;
			unsigned char readBuffer[8];


			//Attempt to open the file.
			moduleFile = fopen(targetFilename, "rb");

			//Check to see if the file is even there.
			if (moduleFile == NULL)
			{
				fprintf(stderr, "Sound System Module File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//Start parsing through the file and grabbing all the data.

			//Read the module file title info
			readCount = fread(&header.title, 1, 20, moduleFile);
			totalReadCount += readCount;

			//DEBUG:  Current number of bytes read
			fprintf(stderr, "DEBUG:  Post Title: Read Count: %d \n", totalReadCount);

			//Read the module file's sample data
			for(int i = 0; i < 31; i++)
			{
				//Read sample title
				readCount = fread(&samples[i].title, 1, 22, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 22)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -2;
				}

				//Read sample length
				readCount = fread(&samples[i].lengthInWords, 1, 2, moduleFile);
				totalReadCount += readCount;

				//Flip the bits around since Amigas were Big Endian machines.
				samples[i].lengthInWords = (((samples[i].lengthInWords & 0x00FF) << 8) | ((samples[i].lengthInWords & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -3;
				}

				//Read sample finetune
				readCount = fread(&samples[i].finetune, 1, 1, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 1)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -4;
				}

				//Read sample volume
				readCount = fread(&samples[i].volume, 1, 1, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 1)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -5;
				}

				//Read repeat start offset
				readCount = fread(&samples[i].repeatOffset, 1, 2, moduleFile);
				totalReadCount += readCount;

				samples[i].repeatOffset = (((samples[i].repeatOffset & 0x00FF) << 8) | ((samples[i].repeatOffset & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -6;
				}

				//Read repeat length
				readCount = fread(&samples[i].repeatLength, 1, 2, moduleFile);
				totalReadCount += readCount;

				samples[i].repeatLength = (((samples[i].repeatLength & 0x00FF) << 8) | ((samples[i].repeatLength & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way to small to be a proper wav file
					fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -7;
				}
			}

			//DEBUG: Check current read count
			fprintf(stderr, "DEBUG:  Post Sample Headers: Current Read Count: %d \n", totalReadCount);


			//Read number of song positions
			readCount = fread(&header.songPositions, 1, 1, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 1)
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -8;
			}

			//Burn through a read
			readCount = fread(&readBuffer, 1, 1, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 1)
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -9;
			}

			//Read the pattern table
			readCount = fread(&header.patternTable, 1, 128, moduleFile);
			totalReadCount += readCount;

			//DEBUG: Check current read count
			fprintf(stderr, "DEBUG:  Current Read Count: %d \n", totalReadCount);

			//Check to see if we actually read enough bytes
			if (readCount != 128)
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -10;
			}

			//Read some tag info
			readCount = fread(&readBuffer, 1, 4, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 4)
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -11;
			}

			//Check to see if there's anything special
			//Check for "M.K."
			if (memcmp(&readBuffer, "M.K.", 4) == 0)
			{
				//Detect M.K. Signature
				fprintf(stderr, "DEBUG: M.K. Module File signature detected.\n");
			}

			//Check for "FLT4"
			else if (memcmp(&readBuffer, "FLT4", 4) == 0)
			{
				//Detect FLT4 Signature
				fprintf(stderr, "DEBUG: FLT4 Module File signature detected.\n");
			}

			//Check for "FLT8"
			else if (memcmp(&readBuffer, "FLT8", 4) == 0)
			{
				//Detect FLT8 Signature
				fprintf(stderr, "DEBUG: FLT8 Module File signature detected.\n");
			}

			//Check for "4CHN"
			else  if (memcmp(&readBuffer, "4CHN", 4) == 0)
			{
				//Detect 4CHN Signature
				fprintf(stderr, "DEBUG: 4CHN Module File signature detected.\n");
			}

			//Check for "6CHN"
			else if (memcmp(&readBuffer, "6CHN", 4) == 0)
			{
				//Detect 6CHN Signature
				fprintf(stderr, "DEBUG: 6CHN Module File signature detected.\n");
			}

			//Check for "8CHN"
			else if (memcmp(&readBuffer, "8CHN", 4) == 0)
			{
				//Detect 8CHN Signature
				fprintf(stderr, "DEBUG: 8CHN Module File signature detected.\n");
			}

			//No letters found
			else
			{
				//Move the seek back since this is part of the pattern data
				fseek(moduleFile, -4, SEEK_CUR);
				totalReadCount -= 4;
				fprintf(stderr, "DEBUG: No Module File signature detected.\n");
			}

			//Figure out how many patterns there are to read.
			//Go through the pattern table and find the largest number.
			for (int i = 0; i < 128; i++)
			{
				if (header.patternTable[i] > numberOfPatterns)
				{
					numberOfPatterns = header.patternTable[i];
					fprintf(stderr, "DEBUG: Current number of patterns: %d\n", numberOfPatterns);
				}
			}

			fprintf(stderr, "DEBUG: Module Load: %d patterns found.\n", numberOfPatterns);

			//Start churning through all the pattern data
			//Go through each pattern
			for (int i = 0; i < numberOfPatterns + 1; i++)
			{
				//Go through each division
				for (int j = 0; j < 64; j++)
				{
					//Go through each channel's data
					for (int k = 0; k < 4; k++)
					{
						//Read the 4 bytes of data
						fread(&readBuffer, 1, 4, moduleFile);
						totalReadCount += readCount;

						//Check to see if we actually read enough bytes
						if (readCount != 4)
						{
							//This file is way to small to be a proper wav file
							fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
							return -12;
						}

						//Parse out the data.
						//Copy the first two bytes over into period
						patterns[i].division[j].channels[k].period = ((readBuffer[0] & 0x0F) << 8) | (readBuffer[1]);

						//Copy the second two bytes over into effect
						patterns[i].division[j].channels[k].effect = ((readBuffer[2] & 0x0F) << 8) | (readBuffer[3]);

						//Bit mask unneeded bits, shift, and add them together
						patterns[i].division[j].channels[k].sample = (readBuffer[0] & 0xF0) | ((readBuffer[2] & 0xF0) >> 4);
					}
				}
			}

			//DEBUG: Check current read count
			fprintf(stderr, "DEBUG:  Current Read Count: %d \n", totalReadCount);

			//Load up the samples with their data
			for (int i = 0; i < 31; i++)
			{
				if (samples[i].lengthInWords > 1)
				{
					//Create some memory to store the sample in.
					samples[i].data = new char[samples[i].lengthInWords * 2];

					//Read the data in there
					readCount = fread(samples[i].data, 1, samples[i].lengthInWords * 2, moduleFile);
					totalReadCount += readCount;

					//Check to see if we actually read enough bytes
					if (readCount != samples[i].lengthInWords * 2)
					{
						//This file is way to small to be a proper wav file
						fprintf(stderr, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.  Sample Data Reported: %d  Sample Data Read: %d \n", targetFilename, samples[i].lengthInWords * 2, readCount);
						//return -13;
					}
				}
			}

			//DEBUG: Check current read count
			fprintf(stderr, "DEBUG:  Current Read Count: %d \n", totalReadCount);

			//Close the file out
			fclose(moduleFile);

			//If we get to this point, everything is okay
			return 0;
		}


		short* ModuleFile::ConvertSample(unsigned char sample)
		{
			//Check for a valid sample
			if (sample > 32)
			{
				return nullptr;
			}

			//Create a buffer to store the converted samples into.
			short* temp = new short[samples[sample].lengthInWords * 2];

			if (samples[sample].lengthInWords > 1)
			{
				//Go through the module sample and convert the 8-bit to 16-bit range
				//Store it in the destinationBuffer
				for (int i = 0; i < (samples[sample].lengthInWords * 2); i++)
				{
					temp[i] = short(samples[sample].data[i] << 8);
				}
			}

			return temp;
		}

		unsigned int ModuleFile::ConvertSampleSize(unsigned char sample)
		{
			//Check for a valid sample
			if (sample > 32)
			{
				return 0;
			}

			return samples[sample].lengthInWords * 2;
		}

		//
		//
		//  Module File Player
		//
		//

		bool ModulePlayer::Load(char * filename)
		{
			//Make sure we have a new module file
			modFile = ModuleFile();

			//Load it up.
			modFile.LoadFile(filename);

			//If we get here, stuff is okay.
			return true;
		}

		bool ModulePlayer::Connect(unsigned int startChannel, unsigned int startSample)
		{
			//Check to see if we have a valid start position when it comes to channel mapping
			if (startChannel > (MAX_CHANNELS - 4))
			{
				return false;
			}

			//Check to see if we have a valid start position when it comes to sample mapping
			if (startSample > (MAX_SAMPLE_BUFFERS - 31))
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
					temp = new short[modFile.ConvertSampleSize(i)];

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
			channelMap[0]->Pan =  0.50f;
			channelMap[1]->Pan = -0.50f;
			channelMap[2]->Pan = -0.50f;
			channelMap[3]->Pan =  0.50f;

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
			unsigned 

			int positionToJumpAfterDivision = -1;

			//Default for most mods, can be changed.
			ticksADivision = DEFAULT_TICKS_A_DIVISION;

			std::chrono::time_point<std::chrono::steady_clock> startTime;
			std::chrono::nanoseconds deltaTime = std::chrono::nanoseconds(0);

			//
			//  Start playback processing
			//
			while (PlayerThreadActive)
			{
				fprintf(stderr, "DEBUG: Mod Player: Starting to play: %s\n", modFile.header.title);
				fprintf(stderr, "DEBUG: Mod Player: Song Positions: %d\n", modFile.header.songPositions);

				for (int j = 0; j < modFile.header.songPositions && PlayerThreadActive; j++)
				{
					CurrentPosition = j;

					//Pull the current pattern from the current position
					CurrentPattern = modFile.header.patternTable[j];

					//Process through the divisions
					for (int i = 0; i < 64 && PlayerThreadActive; i++)
					{
						//Save the timer to help time the processing time for this division
						startTime = std::chrono::steady_clock::now();

						//Set the current division
						CurrentDivision = i;

						//Set up the channels
						fprintf(stderr, "DEBUG: Mod Player: %s - Pattern: %d - Division: %d - Previous Time: %lld\n", modFile.header.title, j, i, deltaTime.count());

						//
						//  Check all the channels for any changes
						//
						for (int c = 0; c < 4; c++)
						{
							//
							//  Check for sample changes on each channel
							//

							//Check to see if sample is not zero
							//If it is zero don't change the sample used in the channel
							if (modFile.patterns[CurrentPattern].division[i].channels[c].sample > 0)
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
								channelMap[c]->offsetIncrement = MOD_NTSC_TUNING / float(modFile.patterns[CurrentPattern].division[i].channels[c].period)
									/ float(SAMPLE_RATE) / 2.0f;
							}


							//
							//  Check for any effect chanages on the channel.
							//

							//Parse out the effect
							effectTypeOnChannel[c] = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x0F00) >> 8;
							effectXOnChannel[c]    = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x00F0) >> 4;
							effectYOnChannel[c]    = (modFile.patterns[CurrentPattern].division[i].channels[c].effect & 0x000F);

							//
							//Turn off any channel based rendering effects for now
							//If there's suppose to be an effect, it should be configured in the next heap of logic.
							//

							//Turn off Arpeggio
							channelMap[c]->EnableArpeggio = false;

							//Turn of Volume Slide
							channelMap[c]->EnableVolumeSlide = false;

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
									channelMap[c]->ArpeggioSampleInterval = MOD_DEFAULT_SAMPLES_TICK;

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

								//Configure Volume Slide or Effect 10 / 0xA
								case 0xA:
									//Set the number of samples that progress for each tick in the effect.
									channelMap[c]->VolumeSlideSampleInterval = MOD_DEFAULT_SAMPLES_TICK;

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

									//Not implemented effect
									default:
										fprintf(stderr, "DEBUG: Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);
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
									fprintf(stderr, "DEBUG: Mod Player - Unimplemented or Unknown effect detected! Effect: %d X: %d Y: %d\n", effectTypeOnChannel[c], effectXOnChannel[c], effectYOnChannel[c]);
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
							if (modFile.patterns[CurrentPattern].division[i].channels[c].sample != 0 || modFile.patterns[CurrentPattern].division[i].channels[c].period != 0)
							{
								//Play it
								channelMap[c]->Play();
							}
						}

						//Wait for the next division
						std::this_thread::sleep_for(std::chrono::nanoseconds(ticksADivision * MOD_DEFAULT_TICK_TIMING_NANO));

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









		//
		//
		//  Sound System Wave File Definitions
		//
		//

		//Default constructor for a SoundSystemWaveFile
		WaveFile::WaveFile()
		{
		}

		//Deconstructor for a SoundSystemWaveFile
		WaveFile::~WaveFile()
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
		int WaveFile::LoadFile(char* targetFilename)
		{
			FILE* soundFile;
			size_t readCount = 0;
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

			printf("DEBUG: Sound System Wave File: %s - Data Bytes Read: %zd\n", targetFilename, readCount * 2);

			//If we get here... It's all good!... Maybe... Hoepfully?

			//Close out the sound file
			fclose(soundFile);

			return 0;
		}
	}
}
