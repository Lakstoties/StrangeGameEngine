#include "include\SGE\sound.h"
#include <portaudio.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <random>
#include <fstream>

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
			//Starting at the offset, copy over samples to the buffer.
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//If we are currently not playing
				if (!Playing)
				{
					sampleBuffer[i] = 0;
				}
				else
				{
					//Copy the sample from the source buffer to the target buffer and adjusted the volume.
					sampleBuffer[i] = int (currentSampleBuffer->buffer[unsigned int (offset)] * Volume);

					//Increment to the next offset
					offset += offsetIncrement;

					//Check to see if this same is suppose to repeat and is set to do so... correctly
					if (Repeatable && repeatDuration > 0 && unsigned int(offset) >= (repeatOffset + repeatDuration))
					{
						//Alter the offset appropriate, keeping in mind how much we have blown past end point to add the difference in to keep looping proper.
						offset = float(repeatOffset) + (offset - float(repeatOffset + repeatDuration));
					}

					//If not repeatable and the offset has gone past the end of the buffer
					else if (unsigned int (offset) >= currentSampleBuffer->bufferSize)
					{
						//Fuck this shit we're out!
						Stop();
					}
				}
			}
		}


		//Function signals the channel is playing, so it will be rendered.
		void SoundChannel::Play()
		{
			//If the channel is at least loaded
			if (currentSampleBuffer != nullptr)
			{
				//Set the flags properly
				Playing = true;

				//Reset the play offset
				offset = 0;
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

			//malloc some ram for the buffer
			buffer = (short*)malloc(sizeof(short) * bufferSize);

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
			memcpy(buffer, samples, sizeof(short) * bufferSize);

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
			memset(buffer, 0, sizeof(short) * bufferSize);

			//Everything happened okay in theory
			return 0;
		}

		//Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
		int SoundSampleBuffer::ResetBuffer()
		{
			//Check to make sure there's actually a buffer to free
			if (buffer == nullptr)
			{
				//Hey, there's no buffer to free!
				return -1;
			}

			//If there's a buffer there...
			//Free it
			free(buffer);

			//Null the point out
			buffer = nullptr;

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
			if (mixingFrameBufferLeft == nullptr && mixingFrameBufferRight == nullptr)
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
		}

		//Deletes and cleans up Frame buffers that were holding audio data
		void DeleteFrameBuffers()
		{
			if (mixingFrameBufferLeft != nullptr && mixingFrameBufferRight != nullptr)
			{
				//Delete old render frame buffers
				for (int i = 0; i < MAX_CHANNELS; i++)
				{
					free(renderedChannelBuffers[i]);
					renderedChannelBuffers[i] = nullptr;
				}

				//Delete old mixing frame buffers
				free(mixingFrameBufferLeft);
				free(mixingFrameBufferRight);

				//Reset the pointer values to nullptr
				mixingFrameBufferLeft = nullptr;
				mixingFrameBufferRight = nullptr;
			}
		}

		//Clears and zeros out data in the mixing buffers
		void ClearMixingBuffers()
		{
			if (mixingFrameBufferLeft != nullptr && mixingFrameBufferRight != nullptr)
			{
				//Initialize the mixing buffers
				//Depending on the platform, possibly not needed, but some platforms don't promise zeroed memory upon allocation.
				memset(mixingFrameBufferLeft, 0, frameBufferSize * sizeof(int));
				memset(mixingFrameBufferRight, 0, frameBufferSize * sizeof(int));
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
				fprintf(stderr, "DEBUG:  Sound  System - Frame Buffer Size Increased to: %i\n", frameBufferSize);
			}

			//Mix the samples into buffer
			//Go through each channel and add it's contents to the mixing buffer
			for (int i = 0; i < MAX_CHANNELS; i++)
			{
				Channels[i].Render(frameCount, renderedChannelBuffers[i]);
				for (unsigned int j = 0; j < frameCount; j++)
				{
					mixingFrameBufferLeft[j]  += int (renderedChannelBuffers[i][j] * (0.5f + Channels[i].Pan));
					mixingFrameBufferRight[j] += int (renderedChannelBuffers[i][j] * (0.5f - Channels[i].Pan));
				}
			}

			//Dump it into the output buffer
			//Adjust to master volume...
			//If the sample size exceeds the sample limit, just hard limit it.
			//Well... people...  Don't fry your damn audio.

			for (unsigned int i = 0; i < frameCount; i++)
			{
				//Apply master volume to the mixing frame buffers
				mixingFrameBufferLeft[i]  = int (mixingFrameBufferLeft[i] * MasterVolume);
				mixingFrameBufferRight[i] = int (mixingFrameBufferRight[i] * MasterVolume);

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
		//  Sound System Wave File Definitions
		//
		//

		//Default constructor for a SoundSystemWaveFile
		WaveFile::WaveFile()
		{
			audioData = nullptr;
			numberOfSamples = 0;
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