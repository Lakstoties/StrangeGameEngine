#include "include\SGE\fileformats.h"

namespace SGE
{
	namespace FileFormats
	{
		const int BITMAP_FILE_HEADER_SIZE = 14;
		const int BITMAP_DATA_HEADER_SIZE = 40;

		int Bitmap::LoadFile(char* targetFilename)
		{
			FILE* bitmapFile;
			size_t readCount = 0;
			unsigned char pixelRed;
			unsigned char pixelGreen;
			unsigned char pixelBlue;

			//
			//  Open the File
			//

			//Attempt to open the bitmap file
			//Open in binary read mode.
			bitmapFile = fopen(targetFilename, "rb");

			//Check to see if we got a valid file pointer
			if (bitmapFile == NULL)
			{
				fprintf(stderr, "Bitmap File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//
			//  Read in File Header Info
			// 

			//  At this point we should have a valid file pointer
			//  Now, let's see if it's remotely the correct format
			//  Read all the individual elements and keep track of how many bytes we've read in

			//Initial readCount to 0
			readCount = 0;

			//Read in the ID fieid.  We are looking for a "BM" to indicate it is a typical BMP/Bitmap
			readCount += fread(&idField, 1, 2, bitmapFile);

			//The ID field before continuing
			//If memcmp doesn't return a 0, something is different
			if (std::memcmp("BM", &idField, 2) != 0)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Incorrect ID field.\n", targetFilename);
				return -2;
			}

			//Read in the BMP field size in bytes
			readCount += fread(&bmpSize, 1, 4, bitmapFile);

			//Read some reserved data, application specific, not our concern really.
			readCount += fread(&reserved1, 1, 2, bitmapFile);

			//Read in some more reserved data, application specific, not our concern, moving on...
			readCount += fread(&reserved2, 1, 2, bitmapFile);

			//Read in the offset that should indicate where the file data should begin
			readCount += fread(&offset, 1, 4, bitmapFile);


			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount < BITMAP_FILE_HEADER_SIZE)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - File header too small.  Read count: %i\n", targetFilename, readCount);
				return -3;
			}

			//
			//  Read Bitmap Info header
			//

			//Read in the size of the Info header
			//This can be used to determine which header is in use.  But, we only need the most basic header info, so this will just help us confirm the data offset value

			//Also reset the read count with the value from this read
			readCount = 0;

			readCount += fread(&sizeOfHeader, 1, 4, bitmapFile);

			//Check to make sure this value is of some kind of known value
			//These values were pulled from the Wikipedia entry on BMP File Format
			if (!(							//If it's not
				sizeOfHeader == 12 ||		//The size of a Windows 2.0 or later / OS/2 1.x header
				sizeOfHeader == 64 ||		//The size of a OS/2 BitmapCoreHeader2	(In theory it adds halftoning, RLE, and Huffnam 1D compression... None we really are going to support.)
				sizeOfHeader == 16 ||		//The size of another OS/2 Bitmap header
				sizeOfHeader == 40 ||		//The size of a Windows NT, 3.1x, or later header  (Adds 16bpp and 32bpp formats.  Adds RLE compression...  Probably might support the different color formats.)
				sizeOfHeader == 52 ||		//The size of some strange undocumented header  (Adds RGB bit masks, Adobe's doing here.)
				sizeOfHeader == 56 ||		//The size of another strange header by Adobe.  (Add alpha channel bit masks.)
				sizeOfHeader == 108 ||		//The size of a Windows NT 4.0, 95, or later header
				sizeOfHeader == 124))		//The size of a Windows NT 5.0, 98, or later header
			{
				//This header size is not a known size
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Bitmap Info Header not of a known size.\n", targetFilename);
				return -4;
			}

			//Check to make sure the header is add least size 40 or larger.  Most formats that use smaller are absolutely ancient.
			if (sizeOfHeader < 40)
			{
				//This is not a supported version at the moment.
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Bitmap Info Header if of an unsupported version.\n", targetFilename);
				return -5;
			}

			//At this point we should have enough header information to figure out the file properly

			//Read in the bitmap width
			readCount += fread(&width, 1, 4, bitmapFile);

			//Read in the bitmap height
			readCount += fread(&height, 1, 4, bitmapFile);

			//Read in the color panes
			readCount += fread(&colorPanes, 1, 2, bitmapFile);

