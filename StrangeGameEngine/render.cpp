#include "include\SGE\render.h"
#include <cstring>
namespace SGE
{
	namespace Render
	{
		//Draws a string of characters
		void DrawString(
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
				Draw8x8Character(characters[stringPosition], characterROM, targetX + stringPosition*characterSpacing, targetY, rColor, gColor, bColor);

				//Move to the next position in the string
				stringPosition++;
			}
		}

		//Draw a single character
		void Draw8x8Character(
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
			int currentRAM = targetX + (targetY * SGE::Display::Video::ResolutionX);

			//Pack the colors into pixel format
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Check the whole character to see if there's anything to draw at all...
			//Or if the thing is blank
			if (characterROM[(unsigned char)character])
			{
				//
				//  Yes, we could use a for loop, but it has been unrolled for performance sake.
				//  It seems that unrolled, static offsets can be used instead of have to contend with the i variable.
				//  Hacky, but it does seem to perform better.
				//

				//First Row
				if (characterToDraw[0])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[0] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[0] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[0] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[0] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[0] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[0] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[0] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[0] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Second Row
				if (characterToDraw[1])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[1] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[1] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[1] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[1] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[1] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[1] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[1] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[1] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Third row
				if (characterToDraw[2])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[2] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[2] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[2] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[2] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[2] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[2] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[2] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[2] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Fourth Row
				if (characterToDraw[3])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[3] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[3] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[3] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[3] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[3] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[3] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[3] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[3] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Fifth Row
				if (characterToDraw[4])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[4] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[4] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[4] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[4] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[4] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[4] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[4] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[4] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Sixth Row
				if (characterToDraw[5])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[5] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[5] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[5] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[5] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[5] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[5] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[5] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[5] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Seventh Row
				if (characterToDraw[6])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[6] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[6] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[6] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[6] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[6] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[6] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[6] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[6] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::Video::ResolutionX;

				//Eigth row
				if (characterToDraw[7])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[7] & 0x01 && (SGE::Display::Video::RAM[currentRAM + 0] = targetColor);
					characterToDraw[7] & 0x02 && (SGE::Display::Video::RAM[currentRAM + 1] = targetColor);
					characterToDraw[7] & 0x04 && (SGE::Display::Video::RAM[currentRAM + 2] = targetColor);
					characterToDraw[7] & 0x08 && (SGE::Display::Video::RAM[currentRAM + 3] = targetColor);
					characterToDraw[7] & 0x10 && (SGE::Display::Video::RAM[currentRAM + 4] = targetColor);
					characterToDraw[7] & 0x20 && (SGE::Display::Video::RAM[currentRAM + 5] = targetColor);
					characterToDraw[7] & 0x40 && (SGE::Display::Video::RAM[currentRAM + 6] = targetColor);
					characterToDraw[7] & 0x80 && (SGE::Display::Video::RAM[currentRAM + 7] = targetColor);
				}
			}
		}

		//Draw Block Data - copy a block of image data over
		void DrawDataBlock(
			int targetX,								//Target X location to start drawing from (Upper Left Corner)
			int targetY,								//Target Y location to start drawing from (Upper Left Corner)
			int sourceWidth,							//The width of the source block
			int sourceHeight,							//The height of the source block
			unsigned int* sourceDataBlock)				//The source data block
		{
			//Set the starting point in VideoRAM
			int targetRAM = (targetX + (targetY * SGE::Display::Video::ResolutionX));

			//Set the starting pint in the source data block RAM
			int sourceRAM = 0;

			//For each row of the source block
			for (int i = 0; i < sourceHeight; i++)
			{
				//Copy of row from the source over to the target
				std::memmove(&SGE::Display::Video::RAM[targetRAM], &sourceDataBlock[sourceRAM], sourceWidth * sizeof(unsigned int));

				//Increment to the next row in the display
				targetRAM += SGE::Display::Video::ResolutionX;

				//Increment to the next row in the source
				sourceRAM += sourceWidth;
			}
		}

		void DrawLine(
			int startX,
			int startY,
			int endX,
			int endY,
			unsigned char red,
			unsigned char green,
			unsigned char blue)
		{
			unsigned int pixelColor = PackColors(red, green, blue);		//Pixel Color data, packed together.
			int ramPosition = 0;										//Position in Video RAM
			float deltaXY = 0.0f;

			//Check for a valid line and prune if it is
			//If the return of Prune Line is false, then we can't draw this damn thing anyway
			if (!PruneLine(startX, startY, endX, endY))
			{
				return;
			}

			//Calculate the deltas
			int deltaX = endX - startX;		//X Delta
			int deltaY = endY - startY;		//Y Delta


			//Which is the bigger delta
			//Delta X is bigger and we will draw based on the X-axis
			//We square the numbers to avoid bothering with absolute value calculations
			if (deltaX * deltaX > deltaY * deltaY)
			{
				if (deltaX < 0)
				{
					//Swap the points if there's a negative X delta, since we like draw positive deltas for simplicity
					int temp = 0;

					//Swap the X
					temp = startX;
					startX = endX;
					endX = temp;

					//Swap the Y
					temp = startY;
					startY = endY;
					endY = temp;

					//Flip the deltas
					deltaY = -deltaY;
					deltaX = -deltaX;
				}

				//Calculate the delta for Y change for X
				deltaXY = float(deltaY) / float(deltaX);

				//Start the starting RAM position
				ramPosition = startX + (SGE::Display::Video::ResolutionX * startY);

				//Draw the line
				for (int i = 0; (i <= deltaX); i++)
				{
					SGE::Display::Video::RAM[ramPosition + i + int(i * deltaXY) * SGE::Display::Video::ResolutionX] = pixelColor;
				}
			}

			//Delta Y is bigger and we will draw based on the Y-axis
			else
			{
				if (deltaY < 0)
				{
					//Swap the points if there's a negative X delta
					int temp = 0;

					//Swap the X
					temp = startX;
					startX = endX;
					endX = temp;

					//Swap the Y
					temp = startY;
					startY = endY;
					endY = temp;

					//Flip the deltas
					deltaY = -deltaY;
					deltaX = -deltaX;
				}

				//Calculate the delta for Y change for X
				deltaXY = float(deltaX) / float(deltaY);

				//Start the starting RAM position
				ramPosition = startX + (SGE::Display::Video::ResolutionX * startY);

				//Draw the line
				for (int i = 0; (i <= deltaY); i++)
				{
					SGE::Display::Video::RAM[ramPosition + (i * SGE::Display::Video::ResolutionX) + int(i * deltaXY)] = pixelColor;
				}
			}
		}


		//Prune two starting points back down to the visible area of the screen
		//Return a true if line can even be drawn on screen
		//Return a false if line is completely off screen
		bool PruneLine(int& startX, int& startY, int& endX, int& endY)
		{
			//Figure out the deltas
			int deltaX = endX - startX;									//X Delta
			int deltaY = endY - startY;									//Y Delta
			

			//Check for lines that cannot possibly exist on the +X, +Y quadrant
			if ((startX < 0 && endX < 0) ||																	//Both X's are negative
				(startY < 0 && endY < 0) ||																	//Both Y's are negative
				(startX >= SGE::Display::Video::ResolutionX && endX >= SGE::Display::Video::ResolutionX) ||	//Both X's are outside the resolution
				(startY >= SGE::Display::Video::ResolutionY && endY >= SGE::Display::Video::ResolutionY))	//Both Y's are outside the resolution
			{
				//This line doesn't exist anywhere we could possibly draw it.
				return false;
			}

			//Check for straight lines outside what we can do anything about
			if ((startX < 0 && deltaX == 0) ||										//X is negative and it's going to stay that way
				(startY < 0 && deltaY == 0) ||										//Y is negative and it's going to stay that way
				(startX >= SGE::Display::Video::ResolutionX && deltaX == 0) ||		//X is outside the resolution and it's going to stay that way
				(startY >= SGE::Display::Video::ResolutionY && deltaY == 0))		//Y is outside the resoltuion and it's going to stay that way
			{
				//Can't draw this line
				return false;
			}

			//Check to see if we need to prune the start point back to the right quardrant
			//Bring in the X
			if (startX < 0)
			{
				//Calculate new Y along X axis
				startX = 0;
				startY += (-startX * deltaY) / deltaX;
			}
			else if (startX >= SGE::Display::Video::ResolutionX)
			{
				startX = SGE::Display::Video::ResolutionX - 1;
				startY -= (-startX * deltaY) / deltaX;
			}

			//Bring in the Y
			if (startY < 0)
			{
				//Calculate new X along Y axis
				startX += (-startY * deltaX) / deltaY;
				startY = 0;
			}
			else if (startY >= SGE::Display::Video::ResolutionY)
			{
				startX -= (-startY * deltaX) / deltaY;
				startY = SGE::Display::Video::ResolutionY - 1;
			}

			//Check to see if we need to prune the end point back to the right quardrant
			//Bring in the X
			if (endX < 0)
			{
				//Calculate new Y along X axis
				endX = 0;
				endY += (-endX * deltaY) / deltaX;
			}
			else if (endX >= SGE::Display::Video::ResolutionX)
			{
				endX = SGE::Display::Video::ResolutionX - 1;
				endY -= (-endX * deltaY) / deltaX;
			}

			//Bring in the Y
			if (endY < 0)
			{
				//Calculate new X along Y axis
				endX += (-endY * deltaX) / deltaY;
				endY = 0;
			}
			else if (endY >= SGE::Display::Video::ResolutionY)
			{
				endX -= (-endY * deltaX) / deltaY;
				endY = SGE::Display::Video::ResolutionY;
			}


			//In theory we've trimmed the line down to valid points within the viewable area of the screen
			return true;
		}

		//Draw a hollow rectangle of a given color and in a desired location of a desired size
		void DrawRectangle(
			int startX,								//Starting X to draw from (Upper Left Corner)
			int startY,								//Starting Y to draw from (Upper Left Corner)
			int width,								//Width of rectangle to draw
			int height,								//Height of rectangle to draw
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Top Row Line
			DrawLine(startX, startY, startX + width - 1, startY, rColor, gColor, bColor);

			//Bottom Row Line
			DrawLine(startX, startY + height - 1, startX + width - 1, startY + height - 1, rColor, gColor, bColor);

			//Left Column Line
			DrawLine(startX, startY, startX, startY + height - 1, rColor, gColor, bColor);

			//Right Column Line
			DrawLine(startX + width - 1, startY, startX + width - 1, startY + height - 1, rColor, gColor, bColor);
		}


		//Draws a filled rectange or box of a given color and in a desired location of a desired size
		void DrawBox(
			int startX,								//Starting X to draw from (Upper Left Corner)
			int startY,								//Starting Y to draw from (Upper Left Corner)
			int width,								//Width of box to draw
			int height,								//Height of box to draw
			unsigned char rColor,					//8-bit (0-255) Red color component
			unsigned char gColor,					//8-bit (0-255) Green color component
			unsigned char bColor)					//8-bit (0-255) Blue color component
		{
			//Check for boxes we cannot draw...
			if ((startX + width < 0) ||						//If the X never graces the +x, +y quadrant
				(startY + height < 0) ||					//If the Y never graces the +x, +y quadrant
				(startX >= SGE::Display::Video::ResolutionX) ||	//If the X start is outside the display resolution
				(startY >= SGE::Display::Video::ResolutionY))		//If the Y start is outside the display resolution
			{
				return;
			}


			//Prune the start point and/or the  dimensions to keep it on the screen
			//Prune the X
			if (startX < 0)
			{
				width += startX;
				startX = 0;
			}

			if (startY < 0)
			{
				height += startY;
				startY = 0;
			}

			if (startX + width >= SGE::Display::Video::ResolutionX)
			{
				width = SGE::Display::Video::ResolutionX - startX;
			}

			if (startY + height >= SGE::Display::Video::ResolutionY)
			{
				height = SGE::Display::Video::ResolutionY - startY;
			}

			//Get the starting point in video RAM based on desired location and size of the display
			int targetRAM = (startX + (startY * SGE::Display::Video::ResolutionX));

			//Starting point
			int startingRAM = targetRAM;

			//Pack the colors up
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Draw the first row
			for (int i = 0; i < width; i++)
			{
				//Copy the color into video ram
				SGE::Display::Video::RAM[targetRAM + i] = targetColor;
			}

			//Then memcpy the rest of the rows from this one
			//First move to the next row
			targetRAM += SGE::Display::Video::ResolutionX;

			//Loop through the remaining rows
			for (int i = 1; i < height; i++)
			{
				//Copy the first row to the rest of the rows
				std::memmove(&SGE::Display::Video::RAM[targetRAM], &SGE::Display::Video::RAM[startingRAM], width * sizeof(unsigned int));
				
				//Next Row
				targetRAM += SGE::Display::Video::ResolutionX;
			}
		}

		//Blank the video RAM with straight zeros.  Hence Zero Alpba Black.
		void ZBlank()
		{
			//Memset for the win!!
			memset(SGE::Display::Video::RAM, 0, SGE::Display::Video::RAMSize * sizeof(unsigned int));
		}

		//Packs the Red, Green, and Blue 8-bit components with a dummy Alpha value into a 32-bit unsigned int
		unsigned int PackColors(
			unsigned char redValue,			//8-bit (0-255) Red component of the color
			unsigned char greenValue,		//8-bit (0-255) Green component of the color
			unsigned char blueValue)		//8-bit (0-255) Blue component of the color
		{
			//Bit shift this shit and return it.
			//Will work so long as Alpha blending is disabled in OpenGL
			return blueValue << 16 | greenValue << 8 | redValue;
		}



		void DrawVectorShape(int startX, int startY, float scalingFactor, int numberOfVertexes, VertexPoint vertexes[], unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Go through the vertex point list and draw lines
			for (int i = 0; i < numberOfVertexes; i++)
			{
				//Draw a line between two points on the vertex, wrapping the last and first at the very end.
				DrawLine(
					startX + int((vertexes[i].x) * scalingFactor),
					startY + int((vertexes[i].y) * scalingFactor),
					startX + int((vertexes[(i + 1) % numberOfVertexes].x) * scalingFactor),
					startY + int((vertexes[(i + 1) % numberOfVertexes].y) * scalingFactor),
					rColor,
					gColor,
					bColor);
			}
		}

		void DrawFilledTriangles(int startX, int startY, float scalingFactor, VertexPoint* vertexArray, unsigned int numberOfVertexes, unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Figure out how many full triangle we have
			int numberOfFullTriangles = numberOfVertexes / 3;
			int numberOfVertexesToUse = numberOfFullTriangles * 3;

			for (int i = 0; i < numberOfVertexesToUse; i = i + 3)
			{
				DrawFilledTriangleFast(startX, startY, scalingFactor, vertexArray[i], vertexArray[i + 1], vertexArray[i + 2], rColor, gColor, bColor);
				//DrawFilledTriangleTrue(targetDisplay, startX, startY, scalingFactor, vertexArray[i], vertexArray[i + 1], vertexArray[i + 2], rColor, gColor, bColor);
			}
		}

		//Uses a variant of the Bresenham algorithm to calculate the two X points along the X-axis for each Y
		//Then mass memcpys from a created pixel buffer to fill in the gaps.
		void DrawFilledTriangleFast(int startX, int startY, float scalingFactor, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Pack the colors up
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Pixel Buffer to mass copy memory from
			int targetPixelBufferSize = 0;

			//Scale the vertexes
			vertex1.x = int(vertex1.x * scalingFactor) + startX;
			vertex1.y = int(vertex1.y * scalingFactor) + startY;
			vertex2.x = int(vertex2.x * scalingFactor) + startX;
			vertex2.y = int(vertex2.y * scalingFactor) + startY;
			vertex3.x = int(vertex3.x * scalingFactor) + startX;
			vertex3.y = int(vertex3.y * scalingFactor) + startY;

			//Variables to hold the top most, bottom most, and middle point vertexes
			VertexPoint topVertex;
			VertexPoint bottomVertex;
			VertexPoint middleVertex;

			//
			//  Floats to hold deltas for the three line segments
			//

			float topToBottomLineDelta = 0.0f;
			float topToMiddleLineDelta = 0.0f;
			float middleToBottomLineDelta = 0.0f;

			//
			//  Mapping Offsets
			//
			int bufferMappingOffsetY = 0;
			int bufferMappingOffsetX = 0;



			//
			//  Sanity Checks for drawing
			//

			if ((vertex1.y < 0 && vertex2.y < 0 && vertex3.y < 0) ||		//Do all these vertexes have Y's in the negatives?
				(vertex1.x < 0 && vertex2.x < 0 && vertex3.x < 0) ||		//Do all these vertexes have X's in the negatives?
				(vertex1.y >= SGE::Display::Video::ResolutionY && vertex2.y >= SGE::Display::Video::ResolutionY && vertex3.y >= SGE::Display::Video::ResolutionY) ||	//Do all these vertexes have Y's greater than our Y resolution?
				(vertex1.x >= SGE::Display::Video::ResolutionY && vertex2.x >= SGE::Display::Video::ResolutionY && vertex3.x >= SGE::Display::Video::ResolutionX))	//Do all these vertexes have X's greater than our X resoltuon?
			{
				//We can't draw this!
				//No part of it will ever exist within the screen
				//Eject!
				return;
			}


			//
			//Calculate the ideal orientation of the vertexs for drawing from lowest Y down the screen.
			//

			//Is Vertex 1 is above Vertex 2
			if (vertex1.y < vertex2.y)
			{
				//Vertex 1 IS above Vertex 2
				
				//Is Vertex 1 is above Vertex 3
				if (vertex1.y < vertex3.y)
				{
					//Vertex 1 is above Vertex 3
					//Hence, Vertex 1 is topmost
					topVertex = vertex1;

					//Compare Vertex 2 and Vertex 3 to figure out which is above the other
					middleVertex = vertex2.y < vertex3.y ? vertex2 : vertex3;
					bottomVertex =  vertex2.y < vertex3.y ? vertex3 : vertex2;
				}

				else
				{
					//Vertex 3 is above Vertex 1
					//Hence, Vertex 3 is top vertex
					topVertex = vertex3;

					//By elimination Vetrex 1 is the middle vertex
					middleVertex = vertex1;

					//And Vertex 2 is the bottom vertex
					bottomVertex = vertex2;
				}
			}

			else
			{
				//Vertex 2 is above Vertex 1 (or equal)

				//Is Vertex 2 above Vertex 3
				if (vertex2.y < vertex3.y)
				{
					//Vertex 2 is above Vertex 3

					//Vertex 2 is top vertex
					topVertex = vertex2;

					//Compare Vertex 1 and Vertex 3 to figure out which is above the other
					middleVertex = vertex1.y < vertex3.y ? vertex1 : vertex3;
					bottomVertex =  vertex1.y < vertex3.y ? vertex3 : vertex1;
				}

				else
				{
					//Vertex 3 is above Vertex 2
					//Hence, Vertex 3 is top vertex
					topVertex = vertex3;

					//By elimination Vertex 2 is the middle vertex
					middleVertex = vertex2;

					//And Vertex 2 is the bottom vertex
					bottomVertex = vertex1;
				}
			}

			//
			//  Calculate line deltas
			//

			topToBottomLineDelta = (float) (bottomVertex.x - topVertex.x) / (float) (bottomVertex.y - topVertex.y);
			topToMiddleLineDelta = (float) (middleVertex.x - topVertex.x) / (float) (middleVertex.y - topVertex.y);
			middleToBottomLineDelta = (float) (bottomVertex.x - middleVertex.x) / (float)(bottomVertex.y - middleVertex.y);

			//Check for division by zeros
			(std::isinf(topToBottomLineDelta)) && (topToBottomLineDelta = 0.0f);
			(std::isinf(topToMiddleLineDelta)) && (topToMiddleLineDelta = 0.0f);
			(std::isinf(middleToBottomLineDelta)) && (middleToBottomLineDelta = 0.0f);

			//
			//  Setup pixel buffers
			//

			//Determine greatest distance between triangle points
			int lowestX = vertex1.x;
			int greatestX = vertex1.x;

			//Short circuit logic

			//Lowest checks
			(vertex2.x < lowestX) && (lowestX = vertex2.x);
			(vertex3.x < lowestX) && (lowestX = vertex3.x);

			//Greatest checks
			(vertex2.x > greatestX) && (greatestX = vertex2.x);
			(vertex3.x > greatestX) && (greatestX = vertex3.x);

			targetPixelBufferSize = greatestX - lowestX;


			//Load up the Row Buffer
			for (int i = 0; i < targetPixelBufferSize && i < SGE::Display::Video::ResolutionX; i++)
			{
				SGE::Display::Video::RowBuffer[i] = targetColor;
			}


			//
			//  Save starting Y mapping offset if negative, that way we can copy data properly taking into account culled data
			//

			bufferMappingOffsetY = topVertex.y < 0 ? -topVertex.y : 0;


			//
			// Start drawing the line points pairs
			//

			//Top-To-Middle Line and Top-To-Bottom Line pairing
			for (int currentScreenY = (topVertex.y < 0) ? 0 : topVertex.y;									//Start screenY at topVertex Y, or 0 if topVertex Y is negative
				currentScreenY <= middleVertex.y && currentScreenY < SGE::Display::Video::ResolutionY;		//Keep drawing to the middleVertex Y or until the end of the viewable screen
				currentScreenY++)
			{
				//
				//  Figure out Top-To-Middle line point
				//

				int currentTopToMiddleX = topVertex.x + ((currentScreenY - topVertex.y) * topToMiddleLineDelta);
				
				//Check for any out of bounds issues and correct
				//Short circuit logic
				(currentTopToMiddleX < 0) && (currentTopToMiddleX = 0);																		//Check to see if currentTopToBottomX is negative
				(currentTopToMiddleX >= SGE::Display::Video::ResolutionX) && (currentTopToMiddleX = SGE::Display::Video::ResolutionX - 1);	//Check to see if currentTopToBottomX is beyond the resolution

				//
				//  Figure out Top-To-Bottom line point
				//

				int currentTopToBottomX = topVertex.x + ((currentScreenY - topVertex.y) * topToBottomLineDelta);

				//Check for any out of bounds issues and correct
				//Short circuit logic
				(currentTopToBottomX < 0) && (currentTopToBottomX = 0);																		//Check to see if currentTopToBottomX is negative
				(currentTopToBottomX >= SGE::Display::Video::ResolutionX) && (currentTopToBottomX = SGE::Display::Video::ResolutionX - 1);	//Check to see if currentTopToBottomX is beyond the resolution

				//
				//  Figure out dimensions for memory copy operations
				//

				//What offset are we moving to
				int copyRowDestination = (currentScreenY * SGE::Display::Video::ResolutionX) +					//Starting a the memory location for the current row (Y) we are on
					((currentTopToBottomX < currentTopToMiddleX) ? currentTopToBottomX : currentTopToMiddleX);		//Offset by the smallest X

				//What offset are we moving from
				//int copyRowSource = (int) (currentTopToBottomX < currentTopToMiddleX) ? (currentScreenY * topToBottomLineDelta) : (currentScreenY * topToMiddleLineDelta);		//Offset by the smallest X delta
				int copyRowSource = 0;

				//And how much
				int copyRowLength = currentTopToBottomX - currentTopToMiddleX;
				
				//Short circuit logic for sign flip
				(copyRowLength < 0) && (copyRowLength = -copyRowLength);

				//Add a little extra
				copyRowLength++;

				//
				//  Mass move data
				//

				std::memmove(&SGE::Display::Video::RAM[copyRowDestination],		//In Video RAM
						&SGE::Display::Video::RowBuffer[copyRowSource],			//From the Row Buffer
						sizeof(unsigned int) * copyRowLength);
			}

			//
			//  Update buffer mapping offsets
			//

			//Take the delta between the middleVertex Y and the topVertex Y and any culling distance from a negative Y
			bufferMappingOffsetX = (middleVertex.y - topVertex.y) + middleVertex.y < 0 ? -middleVertex.y : 0;

			//Middle-To-Bottom and Top-To-Bottom Line pairing
			for (int currentScreenY = middleVertex.y < 0 ? 0 : middleVertex.y;									//Start screenY at middleVertex Y, or 0 if middleVertex Y is negative
				currentScreenY <= bottomVertex.y && currentScreenY < SGE::Display::Video::ResolutionY;			//Keep drawing to the bottomVertex Y or until the end of the viewable screen
				currentScreenY++)
			{
				//
				//  Figure out Top-To-Middle line point
				//

				int currentMiddleToBottom = middleVertex.x + ((currentScreenY - middleVertex.y) * middleToBottomLineDelta);

				//Check for any out of bounds issues and correct
				//Short circuit logic
				(currentMiddleToBottom < 0) && (currentMiddleToBottom = 0);																		//Check to see if currentTopToBottomX is negative
				(currentMiddleToBottom >= SGE::Display::Video::ResolutionX) && (currentMiddleToBottom = SGE::Display::Video::ResolutionX - 1);	//Check to see if currentTopToBottomX is beyond the resolution

				//
				//  Figure out Top-To-Bottom line point
				//

				int currentTopToBottomX = topVertex.x + ((currentScreenY - topVertex.y) * topToBottomLineDelta);

				//Check for any out of bounds issues and correct
				//Short circuit logic
				(currentTopToBottomX < 0) && (currentTopToBottomX = 0);																		//Check to see if currentTopToBottomX is negative
				(currentTopToBottomX >= SGE::Display::Video::ResolutionX) && (currentTopToBottomX = SGE::Display::Video::ResolutionX - 1);	//Check to see if currentTopToBottomX is beyond the resolution

				//
				//  Figure out dimensions for memory copy operations
				//

				//What offset are we moving to
				int copyRowDestination = currentScreenY * SGE::Display::Video::ResolutionX +						//Starting a the memory location for the current row (Y) we are on
					((currentTopToBottomX < currentMiddleToBottom) ? currentTopToBottomX : currentMiddleToBottom);		//Offset by the smallest X

																												//What offset are we moving from
				//int copyRowSource = (int)(currentTopToBottomX < currentMiddleToBottom) ? (currentScreenY * topToBottomLineDelta) : (currentScreenY * topToMiddleLineDelta);		//Offset by the smallest X delta
				int copyRowSource = 0;

																																												//And how much
				int copyRowLength = currentTopToBottomX - currentMiddleToBottom;

				//Short circuit logic for sign flip
				(copyRowLength < 0) && (copyRowLength = -copyRowLength);

				copyRowLength++;

				//
				//  Mass move data
				//

				std::memmove(&SGE::Display::Video::RAM[copyRowDestination],	//In Video RAM
					&SGE::Display::Video::RowBuffer[copyRowSource],			//From the Row Buffer
					sizeof(unsigned int) * copyRowLength);
			}
		}


		//Uses the Barycentric Algorithm to calculate what pixels are within a defined triangle
		//Accurate and true.... but expensive as can be computationally
		//Useful for double checking the Fast Triangle draw function.
		void DrawFilledTriangleTrue(int startX, int startY, float scalingFactor, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, unsigned char rColor, unsigned char gColor, unsigned char bColor)
		{
			//Pack the colors up into something useful
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Apply offsets and scaling
			vertex1.x = int(vertex1.x * scalingFactor + startX);
			vertex1.y = int(vertex1.y * scalingFactor + startY);
			vertex2.x = int(vertex2.x * scalingFactor + startX);
			vertex2.y = int(vertex2.y * scalingFactor + startY);
			vertex3.x = int(vertex3.x * scalingFactor + startX);
			vertex3.y = int(vertex3.y * scalingFactor + startY);

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
			VertexPoint spanningVector12 = { vertex1.x - vertex2.x, vertex1.y - vertex2.y };
			VertexPoint spanningVector23 = { vertex2.x - vertex3.x, vertex2.y - vertex3.y };
			VertexPoint spanningVector31 = { vertex3.x - vertex1.x, vertex3.y - vertex1.y };

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
			for (int y = yMin; y <= yMax; y++)
			{
				for (int x = xMin; x <= xMax; x++)
				{
					if (partialProductY12 - partialProductX12 >= 0 &&
						partialProductY23 - partialProductX23 >= 0 &&
						partialProductY31 - partialProductX31 >= 0)
					{
						//Copy the color over.
						SGE::Display::Video::RAM[x + (y*SGE::Display::Video::ResolutionX)] = targetColor;
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

		int RenderBitmapFile::LoadFile(char* targetFilename)
		{
			FILE* bitmapFile;
			size_t readCount = 0;
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
					image.imageData[j + i] = ((uint32_t)pixelRed) | ((uint32_t)pixelGreen << 8) | ((uint32_t)pixelBlue << 16);
				}
			}

			//If the whole process goes well, and has reached this point
			//Close file
			fclose(bitmapFile);

			//Signal all good
			return 0;
		}


		//
		//
		//  Animation Stuff!
		//
		//


		void AnimationBlock::CreateBuffers(unsigned int number, unsigned int XSize, unsigned int YSize)
		{
			//Store the resolutions
			ResolutionX = XSize;
			ResolutionY = YSize;

			//Compute the individual buffer sizes
			ImageBufferSize = ResolutionX * ResolutionY;

			//Store the number of buffers
			NumberOfImages = number;

			//Create the buffers
			//Create the array
			ImageBuffers = new unsigned int*[NumberOfImages];

			//Go through and create buffers in each spot
			for (unsigned int i = 0; i < NumberOfImages; i++)
			{
				ImageBuffers[i] = new unsigned int[ImageBufferSize];
			}
		}

		void AnimationBlock::DeleteBuffers()
		{
			//Nuke the individual buffers
			for (unsigned int i = 0; i < NumberOfImages; i++)
			{
				delete[] ImageBuffers[i];
			}

			//Nuke the array
			delete[] ImageBuffers;

			//Set the nullptr for the base pointer
			ImageBuffers = nullptr;
		}

		bool AnimationBlock::LoadBuffer(unsigned int number, unsigned int* sourceBuffer, unsigned int sourceXOffset, unsigned int sourceYOffset)
		{




			return true;
		}

		void AnimationBlock::StartAnimation()
		{
			//Capture the current time
			previousTime = std::chrono::steady_clock::now();

			//Reset the current image
			CurrentImageBuffer = 0;

			//Flag for animation
			Animating = true;
		}

		void AnimationBlock::ResetAnimation()
		{
			Animating = false;

			CurrentImageBuffer = 0;
		}

		void AnimationBlock::DrawAnimation(int XPosition, int YPosition)
		{
			//Check for animation
			if (Animating)
			{
				//Check to see if we need to progress to the next frame based on timing factors
				if (std::chrono::steady_clock::now() - previousTime > std::chrono::milliseconds(MillisecondsPerFrame))
				{
					//Grab new previous time
					previousTime = std::chrono::steady_clock::now();

					//Go to the next frame buffer
					CurrentImageBuffer = (CurrentImageBuffer + 1) % NumberOfImages;
				}
			}

			//Draw current frame of animation
			SGE::Render::DrawDataBlock(XPosition, YPosition, ResolutionX, ResolutionY, ImageBuffers[CurrentImageBuffer]);
		}

	}
}
