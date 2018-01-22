#include "include\SGE\fileformats.h"

namespace SGE
{
	namespace FileFormats
	{
		//
		//  Bitmap File Definitions
		//

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

		//
		//  Wave File Definitions
		//

		const int WAVE_FILE_HEADER_SIZE = 12;
		const int WAVE_FILE_SUBCHUNK_HEADER_SIZE = 8;

		//Default constructor for a SoundSystemWaveFile
		Wave::Wave()
		{
		}

		//Deconstructor for a SoundSystemWaveFile
		Wave::~Wave()
		{
			//If audio data actually got loaded, delete it!
			if (audioData != nullptr)
			{
				for (unsigned int i = 0; i < numberOfChannels; i++)
				{
					//Delete the buffer for that channel's data
					delete audioData[i];
				}

				//Finally delete the data for the array of buffer pointers
				delete audioData;
			}
		}
		//Load audio data from a file into a collection of audio buffers
		//Returns a 0 if all is well, something else if there is an error.
		int Wave::LoadFile(char* targetFilename)
		{
			FILE* soundFile;
			size_t readCount = 0;
			bool keepLookingThroughSubchunks = true;

			soundFile = fopen(targetFilename, "rb");

			//
			//Check to see if the file is even there.
			//

			if (soundFile == NULL)
			{
				fprintf(stderr, "Sound System Wave File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//
			//Start parsing the file and check to see if it is valid or not
			//

			//
			//Try to read a RIFF header in
			//

			//Initialize readCount
			readCount = 0;

			//
			//Offset: 0		Size: 4		ChunkID:				Check the chunk ID to see if it says RIFF in ASCII
			//
			readCount += fread(&chunkID, 1, 4, soundFile);

			//
			//  Check for a valid chunkID
			//

			if (memcmp("RIFF", &chunkID, 4) != 0)
			{
				//No RIFF header... Not the format we are looking for!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect header.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -10;
			}

			//
			//Offset: 4		Size: 4		ChunkSize:				Check the chunk size and save it for double checking purposes
			//

			readCount += fread(&chunkSize, 1, 4, soundFile);

			printf("DEBUG: Sound System Wave File: %s - Chunk Size: %d\n", targetFilename, chunkSize);

			//
			//Offset: 8		Size: 4		Format:		Check format for "WAVE"
			//

			readCount += fread(&format, 1, 4, soundFile);

			if (memcmp("WAVE", &format, 4) != 0)
			{
				//No WAVE format... Not the format we are looking for!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect Encoding Format.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -20;
			}

			//Check to see if we actually read enough bytes to make up a proper wave file header
			if (readCount != WAVE_FILE_HEADER_SIZE)
			{
				//This file is way to small to be a proper wav file
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -2;
			}


			//
			//Start going through the subchunks looking for the "fmt " subchunk
			//

			while (keepLookingThroughSubchunks)
			{
				//Reset the readCount
				readCount = 0;

				//
				//Offset: 12	Size: 4		SubchunkID:				Look for the "fmt " (space is null)
				//
				readCount += fread(&subChunkID, 1, 4, soundFile);

				//
				//Offset: 16	Size: 4		Subchunksize:			For PCM files, it should be 16, otherwise this is probably a different format
				//
				readCount += fread(&subChunkSize, 1, 3, soundFile);

				//Is this the "fmt " subchunk
				if (memcmp("fmt", &subChunkID, 3) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					if (fseek(soundFile, subChunkSize, SEEK_CUR) != 0)
					{
						fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

						//Close out the file
						fclose(soundFile);

						return -30;
					}
				}

				//If we hit end of file early
				if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
				{
					fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

					//Close out the file
					fclose(soundFile);

					return -30;
				}
			}

			//Check the subchunk size
			if (subChunkSize != 16)
			{
				//The format chunk size should be 16 bytes, if not... This isn't right
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -40;
			}


			//Confirmed a good subChunkHeader, read in the data
			//Reset Read Count
			readCount = 0;

			//
			//Offset: 20	Size: 2		Audio Format:			Should be 1 for PCM.  If not 1, then probably another format we aren't wanting.
			//
			readCount += fread(&audioFormat, 1, 2, soundFile);

			if (audioFormat != 1)
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -50;
			}

			//
			//Offset: 22	Size: 2		Number of Channels:		1 = Mono, 2 = Stereo, and so forth.   We are looking for Mono channels currently,
			//
			readCount += fread(&numberOfChannels, 1, 2, soundFile);

			//May support more channels later
			printf("DEBUG: Sound System Wave File: %s - Number of Channels: %d\n", targetFilename, numberOfChannels);

			//
			//Offset: 24	Size: 4		Sample Rate
			//
			readCount += fread(&sampleRate, 1, 4, soundFile);

			//Sample Rate of the data.  Currently looking for 44100.
			//May implement resampling in future, but not right now.
			if (sampleRate != 44100)
			{
				//Not 44100Hz sample rate...  Not exactly an error, but no support for other sample rates at the moment.
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" has an unsupported sample rate.  44100Hz Only.  Sorry.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -70;
			}

			//Offset: 28	Size: 4		Byte Rate
			//Equals to:  Sample Rate * Number of Channels * Bits Per Sample / 8
			//The number of bytes per second
			readCount += fread(&byteRate, 1, 4, soundFile);

			//Offset: 32	Size: 2		Block Alignment
			//Equals to:  Numbers of Channels * Bits Per Sample / 8
			//The number of bytes per frame/sample
			readCount += fread(&blockAlignment, 1, 2, soundFile);

			//Offset: 34	Size: 2		Bits Per Sample
			//Bits of data per sample
			//We presently want to see 16 bits.
			readCount += fread(&bitsPerSample, 1, 2, soundFile);

			if (bitsPerSample != 16)
			{
				//Not 16 bit sample depth...  Not exactly an error, but no support for other bit rates at the moment.
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" has an unsupported bit rate.  16-bit Only.  Sorry.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -100;
			}

			//If we hit end of file early
			if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - fmt subchunk too small.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -45;
			}


			//
			//Reset the logic
			//

			keepLookingThroughSubchunks = true;

			//
			//Start going through the subchunks looking for the "data" subchunk
			//

			while (keepLookingThroughSubchunks)
			{
				//Reset the readCount
				readCount = 0;

				//Second chunk of data: "data"
				//Offset: 36	Size: 4		Subchunk 2 ID
				//ID to indicate the next subchunk of data, if it isn't "data" something is wrong
				readCount += fread(&subChunkID, 1, 4, soundFile);

				//
				//Offset: 40	Size: 4		Subchunk 2 Size
				//
				readCount += fread(&subChunkSize, 1, 3, soundFile);

				//Is this the "data" subchunk
				if (memcmp("data", &subChunkID, 4) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					if (fseek(soundFile, subChunkSize, SEEK_CUR) != 0)
					{
						fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

						//Close out the file
						fclose(soundFile);

						return -105;
					}
				}


				//If we hit end of file early
				if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
				{
					fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

					//Close out the file
					fclose(soundFile);

					return -105;
				}
			}


			//If we hit end of file early
			if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
			{
				//The format audio format should be 1, if not...  Abort!
				fprintf(stderr, "Sound System Wave File Error:  File \"%s\" is not correct format - data subchunk too small.\n", targetFilename);

				//Close out the file
				fclose(soundFile);

				return -105;
			}


			//Data that indicates the size of the data chunk
			//Equal to:  Number of Samples * Number of Channels * Bits Per Sample / 8
			//Indicates the amount to read after this chunk

			printf("DEBUG: Sound System Wave File: %s - Data Size: %d\n", targetFilename, subChunkSize);


			//
			//Offset: 44	Size: *?	Data
			//

			//Actual sound data
			//Calculate the number of Samples
			numberOfSamples = subChunkSize / blockAlignment;

			//Create buffers to hold the data
			//Create the pointer array for the channel buffers
			audioData = new short*[numberOfChannels];

			//Create short arrays to hold the channel data
			for (unsigned int i = 0; i < numberOfChannels; i++)
			{
				audioData[i] = new short[numberOfSamples];
			}

			//Read the data in
			readCount = 0;

			//For each sample
			for (unsigned int i = 0; i < numberOfSamples; i++)
			{
				//For each channel
				for (unsigned int j = 0; j < numberOfChannels; j++)
				{
					readCount += fread(&audioData[j][i], 2, 1, soundFile);
				}
			}

			printf("DEBUG: Sound System Wave File: %s - Data Bytes Read: %zd\n", targetFilename, readCount * 2);

			//If we get here... It's all good!... Maybe... Hoepfully?

			//Close out the sound file
			fclose(soundFile);

			return 0;
		}
	}
}