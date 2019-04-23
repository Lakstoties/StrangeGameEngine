#include "include\SGE\fileformats.h"
#include "include\SGE\system.h"
#include <iostream>
#include <fstream>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace FileFormats
	{
		//
		//  Bitmap File Definitions
		//

		const int BITMAP_FILE_HEADER_SIZE = 14;
		const int BITMAP_DATA_HEADER_SIZE = 40;

		Bitmap::~Bitmap()
		{
			//Check for iamgeData and delete it.
			if (imageData != nullptr)
			{
				delete imageData;
			}
		}


		Bitmap::Bitmap(char* targetFilename)
		{
			//FILE* bitmapFile;
			std::fstream bitmapFile;
			std::streamsize  readCount = 0;
			unsigned char pixelRed;
			unsigned char pixelGreen;
			unsigned char pixelBlue;

			//
			//  Open the File
			//

			//Attempt to open the bitmap file
			// Open as a input binary file
			bitmapFile.open(targetFilename, std::ios::in | std::ios::binary);

			//Check to see if we got a valid file pointer
			//if (bitmapFile == NULL)
			if (!bitmapFile)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  Cannot open file \"%s\"\n", targetFilename);
				return;
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
			bitmapFile.read((char *)&idField, 2);
			readCount += bitmapFile.gcount();

			//The ID field before continuing
			//If memcmp doesn't return a 0, something is different
			if (std::memcmp("BM", &idField, 2) != 0)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Incorrect ID field.\n", targetFilename);
				return;
			}

			//Read in the BMP field size in bytes
			//readCount += fread(&bmpSize, 1, 4, bitmapFile);
			bitmapFile.read((char*)&bmpSize, 4);
			readCount += bitmapFile.gcount();

			//Read some reserved data, application specific, not our concern really.
			bitmapFile.read((char*)& reserved1, 2);
			readCount += bitmapFile.gcount();

			//Read in some more reserved data, application specific, not our concern, moving on...
			bitmapFile.read((char*)& reserved2, 2);
			readCount += bitmapFile.gcount();

			//Read in the offset that should indicate where the file data should begin
			bitmapFile.read((char*)& offset, 4);
			readCount += bitmapFile.gcount();


			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount < BITMAP_FILE_HEADER_SIZE)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - File header too small.  Read count: %i\n", targetFilename, readCount);
				return;
			}

			//
			//  Read Bitmap Info header
			//

			//Read in the size of the Info header
			//This can be used to determine which header is in use.  But, we only need the most basic header info, so this will just help us confirm the data offset value

			//Also reset the read count with the value from this read
			readCount = 0;

			bitmapFile.read((char*)& sizeOfHeader, 4);
			readCount += bitmapFile.gcount();
			

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
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Bitmap Info Header not of a known size.\n", targetFilename);
				return;
			}

			//Check to make sure the header is add least size 40 or larger.  Most formats that use smaller are absolutely ancient.
			if (sizeOfHeader < 40)
			{
				//This is not a supported version at the moment.
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Bitmap Info Header if of an unsupported version.\n", targetFilename);
				return;
			}

			//At this point we should have enough header information to figure out the file properly

			//Read in the bitmap width
			bitmapFile.read((char*)&width, 4);
			readCount += bitmapFile.gcount();

			//Read in the bitmap height
			bitmapFile.read((char*)&height, 4);
			readCount += bitmapFile.gcount();

			//Read in the color panes
			bitmapFile.read((char*)&colorPanes, 2);
			readCount += bitmapFile.gcount();

			//Check color panes to make sure it is a value of one, otherwise there's probably something wrong with the file
			if (colorPanes != 1)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Color Panes value is not equal to 1, when it should be.\n", targetFilename);
				return;
			}

			//Read in the bits per pixel
			//Right now we only support 24bpp, but support for other versions is not out of the question
			bitmapFile.read((char*)&bitsPerPixel, 2);
			readCount += bitmapFile.gcount();

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
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Bits Per Pixel value is not 1, 4, 8, 16, 24, or 32.\n", targetFilename);
				return;
			}

			//We are only supporting 24bpp at the moment, so error out if someone tries to us anything else right now
			if (bitsPerPixel != 24)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - Only 24bpp BMPs supported right now.\n", targetFilename);
				return;
			}

			//Read in compression method in use
			bitmapFile.read((char*)&compressionMethod, 4);
			readCount += bitmapFile.gcount();
			

			//We don't support any kind of compression, so if it's anything other than a 0, we can't process it.
			if (compressionMethod != 0)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - No compression is presently supported\n", targetFilename);
				return;
			}

			//
			//  This data header information is useful for other purposes that may be supported in the future.
			//

			//Read in the size of the raw bitmap data size, uncompressed bitmaps can use a dummy 0 value.
			bitmapFile.read((char*)& dataSize, 4);
			readCount += bitmapFile.gcount();

			//Read in the horizontal resolution of the image
			bitmapFile.read((char*)& horizontalResolution, 4);
			readCount += bitmapFile.gcount();

			//Read in the vertical resoltuion of the image
			bitmapFile.read((char*)& verticalResolution, 4);
			readCount += bitmapFile.gcount();

			//Read in the number of colors in the palette  Useful for other smaller bbp formats
			bitmapFile.read((char*)& colorsInPalette, 4);
			readCount += bitmapFile.gcount();

			//Read in number of important colors used.  It's there, not often used... usually ignored.
			bitmapFile.read((char*)& importantColors, 4);
			readCount += bitmapFile.gcount();


			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount < BITMAP_DATA_HEADER_SIZE)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"%s\" is not correct format - File too small - Incomplete Bitmap Data Header, size was %i.\n", targetFilename, readCount);
				return;
			}


			//
			//  Print out some common debug data
			//

			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"%s\" - Bitmap Data Offset: %d\n", targetFilename, offset);
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"%s\" - Header Size: %d\n", targetFilename, sizeOfHeader);
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"%s\" - Height: %d\n", targetFilename, height);
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"%s\" - Width: %d\n", targetFilename, width);
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"%s\" - Size: %d\n", targetFilename, dataSize);


			//
			//  Go to the file offset where the image data starts
			//
			//fseek(bitmapFile, offset, SEEK_SET);
			bitmapFile.seekg(offset);

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
				for (int i = 0; i < (int)width; i++)
				{
					//Pull the pixel bytes
					//Currently works for 24 bits per pixel RGB BMPs
					bitmapFile.read((char*)&pixelBlue, 1);

					//fread(&pixelGreen, 1, 1, bitmapFile);
					bitmapFile.read((char*)&pixelGreen, 1);

					//fread(&pixelRed, 1, 1, bitmapFile);
					bitmapFile.read((char*)&pixelRed, 1);

					//Rearrange the pixel color data to something we can use
					imageData[j + i] = ((uint32_t)pixelRed) | ((uint32_t)pixelGreen << 8) | ((uint32_t)pixelBlue << 16);
				}
			}

			//If the whole process goes well, and has reached this point
			//Close file
			bitmapFile.close();

			//Signal all good
			return;
		}


		//
		//  Wave File Definitions
		//

		const int WAVE_FILE_HEADER_SIZE = 12;
		const int WAVE_FILE_SUBCHUNK_HEADER_SIZE = 8;

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
		Wave::Wave(char* targetFilename)
		{
			std::fstream soundFile;
			std::streamsize readCount = 0;
			bool keepLookingThroughSubchunks = true;


			soundFile.open(targetFilename, std::ios::in | std::ios::binary);

			//Check to see if the file is even there.
			if (!soundFile)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  Cannot open file \"%s\"\n", targetFilename);
				return;
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
			soundFile.read(chunkID, 4);
			readCount += soundFile.gcount();

			//
			//  Check for a valid chunkID
			//

			if (memcmp("RIFF", &chunkID, 4) != 0)
			{
				//No RIFF header... Not the format we are looking for!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect header.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}

			//
			//Offset: 4		Size: 4		ChunkSize:				Check the chunk size and save it for double checking purposes
			//

			//readCount += fread(&chunkSize, 1, 4, soundFile);
			soundFile.read((char*)&chunkSize, 4);
			readCount += soundFile.gcount();


			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: %s - Chunk Size: %d\n", targetFilename, chunkSize);

			//
			//Offset: 8		Size: 4		Format:		Check format for "WAVE"
			//

			//readCount += fread(&format, 1, 4, soundFile);
			soundFile.read((char*)&format, 4);
			readCount += soundFile.gcount();

			if (memcmp("WAVE", &format, 4) != 0)
			{
				//No WAVE format... Not the format we are looking for!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Incorrect Encoding Format.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}

			//Check to see if we actually read enough bytes to make up a proper wave file header
			if (readCount != WAVE_FILE_HEADER_SIZE)
			{
				//This file is way to small to be a proper wav file
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				soundFile.close();
				return;
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
				//readCount += fread(&subChunkID, 1, 4, soundFile);
				soundFile.read((char*)&subChunkID, 4);
				readCount += soundFile.gcount();

				//
				//Offset: 16	Size: 4		Subchunksize:			For PCM files, it should be 16, otherwise this is probably a different format
				//
				//readCount += fread(&subChunkSize, 1, 3, soundFile);
				soundFile.read((char*)&subChunkSize, 4);
				readCount += soundFile.gcount();

				//Is this the "fmt " subchunk
				if (memcmp("fmt", &subChunkID, 3) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					//if (fseek(soundFile, subChunkSize, SEEK_CUR) != 0)
					if (soundFile.seekg(subChunkSize,std::ios::cur))
					{
						SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

						//Close out the file
						soundFile.close();
						return;
					}
				}

				//If we hit end of file early
				if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing fmt subchunk.\n", targetFilename);

					//Close out the file
					soundFile.close();
					return;
				}
			}

			//Check the subchunk size
			if (subChunkSize != 16)
			{
				//The format chunk size should be 16 bytes, if not... This isn't right
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}


			//Confirmed a good subChunkHeader, read in the data
			//Reset Read Count
			readCount = 0;

			//
			//Offset: 20	Size: 2		Audio Format:			Should be 1 for PCM.  If not 1, then probably another format we aren't wanting.
			//
			//readCount += fread(&audioFormat, 1, 2, soundFile);
			soundFile.read((char*)&audioFormat, 2);
			readCount += soundFile.gcount();
			

			if (audioFormat != 1)
			{
				//The format audio format should be 1, if not...  Abort!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Format Chunk Size Incorrect.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}

			//
			//Offset: 22	Size: 2		Number of Channels:		1 = Mono, 2 = Stereo, and so forth.   We are looking for Mono channels currently,
			//
			//readCount += fread(&numberOfChannels, 1, 2, soundFile);
			soundFile.read((char*)& numberOfChannels, 2);
			readCount += soundFile.gcount();

			//May support more channels later
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: %s - Number of Channels: %d\n", targetFilename, numberOfChannels);

			//
			//Offset: 24	Size: 4		Sample Rate
			//
			//readCount += fread(&sampleRate, 1, 4, soundFile);
			soundFile.read((char*)& sampleRate, 4);
			readCount += soundFile.gcount();

			//Sample Rate of the data.  Currently looking for 44100.
			//May implement resampling in future, but not right now.
			if (sampleRate != 44100)
			{
				//Not 44100Hz sample rate...  Not exactly an error, but no support for other sample rates at the moment.
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" has an unsupported sample rate.  44100Hz Only.  Sorry.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}

			//Offset: 28	Size: 4		Byte Rate
			//Equals to:  Sample Rate * Number of Channels * Bits Per Sample / 8
			//The number of bytes per second
			//readCount += fread(&byteRate, 1, 4, soundFile);
			soundFile.read((char*)& byteRate, 4);
			readCount += soundFile.gcount();

			//Offset: 32	Size: 2		Block Alignment
			//Equals to:  Numbers of Channels * Bits Per Sample / 8
			//The number of bytes per frame/sample
			//readCount += fread(&blockAlignment, 1, 2, soundFile);
			soundFile.read((char*)& blockAlignment, 2);
			readCount += soundFile.gcount();

			//Offset: 34	Size: 2		Bits Per Sample
			//Bits of data per sample
			//We presently want to see 16 bits.
			//readCount += fread(&bitsPerSample, 1, 2, soundFile);
			soundFile.read((char*)& bitsPerSample, 2);
			readCount += soundFile.gcount();

			if (bitsPerSample != 16)
			{
				//Not 16 bit sample depth...  Not exactly an error, but no support for other bit rates at the moment.
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" has an unsupported bit rate.  16-bit Only.  Sorry.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}

			//If we hit end of file early
			if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
			{
				//The format audio format should be 1, if not...  Abort!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - fmt subchunk too small.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
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
				//readCount += fread(&subChunkID, 1, 4, soundFile);
				soundFile.read((char*)&subChunkID, 4);
				readCount += soundFile.gcount();

				//
				//Offset: 40	Size: 4		Subchunk 2 Size
				//
				//readCount += fread(&subChunkSize, 1, 3, soundFile);
				soundFile.read((char*)& subChunkSize, 4);
				readCount += soundFile.gcount();

				//Is this the "data" subchunk
				if (memcmp("data", &subChunkID, 4) == 0)
				{
					//We found it!  Move along!
					keepLookingThroughSubchunks = false;
				}
				else
				{
					//Advance past this section of the file
					//if (fseek(soundFile, subChunkSize, SEEK_CUR) != 0)
					soundFile.seekg(subChunkSize, std::ios::cur);
					if (!soundFile)
					{
						SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

						//Close out the file
						soundFile.close();
						return;
					}
				}


				//If we hit end of file early
				if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
				{
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - Missing data subchunk.\n", targetFilename);

					//Close out the file
					soundFile.close();
					return;
				}
			}


			//If we hit end of file early
			if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
			{
				//The format audio format should be 1, if not...  Abort!
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"%s\" is not correct format - data subchunk too small.\n", targetFilename);

				//Close out the file
				soundFile.close();
				return;
			}


			//Data that indicates the size of the data chunk
			//Equal to:  Number of Samples * Number of Channels * Bits Per Sample / 8
			//Indicates the amount to read after this chunk

			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: %s - Data Size: %d\n", targetFilename, subChunkSize);


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
					//readCount += fread(&audioData[j][i], 2, 1, soundFile);
					soundFile.read((char*)&audioData[j][i], 2);
					readCount += soundFile.gcount();
				}
			}

			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: %s - Data Bytes Read: %zd\n", targetFilename, readCount * 2);

			//If we get here... It's all good!... Maybe... Hoepfully?

			//Close out the sound file
			soundFile.close();
			return;
		}

		//
		//
		//  Sound System Module File Definitions
		//
		//

		ModuleFile::ModuleFile()
		{

		}


		ModuleFile::~ModuleFile()
		{

		}


		//
		//  Function to load data from mod fule
		//
		int ModuleFile::LoadFile(char* targetFilename)
		{
			FILE* moduleFile;
			size_t readCount = 0;
			size_t totalReadCount = 0;
			unsigned char readBuffer[8];

			//
			//
			//Attempt to open the file.
			//
			//

			moduleFile = fopen(targetFilename, "rb");

			//Check to see if the file is even there.
			if (moduleFile == NULL)
			{
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//
			//
			//Start parsing through the file and grabbing all the data.
			//
			//

			//
			//Read the module file title info
			//

			readCount = fread(&title, 1, 20, moduleFile);
			totalReadCount += readCount;

			//DEBUG:  Current number of bytes read
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Post Title: Read Count: %d \n", totalReadCount);

			//
			//Read the module file's sample data
			//

			for (int i = 0; i < 31; i++)
			{
				//Read sample title
				readCount = fread(&samples[i].title, 1, 22, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 22)
				{
					//This file is way to small to be a proper wav file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -2;
				}

				//Read sample length
				readCount = fread(&samples[i].lengthInWords, 1, 2, moduleFile);
				totalReadCount += readCount;

				//Flip the bits around since Amigas were Big Endian machines.
				samples[i].lengthInWords = (((samples[i].lengthInWords & 0x00FF) << 8) | ((samples[i].lengthInWords & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way too small to be a proper mod file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -3;
				}

				//Read sample finetune
				readCount = fread(&samples[i].finetune, 1, 1, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 1)
				{
					//This file is way to small to be a proper wav file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -4;
				}

				//Read sample volume
				readCount = fread(&samples[i].volume, 1, 1, moduleFile);
				totalReadCount += readCount;

				//Check to see if we actually read enough bytes
				if (readCount != 1)
				{
					//This file is way to small to be a proper wav file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -5;
				}

				//Read repeat start offset
				readCount = fread(&samples[i].repeatOffset, 1, 2, moduleFile);
				totalReadCount += readCount;

				samples[i].repeatOffset = (((samples[i].repeatOffset & 0x00FF) << 8) | ((samples[i].repeatOffset & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way to small to be a proper wav file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -6;
				}

				//Read repeat length
				readCount = fread(&samples[i].repeatLength, 1, 2, moduleFile);
				totalReadCount += readCount;

				samples[i].repeatLength = (((samples[i].repeatLength & 0x00FF) << 8) | ((samples[i].repeatLength & 0xFF00) >> 8));

				//Check to see if we actually read enough bytes
				if (readCount != 2)
				{
					//This file is way to small to be a proper wav file
					SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
					return -7;
				}
			}

			//DEBUG: Check current read count
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Post Sample Headers: Current Read Count: %d \n", totalReadCount);

			//
			//Read number of song positions
			//

			readCount = fread(&songPositions, 1, 1, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 1)
			{
				//This file is way to small to be a proper wav file
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -8;
			}

			//Burn through a read
			readCount = fread(&readBuffer, 1, 1, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 1)
			{
				//This file is way to small to be a proper wav file
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -9;
			}

			//
			//Read the pattern table
			//

			readCount = fread(&patternTable, 1, 128, moduleFile);
			totalReadCount += readCount;

			//DEBUG: Check current read count
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: %d \n", totalReadCount);

			//Check to see if we actually read enough bytes
			if (readCount != 128)
			{
				//This file is way to small to be a proper wav file
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -10;
			}

			//
			//Read some tag info
			//

			readCount = fread(&readBuffer, 1, 4, moduleFile);
			totalReadCount += readCount;

			//Check to see if we actually read enough bytes
			if (readCount != 4)
			{
				//This file is way to small to be a proper wav file
				SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
				return -11;
			}

			//Check to see if there's anything special
			//Check for "M.K."
			if (memcmp(&readBuffer, "M.K.", 4) == 0)
			{
				//Detect M.K. Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "M.K. Module File signature detected.\n");
			}

			//Check for "FLT4"
			else if (memcmp(&readBuffer, "FLT4", 4) == 0)
			{
				//Detect FLT4 Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "FLT4 Module File signature detected.\n");
			}

			//Check for "FLT8"
			else if (memcmp(&readBuffer, "FLT8", 4) == 0)
			{
				//Detect FLT8 Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "FLT8 Module File signature detected.\n");
			}

			//Check for "4CHN"
			else  if (memcmp(&readBuffer, "4CHN", 4) == 0)
			{
				//Detect 4CHN Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "4CHN Module File signature detected.\n");
			}

			//Check for "6CHN"
			else if (memcmp(&readBuffer, "6CHN", 4) == 0)
			{
				//Detect 6CHN Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "6CHN Module File signature detected.\n");
			}

			//Check for "8CHN"
			else if (memcmp(&readBuffer, "8CHN", 4) == 0)
			{
				//Detect 8CHN Signature
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "8CHN Module File signature detected.\n");
			}

			//No letters found
			else
			{
				//Move the seek back since this is part of the pattern data
				fseek(moduleFile, -4, SEEK_CUR);
				totalReadCount -= 4;
				SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "No Module File signature detected.\n");
			}

			//
			//Figure out how many patterns there are to read.
			//

			//Go through the pattern table and find the largest number.
			for (int i = 0; i < 128; i++)
			{
				if (patternTable[i] > numberOfPatterns)
				{
					numberOfPatterns = patternTable[i];
					SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current number of patterns: %d\n", numberOfPatterns);
				}
			}

			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Module Load: %d patterns found.\n", numberOfPatterns);

			//
			//Start churning through all the pattern data
			//

			//Go through each pattern
			for (int i = 0; i < numberOfPatterns + 1; i++)
			{
				//Go through each division
				for (int j = 0; j < 64; j++)
				{
					//Go through each channel's data
					for (int k = 0; k < 4; k++)
					{
						//Read the 4 bytes of data
						fread(&readBuffer, 1, 4, moduleFile);
						totalReadCount += readCount;

						//Check to see if we actually read enough bytes
						if (readCount != 4)
						{
							//This file is way to small to be a proper wav file
							SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.\n", targetFilename);
							ProperlyLoaded = true;
							return -12;
						}

						//
						//Parse out the data.
						//

						//Copy the first two bytes over into period
						patterns[i][j][k].period = ((readBuffer[0] & 0x0F) << 8) | (readBuffer[1]);

						//Copy the second two bytes over into effect
						patterns[i][j][k].effect = ((readBuffer[2] & 0x0F) << 8) | (readBuffer[3]);

						//Bit mask unneeded bits, shift, and add them together
						patterns[i][j][k].sample = (readBuffer[0] & 0xF0) | ((readBuffer[2] & 0xF0) >> 4);
					}
				}
			}

			//DEBUG: Check current read count
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: %d \n", totalReadCount);

			//
			//Load up the samples with their data
			//

			for (int i = 0; i < 31; i++)
			{
				if (samples[i].lengthInWords > 1)
				{
					//Read the data in there
					readCount = fread(samples[i].data, 1, samples[i].SampleLengthInBytes(), moduleFile);
					totalReadCount += readCount;

					//Check to see if we actually read enough bytes
					if (readCount != samples[i].SampleLengthInBytes())
					{
						//This file is way to small to be a proper wav file
						SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"%s\" is not correct format - File Too Small to be proper.  Sample Data Reported: %d  Sample Data Read: %d \n", targetFilename, samples[i].lengthInWords * 2, readCount);
						//return -13;
					}
				}
			}

			//DEBUG: Check current read count
			SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: %d \n", totalReadCount);
			

			//
			//Close the file out
			//

			fclose(moduleFile);

			//If we get to this point, everything is okay
			ProperlyLoaded = true;
			return 0;
		}

		bool ModuleFile::MODSample::ConvertSampleTo16Bit(SGE::Sound::sampleType* target)
		{
			if (lengthInWords > 1)
			{
				//Go through the module sample and convert the 8-bit to 16-bit range
				//Store it in the destinationBuffer
				for (int i = 0; i < (lengthInWords * 2); i++)
				{
					target[i] = SGE::Sound::sampleType(this->data[i] << 8);
				}
				return true;
			}
			else
			{
				return false;
			}
			
		}

		int ModuleFile::MODSample::SampleLengthInBytes()
		{
			return this->lengthInWords * 2;
		}
	}
}