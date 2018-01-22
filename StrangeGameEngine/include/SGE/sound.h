#pragma once

#include <thread>
#include <mutex>
namespace SGE
{
	namespace Sound
	{
		//
		//  Typedef to define the format used for audio data
		//

		typedef short sampleType;


		//
		//  Sound System specific constants
		//
		//Number of channels for the system
		const int MAX_CHANNELS = 32;

		//Number of samples for the system
		const int MAX_SAMPLE_BUFFERS = 256;

		//Sound system sample rate
		const int SAMPLE_RATE = 44100;

		//Sound system bit depth
		const int SAMPLE_BITS = sizeof(sampleType) << 3;

		//Max frequency that can represented by the Sample Rate
		const int MAX_FREQUENCY = SAMPLE_RATE / 2;

		//Maximum Amplitude given the sample bit depth
		//This is usually half the maximum number represented by the number of bits
		const int SAMPLE_MAX_AMPLITUDE = (1 << (SAMPLE_BITS - 1)) - 1;

		//
		//  Components relevent to Module Tracker parts of the sound system
		//
		namespace ModTracker
		{
			//A list of Module Tracker Format periods
			const unsigned int MOD_NOTE_PERIOD[] =
			{
				//Octave 0 - Historically, unsupported.
				1712,	//C
				1616,	//C#
				1525,	//D
				1440,	//D#
				1357,	//E
				1281,	//F
				1209,	//F#
				1141,	//G
				1077,	//G#
				1017,	//A
				961,	//A#
				907,	//B

				//Octave 1
				856,	//C
				808,	//C#
				762,	//D
				720,	//D#
				678,	//E
				640,	//F
				604,	//F#
				570,	//G
				538,	//G#
				508,	//A
				480,	//A#
				453,	//B

				//Octave 2
				428,	//C
				404,	//C#
				381,	//D
				360,	//D#
				339,	//E
				320,	//F
				302,	//F#
				285,	//G
				269,	//G#
				254,	//A
				240,	//A#
				226,	//B

				//Octave 3
				214,	//C
				202,	//C#
				190,	//D
				180,	//D#
				170,	//E
				160,	//F
				151,	//F#
				143,	//G
				135,	//G#
				127,	//A
				120,	//A#
				113,	//B

				//Octave 4 - Historically Unsupported
				107,	//C
				101,	//C#
				95,		//D
				90,		//D#
				85,		//E
				80,		//F
				76,		//F#
				71,		//G
				67,		//G#
				64,		//A
				60,		//A#
				57		//B
			};

			//A list of Tracker Music MODule file conversation notes.
			//Tuning frequency for NTSC screens for Mod files
			const float NTSC_TUNING = 7159090.5f;

			//Tuning frequency for PAL screens for Mod Files
			const float PAL_TUNING = 7093789.2f;

			//Typical old school tick timing was around 20 milliseconds
			const unsigned int DEFAULT_TICK_TIMING_MILLISECONDS = 20;

			//Typical old school tick timing was around 20 milliseconds.
			const unsigned int DEFAULT_TICK_TIMING_NANO = DEFAULT_TICK_TIMING_MILLISECONDS * 1000000;

			//Default Ticks a Division
			const unsigned int DEFAULT_TICKS_A_DIVISION = 6;

			//  Number of samples sent per tick based on sample rate and typical tick rate
			const unsigned int DEFAULT_SAMPLES_TICK = SAMPLE_RATE * DEFAULT_TICK_TIMING_MILLISECONDS / 1000;
		}

		//
		//  Common values that are precalculated for efficiency.
		//
		namespace Precalculated
		{
			//Precalculated PI/2 to float precision
			const float HALF_PI_FLOAT = 1.57079632679489661923f;

			//Precalculated PI to float precision
			const float PI_FLOAT = 3.14159265358979323846f;

			//Precalculated 2*PI to float precision 
			const float TWO_PI_FLOAT = 6.28318530717958647692f;

			//Semitone multipler
			const float SEMITONE_MULTIPLIER = 1.0595f;
		}



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



		//
		//  Base Pre-Generated Waveforms
		//
		namespace Waveforms
		{
			//
			//  Data regions to store waves
			//
			
			//Square Waveform
			extern float Sine[SAMPLE_RATE];

			//Ramp Down Waveform
			extern float RampDown[SAMPLE_RATE];

			//Square Waveform
			extern float Square[SAMPLE_RATE];

			//
			//  Generation Functions
			//

			//Generate Sine waveform
			void PreGenerateSine();

			//Generate Ramp Down waveform
			void PreGenerateRampDown();

			//Generate Square waveform
			void PreGenerateSquare();

		}

		//
		//  Sample Buffer Structure
		//


		//Sample Buffer to contain audio data that sound channels will link to and play from.
		//For the purposes of the Strange Game Engine.  The data put into these buffers are assumed to be PCM Signed 16-bit at 44.1Khz sample rate.
		struct SoundSampleBuffer
		{
		public:
			//Buffer to contain the samples
			sampleType *buffer = nullptr;

			//Size of the buffer
			unsigned int bufferSize = 0;

			//The offeset the sample will repeat at.
			unsigned int repeatOffset = 0;

			//The duration of the repeat
			unsigned int repeatDuration = 0;

			//Create a blank buffer of a certain sample size
			int CreateBlankBuffer(unsigned int numOfSamples);

			//Create a blank buffer, and then load data into it.
			int Load(unsigned int numOfSamples, sampleType *samples);

			//Zero out a buffer completely
			int ZeroBuffer();

