#pragma once
#include "api.h"
#include "sound.h"
#include "display.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace FileFormats
	{
		//Struct that contains converted bitmap data from a file
		struct SGEAPI Bitmap
		{
			//File Header
			char idField[2];				//ID Field,  should be "BM" for Bitmap.
			unsigned int bmpSize;			//Bitmap file size
			unsigned short reserved1;		//Reserved data that's application specific
			unsigned short reserved2;		//Reserved data that's application specific
			unsigned int offset;			//The offset/address where the bitmap image data can be found.

			//Info Header
			unsigned int sizeOfHeader;		//Size of the header, used to indicate version of the header used and extended information that may exist
			unsigned int width;		//Width of the bitmap in pixels
			unsigned int height;		//Height of the bitmap in pixels
			unsigned short colorPanes;		//Number of color panes
			unsigned short bitsPerPixel;	//Bits per pixel or color depth of image
			unsigned int compressionMethod;	//Compress method in use
			unsigned int dataSize;			//Size of the raw bitmap data
			int horizontalResolution;		//Print horizontal resolution
			int verticalResolution;			//Print vertical resolution
			unsigned int colorsInPalette;	//Number of colors in the color palette
			unsigned int importantColors;	//Number of important colors

			//Data Section
			unsigned int imageDataSize = 0;
			SGE::Display::Video::pixel* imageData = nullptr;

			//Function to Load Data
			int LoadFile(char* targetFilename);
		};

		//Struct that contains converted wave data from a file
		struct SGEAPI Wave
		{
			//
			//  Wave File Header  (RIFF Chunk)
			//
			char chunkID[4];					//Should be "RIFF" to identify it is a RIFF encoded file
			unsigned int chunkSize;				//Should be roughly the size of the file minus the 8 bytes for this and the ChunkID
			char format[4];						//Should be "WAVE" to identify it is a WAVE formatted file

			//
			//  Format Subchunk Header
			//
			char subChunkID[4];				//Sub chunk ID, should be "fmt " (null space)
			unsigned int subChunkSize;		//Sub chunk size, for PCM should be 16

			//
			//  Format SubChunk Data
			//
			unsigned short audioFormat;			//Audio Format, should be 1 for PCM
			unsigned short numberOfChannels;	//Number of channels, 1 for Mono, 2 for Stereo, and so forth
			unsigned int sampleRate;			//Sample rate of the audio data
			unsigned int byteRate;				//Number of bytes per second:  sampleRate * numberOfChannels * bitsPerSample / 8
			unsigned short blockAlignment;		//Number of bytes per frame with all channels:  numberOfChannels * bitsPerSamples / 8
			unsigned short bitsPerSample;		//Bit Depth, 8 = 8 bits, 16 = 16 bits, etc..

			Wave();
			~Wave();

			int LoadFile(char* targetFilename);

			SGE::Sound::sampleType** audioData = nullptr;
			unsigned int numberOfSamples = 0;
		};

		//Class for a module tracker file (FastTracker format)
		class SGEAPI ModuleFile
		{
			///
			///  Specific Module File Structures
			///
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


		public:
			//
			//  MOD File Header
			//

			char title[20] = { 0 };						//Module Title
			unsigned char songPositions = 0;			//Number of song positions, AKA patterns. 1 - 128
			unsigned char patternTable[128] = { 0 };	//Pattern table, legal valves 0 - 63  (High value in table is the highest pattern stored.)

			MODSample samples[31];
			MODPatternData patterns[64];
			unsigned char numberOfPatterns = 0;

			ModuleFile();
			~ModuleFile();

			int LoadFile(char* targetFilename);
			SGE::Sound::sampleType* ConvertSample(unsigned char sample);
			unsigned int ConvertSampleSize(unsigned char sample);

		};
	}
}