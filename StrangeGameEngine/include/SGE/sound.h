#pragma once
#include "api.h"
#include <thread>
#include <mutex>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	//
	//  Sound system namespace
	//
	namespace Sound
	{
		//
		//  Typedef to define the format used for audio data
		//
		typedef short sampleType;

		//
		//  Number of channels for the system
		//
		const int MAX_CHANNELS = 32;

		//
		//  Number of samples for the system
		//
		const int MAX_SAMPLE_BUFFERS = 256;

		//
		//  Sound system sample rate
		//
		const int SAMPLE_RATE = 44100;

		//
		//  Sound system bit depth
		//
		const int SAMPLE_BITS = sizeof(sampleType) << 3;

		//
		//  Max frequency that can represented by the Sample Rate
		//
		const int MAX_FREQUENCY = SAMPLE_RATE / 2;

		//
		//  Maximum Amplitude given the sample bit depth
		//  This is usually half the maximum number represented by the number of bits
		//
		const int SAMPLE_MAX_AMPLITUDE = (1 << (SAMPLE_BITS - 1)) - 1;

		//
		//  Common values that are precalculated for efficiency.
		//
		namespace Precalculated
		{
			//
			//  Precalculated PI/2 to float precision
			//
			const float HALF_PI_FLOAT = 1.57079632679489661923f;

			//
			//  Precalculated PI to float precision
			//
			const float PI_FLOAT = 3.14159265358979323846f;

			//
			//  Precalculated 2*PI to float precision 
			//
			const float TWO_PI_FLOAT = 6.28318530717958647692f;

			//
			//  Semitone multipler
			//
			const float SEMITONE_MULTIPLIER = 1.0595f;
		}


		//
		//  A list of MIDI notes and their associated frequencies.
		//
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
			//  Arrays to store waves in floats
			//
			
			//
			//  Square Waveform
			//
			extern float Sine[SAMPLE_RATE];

			//
			//  Ramp Down Waveform
			//
			extern float RampDown[SAMPLE_RATE];

			//
			//  Square Waveform
			//
			extern float Square[SAMPLE_RATE];

			//
			//  Generation Functions
			//

			//
			//  Generate Sine waveform
			//
			void PreGenerateSine();

			//
			//  Generate Ramp Down waveform
			//
			void PreGenerateRampDown();

			//
			//  Generate Square waveform
			//
			void PreGenerateSquare();
		}

		//
		//  Sample Buffer Class
		//  Contain audio data that sound channels will link to and play from.
		//
		class SampleBuffer
		{
		public:
			//
			//  Buffer to contain the samples
			//
			sampleType* data = NULL;

			//
			//  Size of the buffer
			//
			unsigned int size = 0;

			//
			//  The offeset the sample will repeat at.
			//
			unsigned int repeatOffset = 0;

			//
			//  The duration of the repeat
			//
			unsigned int repeatDuration = 0;

			//
			//  Create a blank buffer of a certain sample size
			//
			bool Allocate(unsigned int numOfSamples);

			//
			//  Create a blank buffer, and then load data into it.
			//
			bool Load(unsigned int numOfSamples, sampleType *samples);

			//
			//  Zero out a buffer completely
			//
			bool Zero();

			//
			//  Free the buffer back to the system, effectively resetting it to before any creation or loading was done to it.
			//
			void Delete();

			//
			//  Destructor to make sure the buffer memory is freed upon destruction to prevent memory leaks.
			//
			~SampleBuffer();			
		};


		namespace FMSynth
		{
			const int OPERATORS_PER_CHANNEL = 4;
			const int MAXIMUM_OPERATORS_TO_PROCESS = 16;


			class Operator
			{
			private:

			public:

				//Float array to use for the waveform
				float* WaveformArray = NULL;

				//Current location in waveform array  
				float currentArrayIndex = 0.0f;

				//How much to increment the array per sample gotten
				float increment = 0.0f;

				float Generate(float modulation);


				Operator();
			};


			class Channel
			{
			private:

			public:
				//

				//Operator array
				Operator Operators[OPERATORS_PER_CHANNEL];

				//Operator Order Array
				Operator* orderOfOperators[MAXIMUM_OPERATORS_TO_PROCESS] = { NULL };

				//Operator Input Array
				bool operatorInputArray[OPERATORS_PER_CHANNEL][OPERATORS_PER_CHANNEL] = { false };

				//Operator Main Out Array
				bool operatorMainOutputArray[OPERATORS_PER_CHANNEL] = { false };

				//Generate a sample
				sampleType Generate();

				void Render(unsigned int numberOfSamples);

				Channel();
			};
		}



		//
		//  Sound Channel Class
		//
		class Channel
		{
		public:
			//
			//  Sample offset
			//
			float offset = 0;

			//
			//  Amount to increment the offset by
			//
			float offsetIncrement = 0;

			//
			//  Current offset Increment after all effects have been applied
			//
			float currentOffsetIncrement = 0;

			//
			//  Current Sound Sample Buffer in use
			//  Set to the maximum buffers, to indicate one hasn't been selected.
			//
			SampleBuffer* currentSampleBuffer = NULL;
			
			//
			//  Render function
			//
			void Render(unsigned int numerOfSamples, int* outputBuffer);
			
			//
			//  Basic functions
			//

			//
			//  Play
			//
			void Play();

			//
			//  Hard stop
			//
			void Stop();

			//
			//  Status flag
			//
			bool Playing = false;

			//
			//  Volumes/Panning
			//
			float Volume = 1.0f;
			float Pan = 0.0f;

			//
			//  Effects for Module system
			//

			//
			//  Arpeggio effect, Effect 0
			//
			bool			arpeggioEnabled = false;
			unsigned int	arpeggioSampleInterval = 0;
			unsigned int	arpeggioCurrentSamples = 0;
			unsigned int	arpeggioState = 0;
			unsigned int	arpeggioSemitoneX = 0;
			unsigned int	arpeggioSemitoneY = 0;
			float			arpeggioOffsetIncrement = 0.0f;

			//
			//  Period Slide, Effect 1 and 2
			//
			bool			periodSlidEnabled = false;
			unsigned int	periodSlideSampleInterval = 0;
			unsigned int	periodSlideCurrentSamples = 0;
			float			periodSlideDelta = 0.0f;
			float			periodTarget = 0.0f;


			//
			//  Volume Slide, Effect 10
			//
			bool			volumeSlideEnabled = false;
			unsigned int	volumeSlideSampleInterval = 0;
			unsigned int	volumeSlideCurrentSamples = 0;
			float			volumeSlideCurrentVolume = 0.0f;
			float			volumeSlideRate = 0.0f;

			//
			//  Vibrato, Effect 4 
			//
			bool			vibratoEnabled = false;
			unsigned int	vibratoCurrentWaveformPosition = 0;
			float			vibratoAmplitude = 0;
			float			vibratoCycles = 0;
			float			vibratoOffsetIncrement = 0;

			//
			//  Vibrato with Effect 14 - 4 settings
			//
			float*			vibratoWaveform = Waveforms::Sine;
			bool			vibratoRetriggers = false;

			//
			//  Retrigger, Effect 14 - 9 settings
			//
			bool		 retriggerSampleEnabled = false;
			unsigned int retriggerSampleInterval = 0;
			unsigned int retriggerCurrentSamples = 0;
			unsigned int retriggerSampleDestination = 0;

			//
			//  Cut,  Effect 14-12 settings
			//
			bool		 cutSampleEnabled = false;
			unsigned int cutSampleInterval = 0;
			unsigned int cutCurrentSamples = 0;

			//
			//  Delay, Effect 14-13 settings
			//
			bool		 delaySampleEnabled = false;
			unsigned int delaySampleInterval = 0;
			unsigned int delayCurrentSamples = 0;

			//
			//  Statistics Stuff
			//
			unsigned int LastRenderedAverageLevel = 0;
		};

		//Class for the Sound System of the game engine.
		//Typically only one instance is used.

		//
		//  Master volume for the system
		//
		extern SGEAPI float MasterVolume;

		//
		//  Statistics
		//
		extern SGEAPI unsigned int MasterVolumeAverageLeftLevel;
		extern SGEAPI unsigned int MasterVolumeAverageRightLevel;

		//
		//  All the sound samples in the system
		//
		extern SampleBuffer SampleBuffers[MAX_SAMPLE_BUFFERS];

		//
		//  All the system's sound channels
		//
		extern Channel Channels[MAX_CHANNELS];

		//
		//  System Managagement
		//
		void Start();
		void Stop();
	}
}