			//Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
			int ResetBuffer();

			//Destructor to make sure the buffer memory is freed upon destruction to prevent memory leaks.
			~SoundSampleBuffer();			
		};


		//
		//  Sound Channel Structure
		//

		struct SoundChannel
		{
			//Offset 
			//Sample offset
			float offset = 0;

			//Amount to increment the offset by
			float offsetIncrement = 0;

			//Current offset Increment after all effects have been applied
			float currentOffsetIncrement = 0;

			//Current Sound Sample Buffer in use
			//Set to the maximum buffers, to indicate one hasn't been selected.
			SoundSampleBuffer* currentSampleBuffer = nullptr;
			
			//Render functions
			void Render(unsigned int numerOfSamples, int* sampleBuffer);

			//Basic functions
			//Play
			void Play();

			//Hard stop
			void Stop();

			//Status flag
			bool Playing = false;

			//Volumes/Panning
			float Volume = 1.0f;
			float Pan = 0.0f;

			//
			//Effects for Module system
			//

			//Arpeggio effect, Effect 0
			bool EnableArpeggio = false;
			unsigned int ArpeggioSampleInterval = 0;
			unsigned int currentArpeggioSamples = 0;
			unsigned int arpeggioState = 0;
			unsigned int arpeggioSemitoneX = 0;
			unsigned int arpeggioSemitoneY = 0;
			float arpeggioOffsetIncrement = 0.0f;

			//Volume Slide, Effect 10
			bool EnableVolumeSlide = false;
			unsigned int VolumeSlideSampleInterval = 0;
			unsigned int currentVolumeSlideSamples = 0;
			float currentVolumeSlide = 0.0f;
			float VolumeSlideRate = 0.0f;

			//Vibrato, Effect 4 
			bool EnableVibrato = false;
			unsigned int currentVibratoWaveformPosition = 0;
			float VibratoAmplitude = 0;
			float VibratoCycles = 0;
			float vibratoOffsetIncrement = 0;

			//Vibrato with Effect 14 - 4 settings
			float* VibratoWaveform = Waveforms::Sine;
			bool RetriggerVibrato = false;

			//Statistics Stuff
			unsigned int LastRenderedAverageLevel = 0;
		};

		//Class for the Sound System of the game engine.
		//Typically only one instance is used.

		//Master volume for the system
		extern float MasterVolume;

		//Statistics
		extern unsigned int MasterVolumeAverageLeftLevel;
		extern unsigned int MasterVolumeAverageRightLevel;

		//All the sound samples in the system
		extern SoundSampleBuffer SampleBuffers[Sound::MAX_SAMPLE_BUFFERS];

		//All the system's sound channels
		extern SoundChannel Channels[MAX_CHANNELS];

		//System Managagement
		void Start();
		void Stop();
	
		namespace FileFormatStructs
		{
			namespace MODFile
			{
				struct MODSample
				{
					char title[23] = { 0 };					//Sample Title
					unsigned short lengthInWords = 0;		//Sample Length in Words (16-bit chunks)
					unsigned char finetune = 0;				//Sample Fine Tune, in lowest four bits.  Technically a signed nibble.
					unsigned char volume = 0;				//Sample volume.  0 - 64 are legal values
					unsigned short repeatOffset = 0;		//Sample repeat offset
					unsigned short repeatLength = 0;		//Sample repeat length
					char* data = nullptr;					//Pointer to Sample data
				};

				struct MODChannelData
				{
					unsigned char sample = 0;				//Sample to use for this channel this division
					unsigned short period = 0;				//Period to play the sample at on this channel
					unsigned short effect = 0;				//Effects to use on this channel this division
				};

				struct MODDivisionData
				{
					MODChannelData channels[4];			//Channels in a division
				};

				struct MODPatternData
				{
					MODDivisionData division[64];		//Division in a pattern
				};
			}
		}

		class ModuleFile
		{
		public:
			//
			//  MOD File Header
			//

			char title[20] = { 0 };						//Module Title
			unsigned char songPositions = 0;			//Number of song positions, AKA patterns. 1 - 128
			unsigned char patternTable[128] = { 0 };	//Pattern table, legal valves 0 - 63  (High value in table is the highest pattern stored.)

			FileFormatStructs::MODFile::MODSample samples[31];
			FileFormatStructs::MODFile::MODPatternData patterns[64];
			unsigned char numberOfPatterns = 0;

			ModuleFile();
			~ModuleFile();

			int LoadFile(char* targetFilename);
			sampleType* ConvertSample(unsigned char sample);
			unsigned int ConvertSampleSize(unsigned char sample);

		};

		//Player for module files
		class ModulePlayer
		{
		public:
			unsigned char CurrentPosition = 0;
			unsigned char CurrentPattern = 0;
			unsigned char CurrentDivision = 0;
			unsigned char CurrentChannelSamples[4] = { 0 };

			unsigned int beatsPerMinute = 125;
			unsigned char ticksADivision = 6;

			SoundSampleBuffer* sampleMap[31] = { nullptr };
			SoundChannel* channelMap[4] = { nullptr };

			//
			ModuleFile modFile = ModuleFile();
			bool PlayerThreadActive = false;
			std::thread playerThread;


			//Main functions
			bool Load(char * filename);
			bool Connect(unsigned int startChannel, unsigned int startSample);
			bool Play();
			bool Stop();

			//Other fuction
			void PlayThread();

			//Constructor Stuff
			ModulePlayer();
			~ModulePlayer();

		};
	}
}