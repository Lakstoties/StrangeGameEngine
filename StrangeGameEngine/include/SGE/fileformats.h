#pragma once
#include "render.h"
#include "sound.h"
#include "display.h"

namespace SGE
{
	namespace FileFormats
	{
		//Struct that contains converted bitmap data from a file
		struct Bitmap
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

		struct Wave
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
	}
}