#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cmath>
#include <random>
#include <portaudio.h>
#include <fstream>


namespace SGE
{
	namespace Sound
	{
		//MIDI frequency note array
		extern const float MIDI_NOTE_FRENQUENCY[128];

		//Half PI to Float resolution
		extern const float HALF_PI_FLOAT;

		//PI to float resolution
		extern const float PI_FLOAT;

		//Two PI to float resolution
		extern const float TWO_PI_FLOAT;

		//Sound system sample rate
		const int SAMPLE_RATE = 44100;

		//Sound system bit depth
		const int SAMPLE_BITS = 16;

		//Maximum Amplitude given the sample bit depth
		//This is usually half the maximum number represented by the number of bits
		const int SAMPLE_MAX_AMPLITUDE = (1 << (SAMPLE_BITS - 1)) - 1;

		//Fixed point math bias
		const int FIXED_POINT_BIAS = 1000;

		//Max number of envelope entries
		const int MAX_ENVELOPE_ENTRIES = 10;

		//Number of channels for the system
		const int MAX_CHANNELS = 32;

		//Number of samples for the system
		const int MAX_SAMPLE_BUFFERS = 256;


		namespace Generators
		{
			//Sample Generation Tools

			//Generate a sine wave sample array
			short* SineGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

			//Generate a pulse wave sample array
			short* PulseGenerator(float hertz, float dutyCycle, unsigned int samplesToGenerate, short amplitude);

			//Generate a triangle wave sample array
			short* TriangleGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

			//Generate a sawtooth wave sample array
			short* SawtoothGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

			//Generate a noise wave sample array
			short* NoiseGenerator(unsigned int samplesToGenerate, short amplitude);
		}


		//Sample Buffer to contain audio data that sound channels will link to and play from.
		//For the purposes of the Strange Game Engine.  The data put into these buffers are assumed to be PCM Signed 16-bit at 44.1Khz sample rate.
		struct SoundSampleBuffer
		{
			//Buffer to contain the samples
			short *buffer = nullptr;

			//Size of the buffer
			unsigned int bufferSize = 0;
						
			//Create a blank buffer of a certain sample size
			int CreateBlankBuffer(unsigned int numOfSamples);

			//Create a blank buffer, and then load data into it.
			int LoadSoundBuffer(unsigned int numOfSamples, short *samples);

			//Zero out a buffer completely
			int ZeroBuffer();

			//Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
			int ResetBuffer();

			//Destructor to make sure the buffer memory is freed upon destruction to prevent memory leaks.
			~SoundSampleBuffer();			
		};



		struct SoundChannel
		{
		private:
			//Pitch bits
			float keyedPitch = 1.0;
			int currentPitch = 1;
			float targetPitch = 1.0;

			//Offset bits
			//Absolute sample offset
			unsigned int offset = 0;

			//Relative sample based timing offset for envelop calculations when the sample is being looped or pitched shifted
			unsigned int timingOffset = 0;

			//Amount to increment the offset by
			int offsetIncrement = 0;

		public:
			SoundChannel();
			~SoundChannel();

			//Current Sound Sample Buffer in use
			//Set to the maximum buffers, to indicate one hasn't been selected.
			 SoundSampleBuffer* currentSampleBuffer = nullptr;
			
			//Render functions
			void RenderSamples(unsigned int numerOfSamples, int* sampleBuffer);

			//Basic functions
			//Hard play
			//Will reset an already playing channel.
			//Will continue a paused channel.
			void Play();

			//Hard stop, will immediately stop a channel.
			void Stop();

			//Stops the channel, and holds the playback offset
			void Pause();

			//Trigger Functions
			//Soft play, if a channel is already playing, it will go to the Attack state.
			void Trigger();

			//Soft stop, will allow for a release decay from the sustain level to take place before hard stop.
			void Release();

			//Function for managing key
			void SetKey(float pitch);
			float GetKey();

			//Function for managing pitch
			void SetPitch(float pitch);
			float GetPitch();

			//Status State Flags
			static const int STATUS_NOT_PLAYING_STATE = 0;
			static const int STATUS_PLAYING_STATE = 2;
			static const int STATIS_PAUSED_STATE = 3;

			//Status State
			int statusState = STATUS_NOT_PLAYING_STATE;

			//Volumes/Panning
			int volume = FIXED_POINT_BIAS;
			int rightVolume = FIXED_POINT_BIAS;
			int leftVolume = FIXED_POINT_BIAS;
			
			//Looping flag
			bool loop = false;

			//Envelope Table
			unsigned int numberOfEnvelopeEntries = 0;		//Number of entries in the envelope table.  If it's zero envelopes aren't used.
			unsigned int currentEnvelopeEntry = 0;			//Current point in the envelope table
			int currentEnvelopVolume = 0;

			bool useEnvelope = false;						//Determine whether or not to use an envelope at all.

