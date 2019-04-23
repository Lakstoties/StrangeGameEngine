#include "include\SGE\sound.h"
#include <RtAudio.h>
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
		//  Sound Channel Definitions
		//

		//
		//  Method to Apply the Areggio Effect
		//
		float Channel::ArpeggioEffect::Apply(float offsetIncrement)
		{
			//  Check to see if the Arpeggio Effect is in effect
			if (Enabled)
			{
				//  Check the state of the Arpeggio effect
				if (CurrentSamples > SampleInterval)
				{
					//  Reset and roll the counter
					CurrentSamples %= SampleInterval;

					//  Transition the state
					//  Increment and then modulus to the states around.
					State = (State + 1) % 3;
				}

				//  Based on that state alter the argpeggio offset increment
				if (State == 1)
				{
					//  If at state 1, calculate the increment for X semitones
					offsetIncrement = float(offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, SemitoneX));
				}

				else if (State == 2)
				{
					//  If at state 2, calculate the increment for Y semitones
					offsetIncrement = float(offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, SemitoneY));
				}

				//  Advance the number of samples
				CurrentSamples++;
			}

			// Return the new offset
			return offsetIncrement;
		}

		//
		//  Method to Apply the Vibrato Effect
		//
		float Channel::VibratoEffect::Apply(float offsetIncrement)
		{
			if (Enabled)
			{
				//Calculate the offset
				offsetIncrement = offsetIncrement * pow(Precalculated::SEMITONE_MULTIPLIER, Amplitude * Waveform[CurrentWaveformPosition]);

				//Increment the vibrato waveform position
				CurrentWaveformPosition = (int)(CurrentWaveformPosition + Cycles) % SAMPLE_RATE;
			}

			return offsetIncrement;
		}

		//
		//  Method to Apply the Period Slide Effect
		//
		float Channel::PeriodSlideEffect::Apply(float offsetIncrement)
		{
			if (Enabled)
			{
				//  Once enough samples have been processed
				if (CurrentSamples >= SampleInterval)
				{
					//  Convert the offset increment to a period in relationship to 1 second
					float offsetPeriod = 1 / offsetIncrement;

					//  Add the period delta
					offsetPeriod += Delta;

					//  Do we have a target period
					if (Target != 0)
					{
						// Check to see if offsetPeriod is past target

						if ((Delta < 0 && offsetPeriod < Target) ||
							(Delta > 0 && offsetPeriod > Target))
						{
							offsetPeriod = Target;
							Enabled = false;
						}
					}

					//  Flip it back to an offset increment
					offsetIncrement = 1 / offsetPeriod;

					//  Reset the counter
					CurrentSamples %= SampleInterval;
				}

				CurrentSamples++;
			}

			return offsetIncrement;
		}

		//
		//  Method to Apply the Volume Slide Effect
		//
		float Channel::VolumeSlideEffect::Apply(float currentVolume)
		{
			//  Check for the Volume Slide effect
			if (Enabled && SampleInterval != 0)
			{
				//Check the state of Volume Slide Effect
				if (CurrentSamples > SampleInterval)
				{
					//Reset and roll the counter
					CurrentSamples %= SampleInterval;

					//Apply the slide
					currentVolume += Rate;

					//Check to make sure it did not exit normal ranges
					//Short circuit logic
					if (currentVolume < 0.0f)
					{
						currentVolume = 0.0f;
					}
					else if (currentVolume > 1.0f)
					{
						currentVolume = 1.0f;
					}
				}
				CurrentSamples++;
			}

			return currentVolume;
		}

		//
		//  Method to Apply the Retrigger Effect
		//
		float Channel::RetriggerEffect::Apply(float offset)
		{
			//  Check to see if we need to trigger the sample
			if (Enabled)
			{
				//  Once enough samples have been processed
				if (CurrentSamples >= SampleInterval)
				{
					//  Set the sample offset back to the designated destination
					offset = (float)SampleDestination;

					//  Reset the counter
					CurrentSamples %= SampleInterval;
				}

				//  Increment to the next sample
				CurrentSamples++;
			}
			
			return offset;
		}

		float Channel::CutEffect::Apply(float currentVolume)
		{
			//  Check to see if we need to cut the sample.
			if (Enabled)
			{
				//  Once enough samples have been processed
				if (CurrentSamples >= SampleInterval)
				{
					//  Zero out volume
					currentVolume = 0.0f;

					//  No need to check anymore
					Enabled = false;
				}

				CurrentSamples++;
			}

			return currentVolume;
		}

		//  Given arguments, render a number of samples asked to the given vector.
		void Channel::Render(unsigned int numberOfSamples, renderSampleType* sampleBuffer)
		{
			//  For statistical reasons
			unsigned int currentSampleAverage = 0;

			//  Starting at the offset, copy over samples to the buffer.
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//  If we are currently not playing and there's actually something to play.
				if (!Playing || SGE::Sound::SampleBuffers[CurrentSampleBuffer].Size == 0 || !CurrentSampleBufferSet)
				{
					//  Nothing playing, 0 out the samples
					sampleBuffer[i] = 0;
				}
				else
				{	
					//  Check to see if we are getting delayed
					if (Delay.Enabled)
					{
						//  Check to see if we've delayed enough
						if (Delay.CurrentSamples >= Delay.SampleInterval)
						{
							//  We're done!
							Delay.Enabled = false;
						}

						//  Increment the delay
						Delay.CurrentSamples++;
					}

					//  Continue life as normal
					else
					{
						//  Grab the sample for the offset
						//  Copy the sample from the source buffer to the target buffer and adjusted the volume.
						//  If the volume effect is in use, use that volume value.
						sampleBuffer[i] = int(SGE::Sound::SampleBuffers[CurrentSampleBuffer].Samples[(unsigned int)offset] * Volume);

						//  Add to the acculumator for the average
						//  Negate negatives since we are only interested in overall amplitude
						currentSampleAverage += abs(sampleBuffer[i]);

						//
						//  Calculate the next offset based on the appropriate increment
						//

						//
						//  Apply Volume Slide Effect
						//
						Volume = VolumeSlide.Apply(Volume);

						//
						//  Apply Cut Effect
						//
						Volume = Cut.Apply(Volume);

											   						 
						//
						//  Effect Pipeline, that affects offset Increments
						//

						//  Apply Retrigger Effect
						//  This effect changes the offset
						offset = Retrigger.Apply(offset);

						//  Apply Period Slide Effect, if active
						//  This changes the offsetIncrement for the channel, hence it write it
						offsetIncrement = PeriodSlide.Apply(offsetIncrement);

						//  Apply the Arpeggio Effect, if active
						currentOffsetIncrement = Arpeggio.Apply(offsetIncrement);

						//  Apply the Vibrato Effect, if active

						currentOffsetIncrement = Vibrato.Apply(currentOffsetIncrement);

						//  Increment Offset  Appropriately
						offset += currentOffsetIncrement;

						//  Check for repeating loops

						//Check to see if this same is suppose to repeat and is set to do so... correctly
						if ((SGE::Sound::SampleBuffers[CurrentSampleBuffer].RepeatDuration > 0) && ((unsigned int)offset >= (SGE::Sound::SampleBuffers[CurrentSampleBuffer].RepeatOffset + SGE::Sound::SampleBuffers[CurrentSampleBuffer].RepeatDuration)))
						{
							//Rewind back by the repeatDuration.
							offset -= SGE::Sound::SampleBuffers[CurrentSampleBuffer].RepeatDuration;
						}

						//  Check to see if it's gone pass the valid bufferSize

						//If not repeatable and the offset has gone past the end of the buffer
						if ((int)offset >= SGE::Sound::SampleBuffers[CurrentSampleBuffer].Size)
						{
							//Fuck this shit we're out!
							Stop();
						}
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
			if (CurrentSampleBufferSet && SGE::Sound::SampleBuffers[CurrentSampleBuffer].Size != 0)
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
			//Set the flags
			Playing = false;

			//Reset the playing offset
			offset = 0;
		}


		//
		//  Set Sample Buffer
		//
		void Channel::SetSampleBuffer(int bufferNumber)
		{
			if (bufferNumber >= 0 && bufferNumber < MAX_SAMPLE_BUFFERS)
			{
				CurrentSampleBuffer = bufferNumber;
				CurrentSampleBufferSet = true;
			}
		}

		//
		//
		// Sound Sample Buffer Defintions
		//
		//

		//
		//  Create a blank buffer
		//
		void SampleBuffer::Create(int numOfSamples)
		{
			//
			// Check for legitmate number of samples
			//
			if (numOfSamples < 1)
			{
				//Info the error
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "SampleBuffer Create Error: Number of sample requested was less than 1\n");
				return;
			}

			//
			// Purge any existing buffers
			//
			if (nullptr)
			{
				this->Delete();
			}

			//
			//  Create the buffer
			//

			// Set the sample size
			this->Size = numOfSamples;

			// Assign a new sample array
			this->Samples = new sampleType[Size];

			// Zero the contents
			this->Zero();
		}

		//
		//  Zero out a buffer
		//
		void SampleBuffer::Zero()
		{
			if (Size > 0 && Samples != nullptr)
			{
				std::fill(Samples, Samples + Size, 0);
			}
		}

		//
		//  Delete a buffer
		//
		void SampleBuffer::Delete()
		{
			// Check to see if there's anything allocated there
			if (Samples != nullptr)
			{
				// Delete the array of stuff
				delete[] Samples;

				// Assign nullptr to the pointer for record keeping
				Samples = nullptr;

				// Set the size back to default
				Size = 0;
			}
		}

		//
		//  Destructor for SampleBuffer
		//
		SampleBuffer::~SampleBuffer()
		{
			this->Delete();
		}

		//
		//  Load Raw Data Samples from a buffer of sampleType
		//
		void SampleBuffer::LoadRaw(int numOfSamples, sampleType* samples)
		{
			// Create the buffer
			this->Create(numOfSamples);

			// Copy stuff over
			std::copy(samples, samples + numOfSamples, Samples);
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

		const int AUDIO_RENDER_BUFFER_SIZE = 256;

		//
		//  RtAudio Components
		//

		RtAudio audioSystem;
		RtAudio::StreamParameters audioStreamParameters;
		RtAudio::StreamOptions audioStreamOptions;

		unsigned int RTAUDIO_RENDER_BUFFER_SIZE = AUDIO_RENDER_BUFFER_SIZE;

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
		//  Master volume for system
		//
		float MasterVolume = 1.0f;

		//
		//  All the sound channel target render buffers
		//
		renderSampleType renderedChannelBuffers[MAX_CHANNELS][AUDIO_RENDER_BUFFER_SIZE] = { 0 };

		//
		//32-bit mixing buffers
		//
		renderSampleType mixingFrameBufferRight[AUDIO_RENDER_BUFFER_SIZE] = { 0 };
		renderSampleType mixingFrameBufferLeft[AUDIO_RENDER_BUFFER_SIZE] = { 0 };

		//
		//Current Frame Buffer Sizes
		//
		unsigned long frameBufferSize = 0;


		


		//Static callback function wrapper to call the callback of the particular SoundSystem object passed.
		int AudioCallback(
			void *output,
			void *input,
			unsigned int frameCount,
			double streamTime,
			RtAudioStreamStatus status,
			void *userData)
		{
			//Statistics stuff
			unsigned int currentLeftAverageLevel = 0;
			unsigned int currentRightAverageLevel = 0;

			//Recast the output buffers
			sampleType* outputBufferLeft =  (sampleType*)output;
			sampleType* outputBufferRight = (sampleType*)output + frameCount;

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
				mixingFrameBufferLeft[i] =	int(renderedChannelBuffers[0][i] * (0.5f + Channels[0].Pan) * MasterVolume);
				mixingFrameBufferRight[i] =	int(renderedChannelBuffers[0][i] * (0.5f - Channels[0].Pan) * MasterVolume);

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

				//  For the right channel
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


				//Sum up the levels
				//Negate negative values since we are interested in overall amplitude and not phasing
				//For the left channel
				currentLeftAverageLevel += std::abs(outputBufferLeft[i]);

				//For the right channel
				currentRightAverageLevel += std::abs(outputBufferRight[i]);
			}

			//Report stats
			MasterVolumeAverageLeftLevel = currentLeftAverageLevel / frameCount;
			MasterVolumeAverageRightLevel = currentRightAverageLevel / frameCount;

			//Return
			//If no major errors, continue the stream.
			return 0;
		}


		//
		//  Sound System Core Functions
		//

		//Start the audio system.
		void Start()
		{
			if (audioSystem.getDeviceCount() < 1)
			{
				//Crap there's no audio devices!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "Error:  No Audio Devices detected!!!\n");
			}
			else
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "%i Audio Devices detected.\n", audioSystem.getDeviceCount());
			}
					   
			//
			//  Get the basic stream settings together
			//
			audioStreamParameters.deviceId = audioSystem.getDefaultOutputDevice();
			audioStreamParameters.nChannels = 2;
			audioStreamParameters.firstChannel = 0;
			
			//
			//  Tweak some options
			//
			audioStreamOptions.flags = RTAUDIO_NONINTERLEAVED;


			//
			//  Try to open up the audio device
			//
			try
			{
				audioSystem.openStream(&audioStreamParameters, NULL, RTAUDIO_SINT16, SGE::Sound::SAMPLE_RATE, &RTAUDIO_RENDER_BUFFER_SIZE, &AudioCallback, NULL, &audioStreamOptions);
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "Default Device Opened Successfully!\n");
			}
			catch (RtAudioError & e)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "Default Device Opened Unsuccessfully!!!\n");
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "Error Output From RtAudio Execept: %s\n", e.getMessage());
				e.printMessage();
			}
			
			//
			// Start the Audio Stream
			//

			SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "Audio Stream Started.\n");
			audioSystem.startStream();

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
			try
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "Attempting to Stop Audio Stream.\n");
				audioSystem.stopStream();
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "Audio Stream Stopped Successfully.\n");
			}
			catch (RtAudioError & e)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "Audio Stop Unsuccessful!!!\n");
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::Sound, "Error Output From RtAudio Execept: %s\n", e.getMessage());
				e.printMessage();
			}

			if (audioSystem.isStreamOpen())
			{
				audioSystem.closeStream();
				SGE::System::Message::Output(SGE::System::Message::Levels::Information, SGE::System::Message::Sources::Sound, "Audio Stream Closed.\n");
			}
		}
	}
}
