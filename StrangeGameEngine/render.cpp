﻿#include "render.h"
namespace SGE
{
	namespace Render
	{
		//Draws a string of characters
		void DrawString(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			char* characters,							//Char pointer to a null terminated character byte string
			const unsigned long long characterROM[],	//64-bit Character ROM array to map characters against
			int characterSpacing,						//The amount space to give each character, standard spacing is 8 to go along with the 8X8 character size
			int targetX,								//Target X location to start drawing from (Upper Left Corner)
			int targetY,								//Target Y location to start drawing from (Upper Left Corner)
			unsigned char rColor,						//8-bit (0-255) Red component of the character color
			unsigned char gColor,						//8-bit (0-255) Green component of the character color
			unsigned char bColor)						//8-bit (0-255) Blue component of the character color
		{
			//Initialize the stringPosition to the starting point for any string.
			int stringPosition = 0;

			//While we haven't hit the null termination for the string.
			while (characters[stringPosition] != 0)
			{
				//Call the Draw the 8x8 character
				Draw8x8Character(targetDisplay, characters[stringPosition], characterROM, targetX + stringPosition*characterSpacing, targetY, rColor, gColor, bColor);

				//Move to the next position in the string
				stringPosition++;
			}
		}

		//Draw a single character
		void Draw8x8Character(
			SGE::VirtualDisplay* targetDisplay,			//Target Display to render onto
			char character,								//Character to draw
			const unsigned long long characterROM[],	//64-bit Character ROM array to map characters against
			int targetX,								//Target X location to start drawing from (Upper Left Corner)
			int targetY,								//Target Y location to start drawing from (Upper Left Corner)
			unsigned char rColor,						//8-bit (0-255) Red component of the character color
			unsigned char gColor,						//8-bit (0-255) Green component of the character color
			unsigned char bColor)						//8-bit (0-255) Blue component of the character color
		{
			//Get a pointer to what we are interested in.
			//Cast down to an unsigned char so to work with 8-bit at a time of the 64-bit
			unsigned char* characterToDraw = (unsigned char*)&characterROM[(unsigned char)character];

			//Get the RAM position to start writing to in Video RAM
			int currentRAM = targetX + (targetY * targetDisplay->virtualVideoX);

			//Pack the colors into pixel format
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Check the whole character to see if there's anything to draw at all...
			//Or if the thing is blank
			if (characterROM[(unsigned char)character])
			{
				//For each 8-bit row in the 8x8 array of the 64-bit character map
				for (int i = 0; i < 8; i++)
				{
					//Check for blank row, if there's something other an 0
					if (characterToDraw[i])
					{
						//For each bit in thr row
						for (int j = 0; j < 8; j++)
						{
							//Check for blank spot
							//AND mask the character row in question to see if there's a 1 in that particular bit spot.
							if (characterToDraw[i] & (0x01 << j))
							{
								targetDisplay->virtualVideoRAM[currentRAM] = targetColor;
							}
							//Hop to the next spot in RAM regardless
							currentRAM++;
						}

						//Hop to the next row in RAM from the end of the current row
						currentRAM += (targetDisplay->virtualVideoX - 8);
					}
					else
					{
						//Hop to the next row in RAM from the start of the current row
						currentRAM += (targetDisplay->virtualVideoX);
					}
				}
			}

		}

		//Draw a pixel
		//Provided as a utility function, not the most efficent for mass writing, but good for spot stuff.
		void DrawPixel(
			SGE::VirtualDisplay* targetDisplay,			//Target Display to render onto
			int targetX,								//Target X location to draw
			int targetY,								//Target Y location to draw
			unsigned char rColor,						//8-bit (0-255) Red component of the pixel color
			unsigned char gColor,						//8-bit (0-255) Green component of the pixel color
			unsigned char bColor)						//8-bit (0-255) Blue component of the pixel color
		{
			//Set the location in video RAM
			int targetRAM = targetX + targetY * targetDisplay->virtualVideoX;

			//As long as we haven't left valid video RAM ranges...
			if (targetRAM >= 0 && targetRAM < targetDisplay->virtualVideoX * targetDisplay->virtualVideoY)
			{
				//Write the pixel
				targetDisplay->virtualVideoRAM[targetRAM] = PackColors(rColor, gColor, bColor);
			}
		}


