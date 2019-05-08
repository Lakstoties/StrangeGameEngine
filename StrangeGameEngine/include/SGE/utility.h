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

			//
			// A list of Module Tracker Format periods
			const enum class MOD_NOTE_PERIODS : unsigned int
			{
				//Octave 0 - Historically, unsupported.
				C0 = 1712,	//C
				CS0 = 1616,	//C#
				D0 = 1525,	//D
				DS0 = 1440,	//D#
				E0 = 1357,	//E
				F0 = 1281,	//F
				FS0 = 1209,	//F#
				G0 = 1141,	//G
				GS0 = 1077,	//G#
				A0 = 1017,	//A
				AS0 = 961,	//A#
				B0 = 907,	//B

				//Octave 1
				C1 = 856,	//C
				CS1 = 808,	//C#
				D1 = 762,	//D
				DS1 = 720,	//D#
				E1 = 678,	//E
				F1 = 640,	//F
				FS1 = 604,	//F#
				G1 = 570,	//G
				GS1 = 538,	//G#
				A1 = 508,	//A
				AS1 = 480,	//A#
				B1 = 453,	//B

				//Octave 2
				C2 = 428,	//C
				CS2 = 404,	//C#
				D2 = 381,	//D
				DS2 = 360,	//D#
				E2 = 339,	//E
				F2 = 320,	//F
				FS2 = 302,	//F#
				G2 = 285,	//G
				GS2 = 269,	//G#
				A2 = 254,	//A
				AS2 = 240,	//A#
				B2 = 226,	//B

				//Octave 3
				C3 = 214,	//C
				CS3 = 202,	//C#
				D3 = 190,	//D
				DS3 = 180,	//D#
				E3 = 170,	//E
				F3 = 160,	//F
				FS3 = 151,	//F#
				G3 = 143,	//G
				GS3 = 135,	//G#
				A3 = 127,	//A
				AS3 = 120,	//A#
				B3 = 113,	//B

				//Octave 4 - Historically Unsupported
				C4 = 107,	//C
				CS4 = 101,	//C#
				D4 = 95,	//D
				DS4 = 90,	//D#
				E4 = 85,	//E
				F4 = 80,	//F
				FS4 = 76,	//F#
				G4 = 71,	//G
				GS4 = 67,	//G#
				A4 = 64,	//A
				AS4 = 60,	//A#
				B4 = 57	//B
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
				class Effect
				{
				public:
					unsigned char Type = 0;
					unsigned char X = 0;
					unsigned char Y = 0;

					void Parse(unsigned short rawEffectData);
				};

				class ModuleLocation
				{
				public:
					//
					//  Total Nunmber of Positions
					//  
					unsigned char NumberOfPositions = 0;


					//
					//  Current Position and Division
					//
					unsigned char Position = 0;
					unsigned char Division = 0;

					//
					//  Jump To Position and Division
					//
					unsigned char JumpPosition = 0;
					unsigned char JumpDivision = 0;
					bool JumpNext = false;

					//
					//  Flag to indicate the end of the Module has been reached
					//
					bool EndOfModule = false;

					void NextDivision();
					void SetNextJump(unsigned char position, unsigned char division);
					void SetDivision(unsigned char division);
					void SetPosition(unsigned char position);
					void Reset();
				};

				
				ModuleLocation Location;

				class ModuleChannel
				{
				public:
					unsigned char Sample = 0;
					unsigned int Period = 0;
					Effect Effect;
				};

				ModuleChannel Channel[4];

				unsigned int beatsPerMinute = 125;
				unsigned char ticksADivision = 6;

				//
				//  Mapping MOD Samples to appropriate SGE Sound System Sample Buffers
				//
				int sampleMap[31] = { -1 };

				//SGE::Sound::Channel* channelMap[4] = { nullptr };
				int channelMap[4] = { -1 };

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