			//Sustain
			unsigned int sustainEntry = 0;					//This determines which entry in the envelop table is the sustain point.
			bool sustainEnabled = true;						//Determines if sustain is enabled.

			//State Flags
			bool triggered = false;
			

			bool sampleIndexExact = false;					//Determines if the sample point defined are relative or exact to handle looping samples and pitch shifting.

			unsigned int enveloptableSampleIndex[MAX_ENVELOPE_ENTRIES];
			int enveloptableVolumeLevel[MAX_ENVELOPE_ENTRIES];

			

		};

		//Class for the Sound System of the game engine.
		//Typically only one instance is used.
		class SoundSystem
		{

		private:
			//A thread reserved to run management functions for the sound system
			std::thread SoundManagerThread;

			//Current Frame Buffer Sizes
			unsigned long frameBufferSize;

			//Generate Frame Buffers
			void GenerateFrameBuffers(unsigned long newRenderFrameBufferSize);

			//Delete Frame Buffers
			void DeleteFrameBuffers();

			//Clear the mixing buffers
			void ClearMixingBuffers();

			//Flag to indicate if this object's attempt to initialize port audio threw an error
			bool paInitializeReturnedError = false;

			//PortAudio Error variable
			PaError portAudioError;

			//PortAudio Stream
			PaStream *soundSystemStream;

			//PortAudio Stream Parameters
			PaStreamParameters outputParameters;

			//Class object specific method callback for PortAudio
			int SoundSystemCallback(const void *input,
				void *output,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags);

			//Static PortAudio callback that serves as a wrapper to allow the SoundSystem object to be passed and the actual callback be called.
			static int PortAudioCallback(
				const void *input,
				void *output,
				unsigned long frameCount,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags,
				void *userData);

			unsigned int minBufferSize = 0;
			unsigned int maxBufferSize = 0;

		public:


			//All the system's sound channels
			SoundChannel soundChannels[MAX_CHANNELS];

			//All the sound channel target render buffers
			int* renderedChannelBuffers[MAX_CHANNELS];

			//All the sound samples in the system
			SoundSampleBuffer soundSamplesBuffers[Sound::MAX_SAMPLE_BUFFERS];

			//32-bit mixing buffers
			int* mixingFrameBufferRight;	//Mixing buffer for Right Channel
			int* mixingFrameBufferLeft;		//Mixing buffer for Left Channel

			//Initialize size for the Render Frame Buffers
			static const unsigned long INITIAL_RENDER_FRAME_BUFFER_SIZE = 1024;

			//Master volume for the system
			int masterVolume = FIXED_POINT_BIAS;

			//Default constructor
			SoundSystem();

			//Explicit destructor to handle closing out stuff.
			~SoundSystem();

			//System Managagement
			void Start();
			void Stop();
		};

		namespace FileFormatStructs
		{
			namespace WaveFile
			{
				//The "RIFF" chunk descriptor
				//WAV files are technically a RIFF type of file of the WAVE format
				struct RIFFHeader
				{
					char chunkID[4];					//Should be "RIFF" to identify it is a RIFF encoded file
					unsigned int chunkSize;				//Should be roughly the size of the file minus the 8 bytes for this and the ChunkID
					char format[4];						//Should be "WAVE" to identify it is a WAVE formatted file
				};

				//There can be many sub chunks in a Wav file.  You have to check to see if they are the ones you are interested in or not
				struct RIFFSubChunkHeader
				{
					char subChunkID[4];				//Sub chunk ID, should be "fmt " (null space)
					unsigned int subChunkSize;		//Sub chunk size, for PCM should be 16
				};

				//The fmt (format) Subchunk
				struct fmtSubChunk
				{
					unsigned short audioFormat;			//Audio Format, should be 1 for PCM
					unsigned short numberOfChannels;	//Number of channels, 1 for Mono, 2 for Stereo, and so forth
					unsigned int sampleRate;			//Sample rate of the audio data
					unsigned int byteRate;				//Number of bytes per second:  sampleRate * numberOfChannels * bitsPerSample / 8
					unsigned short blockAlignment;		//Number of bytes per frame with all channels:  numberOfChannels * bitsPerSamples / 8
					unsigned short bitsPerSample;		//Bit Depth, 8 = 8 bits, 16 = 16 bits, etc..
				};
			}
		}


		class SoundSystemWaveFile
		{
		private:
			FileFormatStructs::WaveFile::RIFFHeader waveFileHeader;
			FileFormatStructs::WaveFile::RIFFSubChunkHeader subChunkHeader;
			FileFormatStructs::WaveFile::fmtSubChunk fmtSubChunkData;

		public:
			SoundSystemWaveFile();
			~SoundSystemWaveFile();

			int LoadFile(char* targetFilename);

			short** audioData;
			unsigned int numberOfSamples;
			unsigned int numberOfChannels;
		};
	}
}