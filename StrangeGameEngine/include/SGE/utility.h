#pragma once
#include "api.h"
#include "sound.h"
#include "fileformats.h"
#include "gui.h"
#include <thread>
#include <ctime>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace Utility
	{
		namespace Math
		{
			//
			//  Constants
			//

			//  
			const int PRECALCULATED_DEGREE_RESOLUTION = 3600;

			//Precalculated PI/2 to float precision
			const float HALF_PI_FLOAT = 1.57079632679489661923f;

			//Precalculated PI to float precision
			const float PI_FLOAT = 3.14159265358979323846f;

			//Precalculated 2*PI to float precision 
			const float TWO_PI_FLOAT = 6.28318530717958647692f;

			//
			//  Precalculated Arrays for Common Math Functions
			//

			extern float SineDegree[PRECALCULATED_DEGREE_RESOLUTION];

			extern float CosineDegree[PRECALCULATED_DEGREE_RESOLUTION];

			void InitializeMath();

			void SGEAPI RotatePointAroundPoint(int originalPointX, int originalPointY, int centerPointX, int centerPointY, int &rotatedPointX, int &rotatedPointY, float degrees);
		}

		namespace ModuleTrackerMusic
		{
			//
			//  Components relevent to Module Tracker parts of the sound system
			//

			// A list of Module Tracker Format periods
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

			// A list of Tracker Music MODule file conversation notes.
			// Tuning frequency for NTSC screens for Mod files
			const float NTSC_TUNING = 7159090.5f;

			// Tuning frequency for PAL screens for Mod Files
			const float PAL_TUNING = 7093789.2f;

			// Typical old school tick timing was around 20 milliseconds
			const unsigned int DEFAULT_TICK_TIMING_MILLI = 20;
		
			const unsigned int DEFAULT_TICK_TIMING_MICRO = DEFAULT_TICK_TIMING_MILLI * 1000;

			// Default Ticks a Division
			const unsigned int DEFAULT_TICKS_A_DIVISION = 6;

			// Number of samples sent per tick based on sample rate and typical tick rate
			const unsigned int DEFAULT_SAMPLES_TICK = SGE::Sound::SAMPLE_RATE * DEFAULT_TICK_TIMING_MILLI / 1000;

			//Player for module files
			class SGEAPI ModulePlayer
			{
			public:
				unsigned char CurrentPosition = 0;
				unsigned char CurrentPattern = 0;
				unsigned char CurrentDivision = 0;
				unsigned char CurrentChannelSamples[4] = { 0 };
				unsigned char PreviousChannelSamples[4] = { 0 };
				unsigned int CurrentChannelPeriods[4] = { 0 };

				unsigned int beatsPerMinute = 125;
				unsigned char ticksADivision = 6;

				//SGE::Sound::SampleBuffer* sampleMap[31] = { nullptr };

				//
				//  Mapping MOD Samples to appropriate SGE Sound System Sample Buffers
				//
				int sampleMap[31] = { -1 };

				SGE::Sound::Channel* channelMap[4] = { nullptr };

				//
				SGE::FileFormats::ModuleFile modFile;
				bool PlayerThreadActive = false;
				std::thread* playerThread = NULL;


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

		namespace Timer
		{
			SGEAPI void AccurateWaitForMilliseconds(int milliseconds, bool superAccurate);

			class SGEAPI TimerDelta
			{
			private:
				bool started = false;

			public:
				float Rate = 0.0f;
				void Start(float rateOfChange);
				float Stop();

				std::clock_t StartTime = 0;
				std::clock_t EndTime = 0;
			};
		}
	}
}
