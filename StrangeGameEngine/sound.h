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
		short *buffer = nullptr;
		unsigned int bufferSize = 0;
		unsigned int offset = 0;

		float offsetIncrement = 0.0f;



		

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

		//Status State Flags
		static const int STATUS_NOTLOADED_STATE = 0;
		static const int STATUS_LOADED_STATE = 1;
		static const int STATUS_PLAYING_STATE = 2;
		static const int STATIS_PAUSED_STATE = 3;

		//Status State
		int statusState = STATUS_NOTLOADED_STATE;

		//Load buffer with some sample.  Uses SoundSystem sample rate and sample resolution.
		void LoadSoundBuffer(unsigned int numOfSamples, short *samples);

		//Volume float
		float volume = 1.0f;

		//Pan float
		float pan = 0.0f;

		//Looping flag
		bool loop = false;

		//ADSR State Flags
		static const int ADSR_NONE_STATE = 0;
		static const int ADSR_ATTACK_STATE = 1;
		static const int ADSR_DECAY_STATE = 2;
		static const int ADSR_SUSTAIN_STATE = 3;
		static const int ADSR_RELEASE_STATE = 4;

		//ADSR State
		int adsrState = ADSR_NONE_STATE;

		//ADSR
		//Rates
		float attackRate = 0.0f;
		float decayRate = 0.0f;
		float releaseRate = 0.0f;

		//Levels
		float maxAttackLevel = 1.0f;
		float sustainLevel = 0.0f;

		float adsrLevel = 0.0f;

		//Flags
		bool adsrActive = false;
		bool adsrTriggered = false;

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

	public:
		//Number of channels for the system
		static const int MAX_CHANNELS = 32;				

		//All the system's sound channels
		SoundChannel soundChannels[MAX_CHANNELS];		
		
		//All the sound channel target render buffers
		int* renderedChannelBuffers[MAX_CHANNELS];	

		//32-bit mixing buffers
		int* mixingFrameBufferRight;	//Mixing buffer for Right Channel
		int* mixingFrameBufferLeft;		//Mixing buffer for Left Channel

		//Initialize size for the Render Frame Buffers
		const unsigned long INITIAL_RENDER_FRAME_BUFFER_SIZE = 1024;

		//Master volume for the system
		float masterVolume;								
		
		//Default constructor
		SoundSystem();		

		//Explicit destructor to handle closing out stuff.
		~SoundSystem();

		//Sound system sample rate
		static const int SAMPLE_RATE = 44100;

		//Sound system bit depth
		static const int SAMPLE_BITS = 16;

		//MIDI frequency note array
		static const float MIDI_NOTE_FRENQUENCY[128];

		//Half PI to Float resolution
		static const float HALF_PI_FLOAT;

		//PI to float resolution
		static const float PI_FLOAT;

		//Two PI to float resolution
		static const float TWO_PI_FLOAT;				

		//Maximum Amplitude given the sample bit depth
		static const int SAMPLE_MAX_AMPLITUDE;
		
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