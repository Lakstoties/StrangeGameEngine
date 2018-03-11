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
		//
		//  Sound Channel Definitions
		//
		//

		//
		//  Given arguments, render a number of samples asked and return a pointer to the buffer.
		//
		void Channel::Render(unsigned int numberOfSamples, int* sampleBuffer)
		{
			//
			//  For statistical reasons
			//
			unsigned int currentSampleAverage = 0;

			//
			//  Starting at the offset, copy over samples to the buffer.
			//
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//
				//  If we are currently not playing and there's actually something to play.
				//
				if (!Playing || currentSampleBuffer->size == 0 || currentSampleBuffer->data == NULL)
				{
					//
					//  Nothing playing, 0 out the samples
					//
					sampleBuffer[i] = 0;
				}
				else
				{
					//
					//  Grab the sample for the offset
					//

					//
					//  Copy the sample from the source buffer to the target buffer and adjusted the volume.
					//  If the volume effect is in use, use that volume value.
					//
					sampleBuffer[i] = int(currentSampleBuffer->data[(unsigned int)offset] *  Volume);

					//
					//  Add to the acculumator for the average
					//  Negate negatives since we are only interested in overall amplitude
					//
					currentSampleAverage += abs(sampleBuffer[i]);

					//
					//  Calculate the next offset based on the appropriate increment
					//

					//
					//  Check for period slide effect
					//
					if (periodSlidEnabled)
					{
						//
						//  Once enough samples have been processed
						//
						if (periodSlideCurrentSamples >= periodSlideSampleInterval)
						{
							//
							//  Convert the offset increment to a period in relationship to 1 second
							//
							float offsetPeriod = 1 / offsetIncrement;

							//
							//  Add the period delta
							//
							offsetPeriod += periodSlideDelta;

							//
							//  Do we have a target period
							//
							if (periodTarget != 0)
							{
								//
								// Check to see if offsetPeriod is past target
								//

								if ((periodSlideDelta < 0 && offsetPeriod < periodTarget) ||
									(periodSlideDelta > 0 && offsetPeriod > periodTarget))
								{
									offsetPeriod = periodTarget;
									periodSlidEnabled = false;
								}
							}

							//
							//  Flip it back to an offset increment
							//
							offsetIncrement = 1 / offsetPeriod;

							//
							//  Reset the counter
							//
							periodSlideCurrentSamples %= periodSlideSampleInterval;
						}

						periodSlideCurrentSamples++;
					}






					//
					//  Check to see if the Arpeggio Effect is in effect
					//
					if (arpeggioEnabled)
					{
						//
						//  Check the state of the Arpeggio effect
						//
						if (arpeggioCurrentSamples > arpeggioSampleInterval)
						{
							//
							//  Reset and roll the counter
							//
							arpeggioCurrentSamples %= arpeggioSampleInterval;

							//
							//  Transition the state
							//  Increment and then modulus to the states around.
							//
							++arpeggioState %= 3;
							
							//
							//  Based on that state alter the argpeggio offset increment
							//
							switch (arpeggioState)
							{
								//
								//  If at default state, just use the regular offsetIncrement
								//
							case 0:
								currentOffsetIncrement = offsetIncrement;
								break;

								//
								//  If at state 1, calculate the increment for X semitones
								//
							case 1:
								currentOffsetIncrement = float(offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, arpeggioSemitoneX));
								break;

								//
								//  If at state 2, calculate the increment for X semitones
								//
							case 2:
								currentOffsetIncrement = float(offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, arpeggioSemitoneY));
								break;
							}
						}

						//
						//  Advance the number of samples
						//
						arpeggioCurrentSamples++;
					}
					else
					{
						currentOffsetIncrement = offsetIncrement;
					}





					//
					//  Check for Vibrato Effect
					//
					if (vibratoEnabled)
					{
						//Calculate the offset
						currentOffsetIncrement = currentOffsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, vibratoAmplitude * vibratoWaveform[vibratoCurrentWaveformPosition]);

						//Increment the vibrato waveform position
						vibratoCurrentWaveformPosition = (int)(vibratoCurrentWaveformPosition + vibratoCycles) % SAMPLE_RATE;
					}

					//
					//  Check for the Volume Slide effect
					//
					if (volumeSlideEnabled)
					{
						//Check the state of Volume Slide Effect
						if (volumeSlideCurrentSamples > volumeSlideSampleInterval)
						{
							//Reset and roll the counter
							volumeSlideCurrentSamples %= volumeSlideSampleInterval;

							//Apply the slide
							Volume += volumeSlideRate;

							//Check to make sure it did not exit normal ranges
							//Short circuit logic
							(Volume < 0.0f) && (Volume = 0.0f);
							(Volume > 1.0f) && (Volume = 1.0f);
						}
						volumeSlideCurrentSamples++;
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
					if ((currentSampleBuffer->repeatDuration > 0) && ((unsigned int)offset >= (currentSampleBuffer->repeatOffset + currentSampleBuffer->repeatDuration)))
					{
						//Rewind back by the repeatDuration.
						offset -= currentSampleBuffer->repeatDuration; 
					}

					//
					//  Check to see if it's gone pass the valid bufferSize
					//

					//If not repeatable and the offset has gone past the end of the buffer
					if ((unsigned int)offset >= currentSampleBuffer->size)
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
			if (currentSampleBuffer != nullptr)
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

		//
		//  Allocate a blank buffer of a certain sample size
		//
		bool SampleBuffer::Allocate(unsigned int numOfSamples)
		{
			//
			//  Delete the previous buffer, if it exists
			//
			Delete();

			//
			//  Set the buffer size
			//
			size = numOfSamples;

			//
			//  Get an array to put in place
			//
			data = new sampleType[size];

			//
			//  Zero out the buffer to make sure it is clean
			//  Some OSes don't make the ram is clean when given
			//
			Zero();

			//
			//  Everything thing should be okay.
			//
			return true;
		}

		//
		//  Create a blank buffer, and then load data into it.
		//
		bool SampleBuffer::Load(unsigned int numOfSamples, sampleType *samples)
		{
			//
			//  Get a clean buffer
			//
			Allocate(numOfSamples);

			//
			//  memcpy over the data into the buffer
			//
			std::memcpy(data, samples, sizeof(sampleType) * size);

			//
			//  Everything should have gone okay...
			//
			return true;
		}

		//
		//  Zero out a buffer completely
		//
		bool SampleBuffer::Zero()
		{
			//
			//  Check to make sure there's actually a buffer to zero out
			//
			if (data == NULL)
			{
				//Hey, there's no buffer to zero out!
				return false;
			}

			//
			//  Otherwise memset the bitch.
			//
			std::memset(data, 0, sizeof(sampleType) * size);

			//
			//  Everything happened okay in theory
			//
			return true;
		}

		//
		//  Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
		//
		void SampleBuffer::Delete()
		{
			//
			//  Delete the buffer
			//
			delete data;

			//
			//  Set the pointer back to NULL
			//
			data = NULL;

			//
			//  Reset the buffer size
			//
			size = 0;
		}

		//
		//  Destructor - Makes sure any memory assigned to the data pointer is deleted and sent back to the OS
		//
		SampleBuffer::~SampleBuffer()
		{
			//
			//  Reset the buffer which will free the memory.
			//
			Delete();
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
		//  All the sound samples in the system
		//
		SampleBuffer SampleBuffers[Sound::MAX_SAMPLE_BUFFERS];

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
			//std::memset(mixingFrameBufferLeft,  0, sizeof(int) * frameBufferSize);
			//std::memset(mixingFrameBufferRight, 0, sizeof(int) * frameBufferSize);

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
				//
				//  Down mix all the samples from all the channels
				//

				//
				//  Channel 0 always initializes the first value in the buffer
				//  This saves us from having to memset the buffers
				//
				mixingFrameBufferLeft[i] =	int(renderedChannelBuffers[0][i] * (0.5f + Channels[0].Pan));
				mixingFrameBufferRight[i] =	int(renderedChannelBuffers[0][i] * (0.5f - Channels[0].Pan));

				//
				//  Starts at channel 1 and goes from there.
				//
				for (unsigned int j = 1; j < MAX_CHANNELS; j++)
				{
					mixingFrameBufferLeft[i]  += int(renderedChannelBuffers[j][i] * (0.5f + Channels[j].Pan));
					mixingFrameBufferRight[i] += int(renderedChannelBuffers[j][i] * (0.5f - Channels[j].Pan));
				}

				//  Apply master volume to the mixing frame buffers
				mixingFrameBufferLeft[i]  = int (mixingFrameBufferLeft[i]  * MasterVolume);
				mixingFrameBufferRight[i] = int (mixingFrameBufferRight[i] * MasterVolume);

				//
				//  Check for things hitting the upper and lower ends of the range
				//

				//
				//  Note about the calculation:  Integer divide the sample in the mixing frame buffer by the maximum allowable sample amount.
				//  If it's anything other than zero, check to see if it is negative for not, and write the proper max sample limit
				//  Otherwise, just write the sample over.
				//

				//  For the left channel
				outputBufferLeft[i] = (mixingFrameBufferLeft[i] / SAMPLE_MAX_AMPLITUDE) ? ( mixingFrameBufferLeft[i] > 0 ? SAMPLE_MAX_AMPLITUDE : -SAMPLE_MAX_AMPLITUDE): mixingFrameBufferLeft[i];

				//  For the right channel
				outputBufferRight[i] = (mixingFrameBufferRight[i] / SAMPLE_MAX_AMPLITUDE) ? (mixingFrameBufferRight[i] > 0 ? SAMPLE_MAX_AMPLITUDE : -SAMPLE_MAX_AMPLITUDE) : mixingFrameBufferRight[i];

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
				NULL,
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
