#pragma once
namespace SGE
{
	namespace Sound
	{
		//A list of MIDI notes and their associated frequencies.
		const float MIDI_NOTE_FRENQUENCY[] =
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

		//Precalculated PI/2 to float precision
		const float HALF_PI_FLOAT = 1.57079632679489661923f;

		//Precalculated PI to float precision
		const float PI_FLOAT = 3.14159265358979323846f;

		//Precalculated 2*PI to float precision 
		const float TWO_PI_FLOAT = 6.28318530717958647692f;

		//Sound system sample rate
		const int SAMPLE_RATE = 44100;

		//Sound system bit depth
		const int SAMPLE_BITS = 16;

		//Maximum Amplitude given the sample bit depth
		//This is usually half the maximum number represented by the number of bits
		const int SAMPLE_MAX_AMPLITUDE = (1 << (SAMPLE_BITS - 1)) - 1;

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
		public:
			//Offset 
			//Sample offset
			float offset = 0;

			//Amount to increment the offset by
			float offsetIncrement = 0;

			//Current Sound Sample Buffer in use
			//Set to the maximum buffers, to indicate one hasn't been selected.
			 SoundSampleBuffer* currentSampleBuffer = nullptr;
			
			//Render functions
			void RenderSamples(unsigned int numerOfSamples, int* sampleBuffer);

			//Basic functions
			//Play
			void Play();

			//Hard stop
			void Stop();

			//Status flag
			bool Playing = false;

			//Repeat information
			//This channel can repeat
			bool Repeatable = false;

			//The offeset the sample will repeat at.
			unsigned int repeatOffset = 0;

			//The duration of the repeat
			unsigned int repeatDuration = 0;

			//Volumes/Panning
			float Volume = 1.0f;
			float Pan = 0.0f;
		};



		//Class for the Sound System of the game engine.
		//Typically only one instance is used.

		//Master volume for the system
		extern float MasterVolume;

		//All the sound samples in the system
		extern SoundSampleBuffer SampleBuffers[Sound::MAX_SAMPLE_BUFFERS];

		//All the system's sound channels
		extern SoundChannel Channels[MAX_CHANNELS];

		//System Managagement
		void Start();
		void Stop();
	
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


		class WaveFile
		{
		private:
			FileFormatStructs::WaveFile::RIFFHeader waveFileHeader;
			FileFormatStructs::WaveFile::RIFFSubChunkHeader subChunkHeader;
			FileFormatStructs::WaveFile::fmtSubChunk fmtSubChunkData;

		public:
			WaveFile();
			~WaveFile();

			int LoadFile(char* targetFilename);

			short** audioData;
			unsigned int numberOfSamples;
			unsigned int numberOfChannels;
		};
	}
}