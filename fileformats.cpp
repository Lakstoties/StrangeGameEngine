#include "include\SGE\fileformats.h"
#include "include\SGE\system.h"
#include <iostream>
#include <fstream>
#include <cstring>

//
//  Strange Game Engine Main Namespace
//
namespace SGE::FileFormats
{
    //
    //  Bitmap File Definitions
    //

    const int BITMAP_FILE_HEADER_SIZE = 14;
    const int BITMAP_DATA_HEADER_SIZE = 40;

    Bitmap::~Bitmap() = default;

    Bitmap::Bitmap(const std::string& targetFilename)
    {
        std::fstream bitmapFile;
        std::streamsize  readCount;
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
        if (!bitmapFile)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  Cannot open file \"" + targetFilename +"\"\n");
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

        //Read in the ID field.  We are looking for a "BM" to indicate it is a typical BMP/Bitmap
        bitmapFile.read((char *)&idField, 2);
        readCount += bitmapFile.gcount();

        //The ID field before continuing
        //If memcmp doesn't return a 0, something is different
        if (std::memcmp("BM", &idField, 2) != 0)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Incorrect ID field.\n");
            return;
        }

        //Read in the BMP field size in bytes
        bitmapFile.read((char*) &bmpSize, 4);
        readCount += bitmapFile.gcount();

        //Read some reserved data, application specific, not our concern really.
        bitmapFile.read((char*) &reserved1, 2);
        readCount += bitmapFile.gcount();

        //Read in some more reserved data, application specific, not our concern, moving on...
        bitmapFile.read((char*) &reserved2, 2);
        readCount += bitmapFile.gcount();

        //Read in the offset that should indicate where the file data should begin
        bitmapFile.read((char*) &offset, 4);
        readCount += bitmapFile.gcount();

        //Check to see if we got a whole header, or if the end of file hit early
        if (readCount < BITMAP_FILE_HEADER_SIZE)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - File header too small.  Read count: " + std::to_string(readCount) + "\n");
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
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Bitmap Info Header not of a known size.\n");
            return;
        }

        //Check to make sure the header is add least size 40 or larger.  Most formats that use smaller are absolutely ancient.
        if (sizeOfHeader < 40)
        {
            //This is not a supported version at the moment.
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Bitmap Info Header if of an unsupported version.\n");
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
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Color Panes value is not equal to 1, when it should be.\n");
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
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Bits Per Pixel value is not 1, 4, 8, 16, 24, or 32.\n");
            return;
        }

        //We are only supporting 24bpp at the moment, so error out if someone tries to us anything else right now
        if (bitsPerPixel != 24)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - Only 24bpp BMPs supported right now.\n");
            return;
        }

        //Read in compression method in use
        bitmapFile.read((char*)&compressionMethod, 4);
        readCount += bitmapFile.gcount();


        //We don't support any kind of compression, so if it's anything other than a 0, we can't process it.
        if (compressionMethod != 0)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - No compression is presently supported\n");
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

        //Read in the vertical resolution of the image
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
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Bitmap File Error:  File \"" + targetFilename + "\" is not correct format - File too small - Incomplete Bitmap Data Header, size was " + std::to_string(readCount) + ".\n");
            return;
        }

        //  Print out some common debug data
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"" + targetFilename + "\" - Bitmap Data Offset: " + std::to_string(offset) + "\n");
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"" + targetFilename + "\" - Header Size: " + std::to_string(sizeOfHeader) + "\n");
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"" + targetFilename + "\" - Height: " + std::to_string(height) + "\n");
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"" + targetFilename + "\" - Width: " + std::to_string(width) + "\n");
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Bitmap Load - \"" + targetFilename + "\" - Size: " + std::to_string(dataSize) + "\n");


        //  Go to the file offset where the image data starts
        bitmapFile.seekg(offset);

        //Create a spot to put the image data
        imageData.resize(height * width);

        //
        //  Load data from image data section
        //  Bitmaps have reverse row ordering and other strangeness
        //  For... Reasons...
        //

        //Start at the start of the bottom row and move backwards
        for (int j = ((int)height - 1) * (int)width; j >= 0; j = j - (int)width)
        {
            //Go through the row data and process it to the corrected format
            for (int i = 0; i < (int)width; i++)
            {
                //Pull the pixel bytes
                //Currently works for 24 bits per pixel RGB BMPs
                bitmapFile.read((char*)&pixelBlue, 1);
                bitmapFile.read((char*)&pixelGreen, 1);
                bitmapFile.read((char*)&pixelRed, 1);

                //Rearrange the pixel color data to something we can use
                imageData[j + i] = ((unsigned int)pixelRed) | ((unsigned int)pixelGreen << 8u) | ((unsigned int)pixelBlue << 16u);
            }
        }

        // If we get here, close the file.  Things should be good...?
        bitmapFile.close();
    }


    //
    //  Wave File Definitions
    //

    const int WAVE_FILE_HEADER_SIZE = 12;
    const int WAVE_FILE_SUBCHUNK_HEADER_SIZE = 8;

    Wave::~Wave() = default;

    //Load audio data from a file into a collection of audio buffers
    //Returns a 0 if all is well, something else if there is an error.
    Wave::Wave(const std::string& targetFilename)
    {
        std::fstream soundFile;
        std::streamsize readCount;
        bool keepLookingThroughSubchunks = true;


        soundFile.open(targetFilename, std::ios::in | std::ios::binary);

        //Check to see if the file is even there.
        if (!soundFile)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  Cannot open file \"" + targetFilename + "\"\n");
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
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Incorrect header.\n");

            //Close out the file
            soundFile.close();
            return;
        }

        //
        //Offset: 4		Size: 4		ChunkSize:				Check the chunk size and save it for double checking purposes
        //
        soundFile.read((char*)&chunkSize, 4);
        readCount += soundFile.gcount();


        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: " + targetFilename + " - Chunk Size: " + std::to_string(chunkSize) + "\n");

        //
        //Offset: 8		Size: 4		Format:		Check format for "WAVE"
        //
        soundFile.read((char*)&format, 4);
        readCount += soundFile.gcount();

        if (memcmp("WAVE", &format, 4) != 0)
        {
            //No WAVE format... Not the format we are looking for!
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Incorrect Encoding Format.\n");

            //Close out the file
            soundFile.close();
            return;
        }

        //Check to see if we actually read enough bytes to make up a proper wave file header
        if (readCount != WAVE_FILE_HEADER_SIZE)
        {
            //This file is way to small to be a proper wav file
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
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
            soundFile.read((char*)&subChunkID, 4);
            readCount += soundFile.gcount();

            //
            //Offset: 16	Size: 4		Subchunksize:			For PCM files, it should be 16, otherwise this is probably a different format
            //
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
                if (soundFile.seekg(subChunkSize,std::ios::cur))
                {
                    SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Missing fmt subchunk.\n");

                    //Close out the file
                    soundFile.close();
                    return;
                }
            }

            //If we hit end of file early
            if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
            {
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" +  targetFilename + "\" is not correct format - Missing fmt subchunk.\n");

                //Close out the file
                soundFile.close();
                return;
            }
        }

        //Check the subchunk size
        if (subChunkSize != 16)
        {
            //The format chunk size should be 16 bytes, if not... This isn't right
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Format Chunk Size Incorrect.\n");

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
        soundFile.read((char*)&audioFormat, 2);
        readCount += soundFile.gcount();


        if (audioFormat != 1)
        {
            //The format audio format should be 1, if not...  Abort!
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Format Chunk Size Incorrect.\n");

            //Close out the file
            soundFile.close();
            return;
        }

        //
        //Offset: 22	Size: 2		Number of Channels:		1 = Mono, 2 = Stereo, and so forth.   We are looking for Mono channels currently,
        //
        soundFile.read((char*)& numberOfChannels, 2);
        readCount += soundFile.gcount();

        //May support more channels later
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: " + targetFilename + " - Number of Channels: " + std::to_string(numberOfChannels) + "\n");

        //
        //Offset: 24	Size: 4		Sample Rate
        //
        soundFile.read((char*)& sampleRate, 4);
        readCount += soundFile.gcount();

        //Sample Rate of the data.  Currently looking for 44100.
        //May implement resampling in future, but not right now.
        if (sampleRate != 44100)
        {
            //Not 44100Hz sample rate...  Not exactly an error, but no support for other sample rates at the moment.
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" has an unsupported sample rate.  44100Hz Only.  Sorry.\n");

            //Close out the file
            soundFile.close();
            return;
        }

        //Offset: 28	Size: 4		Byte Rate
        //Equals to:  Sample Rate * Number of Channels * Bits Per Sample / 8
        //The number of bytes per second
        soundFile.read((char*)& byteRate, 4);
        readCount += soundFile.gcount();

        //Offset: 32	Size: 2		Block Alignment
        //Equals to:  Numbers of Channels * Bits Per Sample / 8
        //The number of bytes per frame/sample
        soundFile.read((char*)& blockAlignment, 2);
        readCount += soundFile.gcount();

        //Offset: 34	Size: 2		Bits Per Sample
        //Bits of data per sample
        //We presently want to see 16 bits.
        soundFile.read((char*)& bitsPerSample, 2);
        readCount += soundFile.gcount();

        if (bitsPerSample != 16)
        {
            //Not 16 bit sample depth...  Not exactly an error, but no support for other bit rates at the moment.
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" has an unsupported bit rate.  16-bit Only.  Sorry.\n");

            //Close out the file
            soundFile.close();
            return;
        }

        //If we hit end of file early
        if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
        {
            //The format audio format should be 1, if not...  Abort!
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - fmt subchunk too small.\n");

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
            soundFile.read((char*)&subChunkID, 4);
            readCount += soundFile.gcount();

            //
            //Offset: 40	Size: 4		Subchunk 2 Size
            //
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
                soundFile.seekg(subChunkSize, std::ios::cur);
                if (!soundFile)
                {
                    SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Missing data subchunk.\n");

                    //Close out the file
                    soundFile.close();
                    return;
                }
            }


            //If we hit end of file early
            if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
            {
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - Missing data subchunk.\n");

                //Close out the file
                soundFile.close();
                return;
            }
        }


        //If we hit end of file early
        if (readCount < WAVE_FILE_SUBCHUNK_HEADER_SIZE)
        {
            //The format audio format should be 1, if not...  Abort!
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Wave File Error:  File \"" + targetFilename + "\" is not correct format - data subchunk too small.\n");

            //Close out the file
            soundFile.close();
            return;
        }


        //Data that indicates the size of the data chunk
        //Equal to:  Number of Samples * Number of Channels * Bits Per Sample / 8
        //Indicates the amount to read after this chunk

        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: " + targetFilename + " - Data Size: " + std::to_string(subChunkSize) + "\n");


        //
        //Offset: 44	Size: *?	Data
        //

        //Actual sound data
        //Calculate the number of Samples
        numberOfSamples = subChunkSize / blockAlignment;

        //Prepare storage for the channels of data
        audioData.resize(numberOfChannels);

        //Create short arrays to hold the channel data
        for (auto &channel : audioData)
        {
            channel.resize(numberOfSamples);
        }

        //Read the data in
        readCount = 0;

        for (unsigned int i = 0; i < numberOfSamples; i++)
        {
            //For each channel
            for (unsigned int j = 0; j < numberOfChannels; j++)
            {
                soundFile.read((char*)&audioData[j][i], 2);
                readCount += soundFile.gcount();
            }
        }

        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Sound System Wave File: " + targetFilename + " - Data Bytes Read: " + std::to_string(readCount * 2) + "\n");

        //If we get here... It's all good!... Maybe... Hopefully?

        //Close out the sound file
        soundFile.close();
    }

    //
    //
    //  Sound System Module File Definitions
    //
    //

    ModuleFile::ModuleFile() = default;

    ModuleFile::~ModuleFile() = default;


    //
    //  Function to load data from mod file
    //
    int ModuleFile::LoadFile(const std::string& targetFilename)
    {
        //FILE* moduleFile;
        std::fstream moduleFile;

        size_t totalReadCount = 0;
        unsigned char readBuffer[8];

        //
        //
        //Attempt to open the file.
        //
        //
        moduleFile.open(targetFilename,std::ios::in | std::ios::binary);

        //Check to see if the file is even there.
        if (!moduleFile)
        {
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File Error:  Cannot open file \"" + targetFilename + "\"\n");
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
        moduleFile.read((char *)&title, 20);
        totalReadCount += moduleFile.gcount();

        //DEBUG:  Current number of bytes read
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Post Title: Read Count: " + std::to_string(totalReadCount) + " \n");

        //
        //Read the module file's sample data
        //

        for (auto & sample : samples)
        {
            //Read sample title
            moduleFile.read((char*)&sample.title, 22);
            totalReadCount += moduleFile.gcount();

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 22)
            {
                //This file is way to small to be a proper wav file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -2;
            }

            //Read sample length
            moduleFile.read((char *)&sample.lengthInWords, 2);
            totalReadCount += moduleFile.gcount();

            //Flip the bits around since Amiga computers were Big Endian machines.
            sample.lengthInWords = (((sample.lengthInWords & 0x00FFu) << 8u) | ((sample.lengthInWords & 0xFF00u) >> 8u));

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 2)
            {
                //This file is way too small to be a proper mod file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -3;
            }

            //Read sample fine-tune
            moduleFile.read((char *)&sample.finetune, 1);
            totalReadCount += moduleFile.gcount();

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 1)
            {
                //This file is way to small to be a proper wav file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -4;
            }

            //Read sample volume
            moduleFile.read((char *)&sample.volume, 1);
            totalReadCount += moduleFile.gcount();

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 1)
            {
                //This file is way to small to be a proper wav file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -5;
            }

            //Read repeat start offset
            moduleFile.read((char *)&sample.repeatOffset, 2);
            totalReadCount += moduleFile.gcount();

            sample.repeatOffset = (((sample.repeatOffset & 0x00FFu) << 8u) | ((sample.repeatOffset & 0xFF00u) >> 8u));

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 2)
            {
                //This file is way to small to be a proper wav file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -6;
            }

            //Read repeat length
            moduleFile.read((char *)&sample.repeatLength, 2);
            totalReadCount += moduleFile.gcount();

            sample.repeatLength = (((sample.repeatLength & 0x00FFu) << 8u) | ((sample.repeatLength & 0xFF00u) >> 8u));

            //Check to see if we actually read enough bytes
            if (moduleFile.gcount() != 2)
            {
                //This file is way to small to be a proper wav file
                SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" +targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                return -7;
            }
        }

        //DEBUG: Check current read count
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Post Sample Headers: Current Read Count: " + std::to_string(totalReadCount) + " \n");

        //
        //Read number of song positions
        //
        moduleFile.read((char *)&songPositions, 1);
        totalReadCount += moduleFile.gcount();

        //Check to see if we actually read enough bytes
        if (moduleFile.gcount() != 1)
        {
            //This file is way to small to be a proper wav file
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
            return -8;
        }

        //Burn through a read
        moduleFile.read((char*)&readBuffer, 1);
        totalReadCount += moduleFile.gcount();

        //Check to see if we actually read enough bytes
        if (moduleFile.gcount() != 1)
        {
            //This file is way to small to be a proper wav file
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
            return -9;
        }

        //
        //Read the pattern table
        //
        moduleFile.read((char*)&patternTable, 128);
        totalReadCount += moduleFile.gcount();

        //DEBUG: Check current read count
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: " + std::to_string(totalReadCount) + " \n");

        //Check to see if we actually read enough bytes
        if (moduleFile.gcount() != 128)
        {
            //This file is way to small to be a proper wav file
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
            return -10;
        }

        //
        //Read some tag info
        //
        moduleFile.read((char*)&readBuffer, 4);
        totalReadCount += moduleFile.gcount();

        //Check to see if we actually read enough bytes
        if (moduleFile.gcount() != 4)
        {
            //This file is way to small to be a proper wav file
            SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
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
            moduleFile.seekg(-4, std::ios_base::cur);
            totalReadCount -= 4;
            SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "No Module File signature detected.\n");
        }

        //
        //Figure out how many patterns there are to read.
        //

        //Go through the pattern table and find the largest number.
        for (unsigned char i : patternTable)
        {
            if (i > numberOfPatterns)
            {
                numberOfPatterns = i;
                SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current number of patterns: " + std::to_string(numberOfPatterns) + "\n");
            }
        }

        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Module Load: " + std::to_string(numberOfPatterns) + " patterns found.\n");

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
                    moduleFile.read((char *)&readBuffer, 4);
                    totalReadCount += moduleFile.gcount();

                    //Check to see if we actually read enough bytes
                    if (moduleFile.gcount() != 4)
                    {
                        //This file is way to small to be a proper wav file
                        SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.\n");
                        ProperlyLoaded = true;
                        return -12;
                    }

                    //
                    //Parse out the data.
                    //

                    //Copy the first two bytes over into period
                    patterns[i][j][k].period = ((readBuffer[0] & 0x0Fu) << 8u) | (readBuffer[1]);

                    //Copy the second two bytes over into effect
                    patterns[i][j][k].effect = ((readBuffer[2] & 0x0Fu) << 8u) | (readBuffer[3]);

                    //Bit mask unneeded bits, shift, and add them together
                    patterns[i][j][k].sample = (readBuffer[0] & 0xF0u) | ((readBuffer[2] & 0xF0u) >> 4u);
                }
            }
        }

        //DEBUG: Check current read count
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: " + std::to_string(totalReadCount) + " \n");

        //
        //Load up the samples with their data
        //

        for (auto & sample : samples)
        {
            if (sample.lengthInWords > 1)
            {
                //Read the data in there
                moduleFile.read((char*)sample.data, sample.SampleLengthInBytes());
                totalReadCount += moduleFile.gcount();

                //Check to see if we actually read enough bytes
                if (moduleFile.gcount() != sample.SampleLengthInBytes())
                {
                    //This file is way to small to be a proper wav file
                    SGE::System::Message::Output(SGE::System::Message::Levels::Error, SGE::System::Message::Sources::FileFormats, "Sound System Module File \"" + targetFilename + "\" is not correct format - File Too Small to be proper.  Sample Data Reported: " + std::to_string(sample.lengthInWords * 2) + "  Sample Data Read: " + std::to_string(moduleFile.gcount()) + " \n");
                }
            }
        }

        //DEBUG: Check current read count
        SGE::System::Message::Output(SGE::System::Message::Levels::Debug, SGE::System::Message::Sources::FileFormats, "Current Read Count: " + std::to_string(totalReadCount) + " \n");


        //
        //Close the file out
        //
        moduleFile.close();

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
                target[i] = SGE::Sound::sampleType((unsigned char)this->data[i] << 8u);
            }
            return true;
        }
        else
        {
            return false;
        }

    }

    int ModuleFile::MODSample::SampleLengthInBytes() const
    {
        return this->lengthInWords * 2;
    }
}