		//Draw Block Data - copy a block of image data over
		void DrawDataBlock(
			SGE::VirtualDisplay* targetDisplay,			//Target Display to render onto
			int targetX,								//Target X location to start drawing from (Upper Left Corner)
			int targetY,								//Target Y location to start drawing from (Upper Left Corner)
			int sourceWidth,							//The width of the source block
			int sourceHeight,							//The height of the source block
			unsigned int* sourceDataBlock)				//The source data block
		{
			//Set the starting point in VideoRAM
			int targetRAM = (targetX + (targetY * targetDisplay->virtualVideoX));

			//Set the starting pint in the source data block RAM
			int sourceRAM = 0;

			//Set the maximum range for the current video RAM
			int maxRAM = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//For each row of the source block
			for (int i = 0; i < sourceHeight; i++)
			{
				//Copy of row from the source over to the target
				memcpy(&targetDisplay->virtualVideoRAM[targetRAM], &sourceDataBlock[sourceRAM], sourceWidth * 4);

				//Increment to the next row in the display
				targetRAM += targetDisplay->virtualVideoX;

				//Increment to the next row in the source
				sourceRAM += sourceWidth;
			}
		}



		//Draw a free line
		void DrawLine(
			SGE::VirtualDisplay* targetDisplay,			//Target Display to render onto
			int startX,									//Target X location to start drawing from
			int startY,									//Target Y location to start drawing from
			int endX,									//Target X location to draw to
			int endY,									//Target Y location to draw to
			unsigned char rColor,						//8-bit (0-255) Red component of the pixel color
			unsigned char gColor,						//8-bit (0-255) Green component of the pixel color
			unsigned char bColor)						//8-bit (0-255) Blue component of the pixel color
		{
			int deltaL;									//The amount of error compensation per correction
			int currentRAM;								//Current point in Video RAM to start drawing from
			int loopRAMStep;							//Which way we are suppose to move in Video RAM to simulate moving via the X axis
			int loopRAMJump;							//Which way we are suppose to move in Video RAM to simulate moving via the Y axis
			int maxRAMBytes;							//The maximum size of Video RAM
			int errorL;									//The amount of error accumulated for each step of drawing the line
			int error;									//The amount of error accumulated from drawing the line so far

			//Calculate deltas
			int deltaX = endX - startX;					//The amount of change in X coordinate from start to end 
			int deltaY = endY - startY;					//The amount of change in Y coordinate from start to end

			//Pack the color components up into ready to write form
			unsigned int colorValues = PackColors(rColor, gColor, bColor);

			//Check for any straight lines and use the more efficient functions

			//The line is straight up or a column, draw as such
			if (deltaX == 0)
			{
				//No change in X, this is probably a Column that needs to be drawn.
				DrawColumn(targetDisplay, startX, startY, endY - startX, rColor, gColor, bColor);
				return;
			}

			//The line is horizontal or a row, draw as such
			if (deltaY == 0)
			{
				//No change in Y, this is probably a row that needs to be drawn.
				DrawRow(targetDisplay, startX, startY, endX - startX, rColor, gColor, bColor);
				return;
			}

			//Determine orientation
			//Here we determine which way the line is going to adjust parameters for drawing

			//X greater octet
			if (std::abs(deltaX) > std::abs(deltaY))
			{
				//Determine X direction
				if (deltaX > 0)
					loopRAMStep = 1;
				else
					loopRAMStep = -1;

				//Determine Y direction
				if (deltaY > 0)
					loopRAMJump = targetDisplay->virtualVideoX;
				else
					loopRAMJump = -targetDisplay->virtualVideoY;

				//Set the DeltaL
				deltaL = std::abs(deltaX);

				//Set the Error
				errorL = std::abs(deltaY);
			}
			//Y greater octet
			else
			{
				//Determine X direction
				if (deltaX > 0)
					loopRAMJump = 1;
				else
					loopRAMJump = -1;

				//Determine Y direction
				if (deltaY > 0)
					loopRAMStep = targetDisplay->virtualVideoX;
				else
					loopRAMStep = -targetDisplay->virtualVideoX;

				//Set the DeltaL
				deltaL = std::abs(deltaY);

				//Set the Error
				errorL = std::abs(deltaX);
			}

			//Do some common and mostly static calculations
			currentRAM = (startX + (startY * targetDisplay->virtualVideoX));
			maxRAMBytes = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;


			//Initialize error
			error = -deltaL;

			for (int i = 0;
				i <= deltaL &&					//For the number of points to drop
				currentRAM < maxRAMBytes &&		//Don't exceed maximum RAM
				currentRAM >= 0;				//Don't exceed RAM the other way
				i++)
			{
				//Plot point
				targetDisplay->virtualVideoRAM[currentRAM] = colorValues;

				//Increment to the next RAM location
				currentRAM += loopRAMStep;

				//Calculate the error
				error += errorL;

				//Check for error jump
				if (error >= 0)
				{
					//Adjust drawing position to compensate for error
					currentRAM += loopRAMJump;

					//Adjust error amount to show latest compensation
					error -= deltaL;
				}
			}
		}


