#include "render.h"
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
								memcpy(&targetDisplay->virtualVideoRAM[currentRAM], &targetColor, 4);
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
				DrawColumn(targetDisplay, startX, startY, endY - startY, rColor, gColor, bColor);
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
					loopRAMJump = -targetDisplay->virtualVideoX;

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
				memcpy(&targetDisplay->virtualVideoRAM[currentRAM], &colorValues, 4);

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
			int currentRAM = (startX + startY*targetDisplay->virtualVideoX);

			//Figure out the maximum VRAM size
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//Direction and amount to step in ram
			int ramStep = 0;

			//Pack those colors up
			unsigned int colorValues = PackColors(rColor, gColor, bColor);


			//Check to see which direction we are going
			//If the width is negative and going to the left
			if (width < 0)
			{
				//Set ram stepping
				ramStep = -1;

				//Flip the width sign to make the for loop happy
				width *= -1;
			}
			//If the width is positive and going to the right
			else if (width > 0)
			{
				//Set the ram stepping
				ramStep = 1;
			}
			//If the width is zero...
			else
			{
				//There's nothing to do, since there's no width.
				return;
			}


			//Assume we are given valid direction and width
			for (int i = 0; i < width; i++)
			{
				//But check to see if we go well outside normal bounds
				if (currentRAM < 0 || currentRAM >= displayRAMSize)
				{
					//Get of the loop we are done and at the limits
					break;
				}

				//Dump the values to memory
				memcpy(&targetDisplay->virtualVideoRAM[currentRAM], &colorValues, 4);

				//Go to the next ram location
				currentRAM += ramStep;
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
			int currentRAM = (startX + startY*targetDisplay->virtualVideoX);

			//Calculate the total display RAM size
			int displayRAMSize = targetDisplay->virtualVideoX * targetDisplay->virtualVideoY;

			//Direct to which to step around in the RAM
			int ramStep = 0;


			//Pack the color components up
			unsigned int colorValues = PackColors(rColor, gColor, bColor);


			//Check to see which direction we are going
			//If we are going up
			if (height < 0)
			{
				//Set the ramstep
				ramStep = -targetDisplay->virtualVideoX;

				//Flip the sign on the height, so the for loop is happy.
				height *= -1;
			}
			//If we are going down
			else if (height > 0)
			{
				ramStep = targetDisplay->virtualVideoX;
			}
			//If we aren't... going anywhere...
			else
			{
				//Height 0 doesn't make sense, there's nothing to do
				//... so return.
				return;
			}


			//Assuming we are drawing down a legitmate path
			for (int i = 0; i < height; i++)
			{
				//Check to make sure we haven't gone off the rails
				//and are about to violate memory regions.
				if (currentRAM < 0 || currentRAM >= displayRAMSize)
				{
					//Break out of the loop, we have reached the end of valid territory.
					break;
				}

				//Dump the values to the memory location
				memcpy(&targetDisplay->virtualVideoRAM[currentRAM], &colorValues, 4);

				//Go to the next point
				currentRAM += ramStep;
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



		void DrawVectorShape(SGE::VirtualDisplay* targetDisplay, int startX, int startY, float scalingFactor, int numberOfVertexes, VertexPoint vertexes[], unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Go through the vertex point list and draw lines
			for (int i = 0; i < numberOfVertexes; i++)
			{
				//Draw a line between two points on the vertex, wrapping the last and first at the very end.
				DrawLine(targetDisplay, 
					startX + int((vertexes[i].x) * scalingFactor), 
					startY + int((vertexes[i].y) * scalingFactor), 
					startX + int((vertexes[(i + 1) % numberOfVertexes].x) * scalingFactor), 
					startY + int((vertexes[(i + 1) % numberOfVertexes].y) * scalingFactor), 
					rColor, 
					gColor, 
					bColor);
			}
		}


		void DrawFilledTriangle(SGE::VirtualDisplay* targetDisplay, int startX, int startY, float scalingFactor, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Pack the colors up into something useful
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Apply offsets and scaling
			vertex1.x = vertex1.x * scalingFactor + startX;
			vertex1.y = vertex1.y * scalingFactor + startY;
			vertex2.x = vertex2.x * scalingFactor + startX;
			vertex2.y = vertex2.y * scalingFactor + startY;
			vertex3.x = vertex3.x * scalingFactor + startX;
			vertex3.y = vertex3.y * scalingFactor + startY;

			//Set the initial maxes and mins to the first vertex
			//For comparison purposes.
			int xMax = vertex1.x;
			int xMin = vertex1.x;
			int yMax = vertex1.y;
			int yMin = vertex1.y;

			//Determine bounding box of triangle

			//Compare vertex 2 against vertex 1's values
			//X Max comparison
			if (vertex2.x > xMax)
			{
				xMax = vertex2.x;
			}

			//X Min comparison
			if (vertex2.x < xMin)
			{
				xMin = vertex2.x;
			}

			//Y Max comparison
			if (vertex2.y > yMax)
			{
				yMax = vertex2.y;
			}

			//Y Min comparison
			if (vertex2.y < yMin)
			{
				yMin = vertex2.y;
			}

			//Compare vertex 3 against the current Max/Min values
			//X Max Comparison
			if (vertex3.x > xMax)
			{
				xMax = vertex3.x;
			}

			//X Min comparison
			if (vertex3.x < xMin)
			{
				xMin = vertex3.x;
			}

			//Y Max comparison
			if (vertex3.y > yMax)
			{
				yMax = vertex3.y;
			}

			//Y Min comparison
			if (vertex3.y < yMin)
			{
				yMin = vertex3.y;
			}

			//Use half-space functions to determine if the pixel is in the space of the triangle or not

			//Same pre-calculations of unchanging values
			VertexPoint spanningVector12 = {vertex1.x - vertex2.x, vertex1.y - vertex2.y};
			VertexPoint spanningVector23 = {vertex2.x - vertex3.x, vertex2.y - vertex3.y};
			VertexPoint spanningVector31 = {vertex3.x - vertex1.x, vertex3.y - vertex1.y};

			//Partial crossproducts geared towards the Y element of the looping
			int partialProductY12 = spanningVector12.x * (yMin - vertex1.y);
			int partialProductY23 = spanningVector23.x * (yMin - vertex2.y);
			int partialProductY31 = spanningVector31.x * (yMin - vertex3.y);

			//Partial crossproducts geared towards the X element of the looping
			int partialProductX12 = spanningVector12.y * (xMin - vertex1.x);
			int partialProductX23 = spanningVector23.y * (xMin - vertex2.x);
			int partialProductX31 = spanningVector31.y * (xMin - vertex3.x);

			int resetPartialProductX12 = partialProductX12;
			int resetPartialProductX23 = partialProductX23;
			int resetPartialProductX31 = partialProductX31;

			

			//Go through the bounded, looking for which pixels are within the triangle
			for (int y = yMin; y < yMax; y++)
			{
				for (int x = xMin; x < xMax; x++)
				{
					if (partialProductY12 - partialProductX12 > 0 &&
						partialProductY23 - partialProductX23 > 0 &&
						partialProductY31 - partialProductX31 > 0)
					{
						//Copy the color over.
						memcpy(&targetDisplay->virtualVideoRAM[x + (y*targetDisplay->virtualVideoX)], &targetColor, 4);
					}
					//Increment the partial product
					partialProductX12 += spanningVector12.y;
					partialProductX23 += spanningVector23.y;
					partialProductX31 += spanningVector31.y;

				}

				//Increment the partial product
				partialProductY12 += spanningVector12.x;
				partialProductY23 += spanningVector23.x;
				partialProductY31 += spanningVector31.x;

				//Reset the X partial products
				partialProductX12 = resetPartialProductX12;
				partialProductX23 = resetPartialProductX23;
				partialProductX31 = resetPartialProductX31;
			}

		}







		ImageData::ImageData()
		{

		}

		ImageData::~ImageData()
		{
			if (imageData != nullptr)
			{
				delete imageData;
			}
		}

		//Default constructor RenderBitmapFile
		RenderBitmapFile::RenderBitmapFile()
		{
			//Reserved for setup functions
		}

		//Deconstructor for RenderBitmapFile
		RenderBitmapFile::~RenderBitmapFile()
		{
			//Reserved for clean up functions
		}

		int RenderBitmapFile::LoadFile(char* targetFilename)
		{
			FILE* bitmapFile;
			unsigned int readCount = 0;
			unsigned char pixelRed;
			unsigned char pixelGreen;
			unsigned char pixelBlue;

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
			image.imageData = new unsigned int[bitmapInfo.bitmapHeight * bitmapInfo.bitmapWidth];

			//Set the other aspects of the image
			image.height = bitmapInfo.bitmapHeight;
			image.width = bitmapInfo.bitmapWidth;
			image.imageDataSize = image.height * image.width;

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
					image.imageData[j+i] = ((uint32_t)pixelRed) | ((uint32_t)pixelGreen << 8) | ((uint32_t)pixelBlue << 16);
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