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
#include "include\SGE\system.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace Sound
	{
		//
		//  Sample Buffers Namespace
		//
		namespace Sample
		{
			//
			//  An array of buffer pointers
			//
			sampleType* Buffer[MAX_SAMPLE_BUFFERS] = { NULL };

			//
			//  Buffer sizes
			//
			unsigned int Size[MAX_SAMPLE_BUFFERS] = { 0 };

			//
			//  Repeat offets
			//
			unsigned int RepeatOffset[MAX_SAMPLE_BUFFERS] = { 0 };

			//
			//  Repeat Durations
			//
			unsigned int RepeatDuration[MAX_SAMPLE_BUFFERS] = { 0 };

			//
			//  Allocate memory to a buffer
			//
			int Allocate(unsigned int bufferNumber, unsigned int numberOfSamples)
			{
				//
				//  Check to make sure we've got a legit bufferNumber
				//
				if (bufferNumber >= MAX_SAMPLE_BUFFERS)
				{
					//
					//  No such buffer, try again
					//
					return -1;
				}

				//
				//  Attempt to free anything that could be there
				//  If it's NULL, free won't do anything
				//
				std::free(Buffer[bufferNumber]);

				//
				//  Allocate some memory
				//  Cast the void pointer to a sampletype pointer
				//
				Buffer[bufferNumber] = (sampleType*) std::malloc(numberOfSamples * sizeof(sampleType));

				//
				//  Set the buffer size
				//
				Size[bufferNumber] = numberOfSamples;

				//
				//  Should be good, return 0
				//
				return 0;
			}

			//
			//  Load data to a buffer
			//
			int Load(unsigned int bufferNumber, unsigned int numOfSamples, sampleType *samples)
			{
				//
				//  Check to make sure we've got a legit bufferNumber
				//
				if (bufferNumber >= MAX_SAMPLE_BUFFERS)
				{
					//
					//  No such buffer, try again
					//
					return -1;
				}

				//
				//  Allocate the buffer
				//
				Allocate(bufferNumber, numOfSamples);


				//
				//  Check the source samples
				//
				if (samples == NULL)
				{
					//
					//  Okay, really?  There's nothing to copy!
					//
					return -3;
				}

				//
				//  Attempt to copy all this stuff
				//
				std::memcpy(Buffer[bufferNumber], samples, numOfSamples * sizeof(sampleType));

				//
				//  Should be good to go, return 0
				//
				return 0;
			}

			//
			//  Zero out a buffer completely
			//
			int Zero(unsigned int bufferNumber)
			{
				//
				//  Check to make sure we've got a legit bufferNumber
				//
				if (bufferNumber >= MAX_SAMPLE_BUFFERS)
				{
					//
					//  No such buffer, try again
					//
					return -1;
				}

				//
				//  Is there anything in this buffer
				//
				if (Buffer[bufferNumber] == NULL)
				{
					//
					//  There's nothing here, it's NULL
					//
					return -1;
				}

				//
				//  Zero out the buffer
				//
				std::memset(Buffer[bufferNumber], 0, Size[bufferNumber] * sizeof(sampleType));

				//
				//  Should be good, return 0
				//
				return 0;
			}

			//
			//  Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
			//
			int Free(unsigned int bufferNumber)
			{
				//
				//  Check to make sure we've got a legit bufferNumber
				//
				if (bufferNumber >= MAX_SAMPLE_BUFFERS)
				{
					//
					//  No such buffer, try again
					//
					return -1;
				}

				//
				//  Attempt to free the buffer
				//
				free(Buffer[bufferNumber]);

				//
				//  Be sure to set the freed pointer to NULL
				//
				Buffer[bufferNumber] = NULL;

				//
				//  Should be good, return 0
				//
				return 0;
			}

			//
			//  Flush the system out
			//
			void Flush()
			{
				//
				//  Purge out all the sample buffers
				//

				for (int i = 0; i < MAX_SAMPLE_BUFFERS; i++)
				{
					//
					//  Free doesn't do anything on 
					//
					free(Buffer[i]);

					//
					//  Assign the buffer point to NULL
					//
					Buffer[i] = NULL;
				}

				//
				//  All done, at least in theory.
				//
				return;
			}
		}






		//
		//
		//  Sound Channel Definitions
		//
		//

		//Given arguments, render a number of samples asked and return a pointer to the buffer.
		void Channel::Render(unsigned int numberOfSamples, int* sampleBuffer)
		{
			unsigned int currentSampleAverage = 0;

			//Starting at the offset, copy over samples to the buffer.
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//If we are currently not playing
				//And there's actually something to play.
				if (!Playing || Sample::Size[currentSampleBuffer] == 0 || Sample::Buffer[currentSampleBuffer] == NULL)
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
					sampleBuffer[i] = int((Sample::Buffer[currentSampleBuffer])[(unsigned int)offset] *  Volume);

					//Add to the acculumator for the average
					//Negate negatives since we are only interested in overall amplitude
					currentSampleAverage += abs(sampleBuffer[i]);

					//
					//  Calculate the next offset based on the appropriate increment
					//

					//
					//  Check to see if the Arpeggio Effect is in effect
					//
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
							if (arpeggioState != 0)
							{
								//Caclulate the semitone using the current offset
								//Check to see if state 1 or 2 and use X and Y values accordingly
								currentOffsetIncrement = float(offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, arpeggioState == 1 ? arpeggioSemitoneX : arpeggioSemitoneY));
							}
							else
							{
								currentOffsetIncrement = offsetIncrement;
							}
						}

						//Advance the number of samples
						currentArpeggioSamples++;
					}
					else
					{
						currentOffsetIncrement = offsetIncrement;
					}

					//
					//  Check for Vibrato Effect
					//
					if (EnableVibrato)
					{
						//Calculate the offset
						currentOffsetIncrement = currentOffsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, VibratoAmplitude * VibratoWaveform[currentVibratoWaveformPosition]);

						//Increment the vibrato waveform position
						currentVibratoWaveformPosition = (int)(currentVibratoWaveformPosition + VibratoCycles) % SAMPLE_RATE;
					}

					//
					//  Check for the Volume Slide effect
					//
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

					//For the Arpeggio Effect
					offset += currentOffsetIncrement;

					//
					//  Check for repeating loops
					//

					//Check to see if this same is suppose to repeat and is set to do so... correctly
					if ((Sample::RepeatDuration[currentSampleBuffer] > 0) && ((unsigned int)offset >= (Sample::RepeatOffset[currentSampleBuffer] + Sample::RepeatDuration[currentSampleBuffer])))
					{
						//Rewind back by the repeatDuration.
						offset -= Sample::RepeatDuration[currentSampleBuffer]; 
					}

					//
					//  Check to see if it's gone pass the valid bufferSize
					//

					//If not repeatable and the offset has gone past the end of the buffer
					if ((unsigned int)offset >= Sample::Size[currentSampleBuffer])
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
		void Channel::Play()
		{
			//If the channel is at least loaded
			if (currentSampleBuffer != MAX_SAMPLE_BUFFERS)
			{
				//Reset the play offset
				offset = 0;

				//Set the flags properly
				Playing = true;
			}
		}

		//Stop the currently playing audio
		void Channel::Stop()
		{
			//If the channel at least is loaded with some data
			if (currentSampleBuffer != MAX_SAMPLE_BUFFERS)
			{
				//Set the flags
				Playing = false;

				//Reset the playing offset
				offset = 0;
			}
		}


		//
		//
		// Waveforms
		//
		//

		namespace Waveforms
		{
			float Sine[SAMPLE_RATE] = { 0 };
			float RampDown[SAMPLE_RATE] = { 0 };
			float Square[SAMPLE_RATE] = { 0 };

			void PreGenerateSine()
			{
				//Calculate radian to frequency step for a 1Hz full sine wave
				float periodStepping = (Precalculated::TWO_PI_FLOAT) / SAMPLE_RATE;

				//Calculate all the points
				for (unsigned int i = 0; i < SAMPLE_RATE; i++)
				{
					//Put the data in the Sine waveform array
					Sine[i] = sinf(periodStepping * i);
				}
			}

			void PreGenerateRampDown()
			{
				//Calculate all the points
				for (unsigned int i = 0; i < SAMPLE_RATE; i++)
				{
					//Put all the data in the Ramp Down waveform array
					RampDown[i] = 1.0f - (i / float(SAMPLE_RATE / 2));
				}
			}

			void PreGenerateSquare()
			{
				//Calculate all the points
				//First part
				for (unsigned int i = 0; i < (SAMPLE_RATE / 2); i++)
				{
					//Put all the data in the Squrre waveform array.
					Square[i] = 1.0f;
				}

				//Second part
				for (unsigned int i = (SAMPLE_RATE / 2); i < SAMPLE_RATE; i++)
				{
					Square[i] = -1.0f;
				}
			}

			void Derive16BitSine(short* targetBuffer, unsigned int bufferLength, float frequency)
			{

			}
		}


		//
		//
		//  Sound System Core Components
		//
		//

		//
		//Varibles to hold average volume levels from the last callback from PortAudio
		//
		unsigned int MasterVolumeAverageLeftLevel = 0;
		unsigned int MasterVolumeAverageRightLevel = 0;

		//
		//  All the system's sound channels
		//
		Channel Channels[MAX_CHANNELS];

		//
		//  Initialize size for the Render Frame Buffers
		//
		const unsigned long INITIAL_RENDER_FRAME_BUFFER_SIZE = 1024;

		//
		//  Master volume for system
		//
		float MasterVolume = 1.0f;

		//
		//All the sound channel target render buffers
		//
		int* renderedChannelBuffers[MAX_CHANNELS] = { nullptr };

		//
		//32-bit mixing buffers
		//
		int* mixingFrameBufferRight = nullptr;		//Mixing buffer for Right Channel
		int* mixingFrameBufferLeft  = nullptr;		//Mixing buffer for Left Channel

		//
		//Current Frame Buffer Sizes
		//
		unsigned long frameBufferSize = 0;

		//
		//  Sound Buffer Functions
		//

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

		//
		//
		//  Port Audio Components
		//
		//

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
			sampleType* outputBufferLeft =  ((sampleType**)output)[0];
			sampleType* outputBufferRight = ((sampleType**)output)[1];


			//Check to see if the frame buffer size is bigger than the frame buffers already allocated
			if (frameCount > frameBufferSize)
			{
				DeleteFrameBuffers();
				GenerateFrameBuffers(frameCount);
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::Sound, "DEBUG:  Sound  System - Frame Buffer Size Increased to: %lu\n", frameBufferSize);

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


		//
		//
		//  Sound System Core Functions
		//
		//

		//Start the audio system.
		void Start()
		{
			//Initialize PortAudio
			portAudioError = Pa_Initialize();

			//Check for any errors
			if (portAudioError != paNoError)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}

			//Setup Stream parameters
			//Grab the default output device
			outputParameters.device = Pa_GetDefaultOutputDevice();

			//If there is not default output device, Error
			if (outputParameters.device == paNoDevice)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: No default output device.\n");
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
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
			else
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "PortAudio Stream Opened\n");
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
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
				}
				else
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "PortAudio Stream Started.\n");
				}
			}

			//
			//Generate PreGenerated Base Waveforms.
			//

			Waveforms::PreGenerateSine();
			Waveforms::PreGenerateRampDown();
			Waveforms::PreGenerateSquare();

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
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
				}
				else
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "PortAudio Stream Stopped.\n");
				}
			}

			//Close the PortAudio stream
			portAudioError = Pa_CloseStream(soundSystemStream);

			if (portAudioError != paNoError)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
			else
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "PortAudio Stream Closed.\n");
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
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "PortAudio Error: %s\n", Pa_GetErrorText(portAudioError));
			}
		}
	}
}