		//Draw a line row
		void DrawRow(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			int startX,								//Starting X point (Upper Left Corner)
			int startY,								//Starting Y point (Upper Left Corner)
			int width,								//Desired width of row line
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Do the calculations once for common bits

			//Figure out the starting point in video RAM
			int startRAMByte = (startX + startY*targetDisplay->virtualVideoX);

			//Linear translation of VRAM location for the maximum width
			int rowBytes = (startY + 1) * targetDisplay->virtualVideoX;

			//Figure out the maximum VRAM size
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//Pack those colors up
			unsigned int colorValues = PackColors(rColor, gColor, bColor);


			//Start marking some dots
			for (int i = startRAMByte;
				i < (startRAMByte + width) &&			//Don't draw pass desired width
				i < rowBytes &&							//Don't draw past the current row
				i < (displayRAMSize);					//Don't go outside the available video RAM
				i++)
			{
				//Set color of pixel
				targetDisplay->virtualVideoRAM[i] = colorValues;
			}
		}

		//Draw a line column
		void DrawColumn(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			int startX,								//Starting X point to draw from (Upper Left Corner)
			int startY,								//Starting Y point to draw from (Upper Left Corner)
			int height,								//Height desired for column
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Do some fairly static calculations once

			//Starting point in Video RAM
			int startRAM = (startX + startY*targetDisplay->virtualVideoX);

			//Linear translation in Video RAM of the maximum height
			int columnMaxBytes = startRAM + targetDisplay->virtualVideoX * height;

			//Calculate the total display RAM size
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//Pack the color components up
			unsigned int colorValues = PackColors(rColor, gColor, bColor);

			//Start marking some pixels
			for (int i = startRAM;
				i < columnMaxBytes &&							//Don't go past desired row for column
				i < (displayRAMSize);							//Don't go outside the available video RAM
				i = i + targetDisplay->virtualVideoX)
			{
				//Set color of pixel
				targetDisplay->virtualVideoRAM[i] = colorValues;
			}
		}

		//Draw a hollow rectangle of a given color and in a desired location of a desired size
		void DrawRectangle(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			int startX,								//Starting X to draw from (Upper Left Corner)
			int startY,								//Starting Y to draw from (Upper Left Corner)
			int width,								//Width of rectangle to draw
			int height,								//Height of rectangle to draw
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Top Row Line
			DrawRow(targetDisplay, startX, startY, width, rColor, gColor, bColor);

			//Bottom Row Line
			DrawRow(targetDisplay, startX, startY + height - 1, width, rColor, gColor, bColor);

			//Left Column Line
			DrawColumn(targetDisplay, startX, startY + 1, height - 1, rColor, gColor, bColor);

			//Right Column Line
			DrawColumn(targetDisplay, startX + width - 1, startY + 1, height - 1, rColor, gColor, bColor);
		}


