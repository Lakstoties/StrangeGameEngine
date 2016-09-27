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
	class SoundChannel
	{
	private:

		//Pitch bits
		float keyedPitch = 1.0;
		float currentPitch = 1.0;
		float targetPitch = 1.0;
		
		//Buffer bits
		short *buffer;
		unsigned int bufferSize;
		unsigned int offset;

		float offsetIncrement;

		//Pan float
		float pan;

		//ADSR
		float attackRate;
		float decayRate;
		float sustainLevel;
		float releaseRate;


		//Flag bits
		bool playing;
		bool stopped;
		bool loaded;
		bool paused;
		bool loop;

		//ADSR Flag bits
		bool attackState;
		bool decayState;
		bool sustainState;
		bool releaseState;

	
	public:
		SoundChannel();
		~SoundChannel();

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

		//ADSR Functions
		//ADSR activation control
		void ADSRActive(bool newState);
		bool ADSRActive();

		//Attack Functions
		void ADSRAttack(float attackRate);
		float ADSRAttack();

		//Decay Functions
		void ADSRDecay(float decayRate);
		float ADSRDecay();

		//Sustain Function
		void ADSRSustain(float sustainPoint);
		float ADSRSustain();

		//Release Functions
		void ADSRRelease(float releaseRate);
		float ADSRRelease();

		//Trigger Functions
		//Soft play, if a channel is already playing, it will go to the Attack state.
		void Trigger();

		//Soft stop, will allow for a release decay from the sustain level to take place before hard stop.
		void Release();


		//Functions for managing volume
		void SetVolume(float volume);
		float GetVolume();

		//Function for managing key
		void SetKey(float pitch);
		float GetKey();

		//Function for managing pitch
		void SetPitch(float pitch);
		float GetPitch();

		//Functions to changing play offset
		void SetOffset(unsigned int targetOffset);
		unsigned int GetOffset();
	
		//Functions for panning
		void SetPan(float pan);
		float GetPan();


		void Loop(bool active);

		//Status functions
		bool Playing();
		bool Stopped();
		bool Loaded();
		bool Active();
		bool Paused();

		//Load buffer with some sample.  Uses SoundSystem sample rate and sample resolution.
		void LoadSoundBuffer(unsigned int numOfSamples, short *samples);

		//Volume float
		float volume;
	};


	class SoundSystem
	{
	private:
		std::thread SoundManagerThread;

		//Current Frame Buffer Sizes
		unsigned long frameBufferSize;

		//Generate Frame Buffers
		void GenerateFrameBuffers(unsigned long newRenderFrameBufferSize);

		//Delete Frame Buffers
		void DeleteFrameBuffers();

		//Clear the mixing buffers
		void ClearMixingBuffers();

		bool paInitializeReturnedError = false;		//Flag to indicate if this object's attempt to initialize port audio threw an error

		PaError portAudioError;						//PortAudio Error variable

		PaStream *soundSystemStream;				//PortAudio Stream

		PaStreamParameters outputParameters;

		//Class object specific method callback for PortAudio
		int SoundSystemCallback(const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags);

		short *soundSystemBuffer;

		//Static PortAudio callback that serves as a wrapper to allow the SoundSystem object to be passed and the actual callback be called.
		static int PortAudioCallback(
			const void *input,
			void *output,
			unsigned long frameCount,
			const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags,
			void *userData);

	public:
		static const int MAX_CHANNELS = 32;				//Number of channels for the system

		SoundChannel soundChannels[MAX_CHANNELS];		//All the system's sound channels
		
		int* renderedChannelBuffers[MAX_CHANNELS];	//All the sound channel target render buffers

		//32-bit mixing buffers
		int* mixingFrameBufferRight;					//Mixing buffer for Right Channel
		int* mixingFrameBufferLeft;						//Mixing buffer for Left Channel

		//Initialize size for the Render Frame Buffers
		const unsigned long INITIAL_RENDER_FRAME_BUFFER_SIZE = 1024;


		float masterVolume;								//Master volume for the system
		

		SoundSystem();					//Default constructor
		~SoundSystem();					//Explicit destructor to handle closing out stuff.

		
		static const int SAMPLE_RATE = 44100;			//Sound system sample rate
		static const int SAMPLE_BITS = 16;				//Sound system bit depth
		static const float MIDI_NOTE_FRENQUENCY[128];	//MIDI frequency not
		static const float HALF_PI_FLOAT;				//Half PI to Float resolution
		static const float PI_FLOAT;					//PI resolution
		static const float TWO_PI_FLOAT;				//Two PI resolution

		static const int SAMPLE_MAX_AMPLITUDE;			//Maximum Amplitude given the sample bit depth


		//System Managagement
		void Start();
		void Stop();

		//Sample Generation Tools

		//Generate a sine wave sample array
		static short* SineGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

		//Generate a pulse wave sample array
		static short* PulseGenerator(float hertz, float dutyCycle, unsigned int samplesToGenerate, short amplitude);

		//Generate a triangle wave sample array
		static short* TriangleGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

		//Generate a sawtooth wave sample array
		static short* SawtoothGenerator(float hertz, unsigned int samplesToGenerate, short amplitude);

		//Generate a noise wave sample array
		static short* NoiseGenerator(unsigned int samplesToGenerate, short amplitude);		
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