			//Check color panes to make sure it is a value of one, otherwise there's probably something wrong with the file
			if (colorPanes != 1)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Color Panes value is not equal to 1, when it should be.\n", targetFilename);
				return -6;
			}

			//Read in the bits per pixel
			//Right now we only support 24bpp, but support for other versions is not out of the question
			readCount += fread(&bitsPerPixel, 1, 2, bitmapFile);

			//Check to make sure the bits per pixel is something sensible
			if (!(						//If not at one the below:
				bitsPerPixel == 1 ||	//Monochrome
				bitsPerPixel == 4 ||	//16 Color
				bitsPerPixel == 8 ||	//256 Color
				bitsPerPixel == 16 ||	//High Color
				bitsPerPixel == 24 ||	//True Color
				bitsPerPixel == 32))	//True Color with Alpha
			{
				//This doesn't have a proper bits per pixel value
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Bits Per Pixel value is not 1, 4, 8, 16, 24, or 32.\n", targetFilename);
				return -7;
			}

			//We are only supporting 24bpp at the moment, so error out if someone tries to us anything else right now
			if (bitsPerPixel != 24)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - Only 24bpp BMPs supported right now.\n", targetFilename);
				return -8;
			}

			//Read in compression method in use
			readCount += fread(&compressionMethod, 1, 4, bitmapFile);

			//We don't support any kind of compression, so if it's anything other than a 0, we can't process it.
			if (compressionMethod != 0)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - No compression is presently supported\n", targetFilename);
				return -9;
			}

			//
			//  This data header information is useful for other purposes that may be supported in the future.
			//

			//Read in the size of the raw bitmap data size, uncompressed bitmaps can use a dummy 0 value.
			readCount += fread(&dataSize, 1, 4, bitmapFile);

			//Read in the horizontal resolution of the image
			readCount += fread(&horizontalResolution, 1, 4, bitmapFile);

			//Read in the vertical resoltuion of the image
			readCount += fread(&verticalResolution, 1, 4, bitmapFile);

			//Read in the number of colors in the palette  Useful for other smaller bbp formats
			readCount += fread(&colorsInPalette, 1, 4, bitmapFile);

			//Read in number of important colors used.  It's there, not often used... usually ignored.
			readCount += fread(&importantColors, 1, 4, bitmapFile);


			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount < BITMAP_DATA_HEADER_SIZE)
			{
				std::fprintf(stderr, "Bitmap File Error:  File \"%s\" is not correct format - File too small - Incomplete Bitmap Data Header, size was %i.\n", targetFilename, readCount);
				return -10;
			}


			//
			//  Print out some common debug data
			//

			printf("DEBUG: Bitmap Load - \"%s\" - Bitmap Data Offset: %d\n", targetFilename, offset);
			printf("DEBUG: Bitmap Load - \"%s\" - Header Size: %d\n", targetFilename, sizeOfHeader);
			printf("DEBUG: Bitmap Load - \"%s\" - Height: %d\n", targetFilename, height);
			printf("DEBUG: Bitmap Load - \"%s\" - Width: %d\n", targetFilename, width);
			printf("DEBUG: Bitmap Load - \"%s\" - Size: %d\n", targetFilename, dataSize);


			//
			//  Go to the file offset where the image data starts
			//
			fseek(bitmapFile, offset, SEEK_SET);

			//Create a spot to put the image data
			imageData = new SGE::Display::Video::pixel[height * width];

			//Set the other aspects of the image
			imageDataSize = height * width;

			//
			//  Load data from image data section
			//  Bitmaps have reverse row ordering and other strangeness
			//  For... Reasons...
			//

			//Start at the start of the bottom row and move backwards
			for (int j = (height - 1) * width; j >= 0; j = j - width)
			{
				//Go through the row data and process it to the corrected format
				for (int i = 0; i < width; i++)
				{
					//Pull the pixel bytes
					//Currently works for 24 bits per pixel RGB BMPs
					fread(&pixelBlue, 1, 1, bitmapFile);
					fread(&pixelGreen, 1, 1, bitmapFile);
					fread(&pixelRed, 1, 1, bitmapFile);

					//Rearrange the pixel color data to something we can use
					imageData[j + i] = ((uint32_t)pixelRed) | ((uint32_t)pixelGreen << 8) | ((uint32_t)pixelBlue << 16);
				}
			}

			//If the whole process goes well, and has reached this point
			//Close file
			fclose(bitmapFile);

			//Signal all good
			return 0;
		}
	}
}