		//Draws a filled rectange or box of a given color and in a desired location of a desired size
		void DrawBox(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			int startX,								//Starting X to draw from (Upper Left Corner)
			int startY,								//Starting Y to draw from (Upper Left Corner)
			int width,								//Width of box to draw
			int height,								//Height of box to draw
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Get the starting point in video RAM based on desired location and size of the display
			int targetRAM = (startX + (startY * targetDisplay->virtualVideoX));

			//Starting point
			int startingRAM = targetRAM;

			//Pack the colors up
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Draw the first row
			for (int i = 0; i < width; i++)
			{
				//Copy the color into video ram
				memcpy(&targetDisplay->virtualVideoRAM[targetRAM+i], &targetColor, 4);
			}
			
			//Then memcpy the rest of the rows from this one
			//First move to the next row
			targetRAM += targetDisplay->virtualVideoX;

			//Loop through the remaining rows
			for (int i = 1; i < height; i++)
			{
				//Copy the first row to the rest of the rows
				memcpy(&targetDisplay->virtualVideoRAM[targetRAM], &targetDisplay->virtualVideoRAM[startingRAM], width * 4);

				//Next Row
				targetRAM += targetDisplay->virtualVideoX;
			}
		}

		//Blank the video RAM with straight zeros.  Hence Zero Alpba Black.
		void ZBlank(SGE::VirtualDisplay* targetDisplay)
		{
			//Get the display RAM size in byte chunks
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY * sizeof(unsigned int);

			//Memset for the win!!
			memset(targetDisplay->virtualVideoRAM, 0, displayRAMSize);
		}

		//Blanks the video RAM completely with a choosen color
		void Blank(
			SGE::VirtualDisplay* targetDisplay,		//Target Display to render onto
			unsigned char rColor,					//8-bit (0-255) Red component of the color
			unsigned char gColor,					//8-bit (0-255) Green component of the color
			unsigned char bColor)					//8-bit (0-255) Blue component of the color
		{
			//Get the display RAM size in 32-bit chunks
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//Pack up the color components into a 32-bit chunk
			unsigned int pixelValues = PackColors(rColor, gColor, bColor);

			//Fill the first "row"
			for (int i = 0; i < targetDisplay->virtualVideoX; i++)
			{
				memcpy(targetDisplay->virtualVideoRAM + i, &pixelValues, 4);
			}

			//File subsequent rows with the first
			for (int i = targetDisplay->virtualVideoX; i < displayRAMSize; i = i + targetDisplay->virtualVideoX)
			{
				memcpy(&targetDisplay->virtualVideoRAM[i], targetDisplay->virtualVideoRAM, targetDisplay->virtualVideoX * 4);
			}
		}

		//Packs the Red, Green, and Blue 8-bit components with a dummy Alpha value into a 32-bit unsigned int
		unsigned int PackColors(
			unsigned char redValue,			//8-bit (0-255) Red component of the color
			unsigned char greenValue,		//8-bit (0-255) Green component of the color
			unsigned char blueValue)		//8-bit (0-255) Blue component of the color
		{
			//Bit shift this shit and return it.
			//Will work so long as Alpha blending is disabled in OpenGL
			return ((uint32_t)redValue) | ((uint32_t)greenValue << 8) | ((uint32_t)blueValue << 16);
		}

		RenderBitmapFile::RenderBitmapFile()
		{
			height = 0;
			width = 0;
			imageData = nullptr;
		}

		RenderBitmapFile::~RenderBitmapFile()
		{
			//If something got allocated to the imageData pointer, nuke it!
			if (imageData != nullptr)
			{
				delete imageData;
			}
		}

