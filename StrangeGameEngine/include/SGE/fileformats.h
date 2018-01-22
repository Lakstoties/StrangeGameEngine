#pragma once
#include "render.h"
#include "display.h"

namespace SGE
{
	namespace FileFormats
	{
		//Basic Bitmap info header... Mostly all the info we really need... hopefully.
		struct InfoHeader
		{

		};

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

	}
}