		int RenderBitmapFile::LoadFile(char* targetFilename)
		{
			FILE* bitmapFile;
			unsigned int readCount = 0;
			unsigned char pixelRed;
			unsigned char pixelGreen;
			unsigned char pixelBlue;
			unsigned int* rowData;
			

			//Attempt to open the bitmap file
			//Open in binary read mode.
			bitmapFile = fopen(targetFilename, "rb");

			//Check to see if we got a valid file pointer
			if (bitmapFile == NULL)
			{
				fprintf(stderr, "Render Bitmap File Error:  Cannot open file \"%s\"\n", targetFilename);
				return -1;
			}

			//If we got a valid file pointer, lets attempt to read the basic bitmap header
			//Due to alignment issues, manually read the individual bits
			
			readCount = fread(&bitmapHeader.idField, 1, 2, bitmapFile);
			readCount = fread(&bitmapHeader.bmpSize, 4, 1, bitmapFile);
			readCount = fread(&bitmapHeader.reserved1, 2, 1, bitmapFile);
			readCount = fread(&bitmapHeader.reserved2, 2, 1, bitmapFile);
			readCount = fread(&bitmapHeader.offset, 4, 1, bitmapFile);


			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount == 0)
			{
				fprintf(stderr, "Render Bitmap File Error:  File \"%s\" is not correct format - File too small.\n", targetFilename);
				return -2;
			}

			//Let's check to see if we got some valid data
			//Check the ID field
			if (memcmp("BM", &bitmapHeader.idField, 2) != 0)
			{
				fprintf(stderr, "Render Bitmap File Error:  File \"%s\" is not correct format - Bitmap File Header ID Field incorrect.\n", targetFilename);
				return -3;
			}
			
			//Read the Bitmap info header
			readCount = fread(&bitmapInfo, 1, sizeof(bitmapInfo), bitmapFile);

			//Check to see if we got a whole header, or if the end of file hit early
			if (readCount != sizeof(bitmapInfo))
			{
				fprintf(stderr, "Render Bitmap File Error:  File \"%s\" is not correct format - File too small.\n", targetFilename);
				return -2;
			}

			printf("DEBUG: Bitmap Load - \"%s\" - Bitmap Data Offset: %d\n", targetFilename, bitmapHeader.offset);
			printf("DEBUG: Bitmap Load - \"%s\" - Header Size: %d\n", targetFilename, bitmapInfo.sizeOfHeader);
			printf("DEBUG: Bitmap Load - \"%s\" - Height: %d\n", targetFilename, bitmapInfo.bitmapHeight);
			printf("DEBUG: Bitmap Load - \"%s\" - Width: %d\n", targetFilename, bitmapInfo.bitmapWidth);
			printf("DEBUG: Bitmap Load - \"%s\" - Size: %d\n", targetFilename, bitmapInfo.imageSize);


			//Go to the file offset where the image data starts
			fseek(bitmapFile, bitmapHeader.offset, SEEK_SET);

			//Create a spot to put the image data
			imageData = new unsigned int[bitmapInfo.bitmapHeight * bitmapInfo.bitmapWidth];

			//Loop in a way to reverse the row order... because bitmaps are stored mirror from normal coordinate systems.
			//For... reasons...

			//Start at the start of the bottom row and move backwards
			for (int j = (bitmapInfo.bitmapHeight - 1) * bitmapInfo.bitmapWidth; j >= 0; j = j - bitmapInfo.bitmapWidth)
			{
				//Go through the row data and process it to the corrected format
				for (int i = 0; i < bitmapInfo.bitmapWidth; i++)
				{
					//Pull the pixel bytes
					//Currently works for 24 bits per pixel RGB BMPs
					fread(&pixelBlue, 1, 1, bitmapFile);
					fread(&pixelGreen, 1, 1, bitmapFile);
					fread(&pixelRed, 1, 1, bitmapFile);

					//Rearrange the pixel color data to something we can use
					imageData[j+i] = ((uint32_t)pixelRed) | ((uint32_t)pixelGreen << 8) | ((uint32_t)pixelBlue << 16);
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