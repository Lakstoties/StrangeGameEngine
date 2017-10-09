#include "include\SGE\render.h"
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
			int currentRAM = targetX + (targetY * SGE::Display::ResolutionX);

			//Pack the colors into pixel format
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Check the whole character to see if there's anything to draw at all...
			//Or if the thing is blank
			if (characterROM[unsigned char (character)])
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
					characterToDraw[0] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[0] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[0] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[0] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[0] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[0] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[0] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[0] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Second Row
				if (characterToDraw[1])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[1] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[1] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[1] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[1] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[1] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[1] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[1] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[1] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Third row
				if (characterToDraw[2])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[2] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[2] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[2] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[2] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[2] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[2] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[2] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[2] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Fourth Row
				if (characterToDraw[3])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[3] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[3] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[3] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[3] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[3] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[3] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[3] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[3] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Fifth Row
				if (characterToDraw[4])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[4] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[4] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[4] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[4] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[4] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[4] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[4] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[4] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Sixth Row
				if (characterToDraw[5])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[5] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[5] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[5] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[5] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[5] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[5] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[5] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[5] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Seventh Row
				if (characterToDraw[6])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[6] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[6] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[6] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[6] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[6] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[6] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[6] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[6] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
				}

				//Hop to the next row in RAM from the start of the current row
				currentRAM += SGE::Display::ResolutionX;

				//Eigth row
				if (characterToDraw[7])
				{
					//Row of pixels
					//This uses short circuit logic
					characterToDraw[7] & 0x01 && (SGE::Display::VideoRAM[currentRAM + 0] = targetColor);
					characterToDraw[7] & 0x02 && (SGE::Display::VideoRAM[currentRAM + 1] = targetColor);
					characterToDraw[7] & 0x04 && (SGE::Display::VideoRAM[currentRAM + 2] = targetColor);
					characterToDraw[7] & 0x08 && (SGE::Display::VideoRAM[currentRAM + 3] = targetColor);
					characterToDraw[7] & 0x10 && (SGE::Display::VideoRAM[currentRAM + 4] = targetColor);
					characterToDraw[7] & 0x20 && (SGE::Display::VideoRAM[currentRAM + 5] = targetColor);
					characterToDraw[7] & 0x40 && (SGE::Display::VideoRAM[currentRAM + 6] = targetColor);
					characterToDraw[7] & 0x80 && (SGE::Display::VideoRAM[currentRAM + 7] = targetColor);
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
			int targetRAM = (targetX + (targetY * SGE::Display::ResolutionX));

			//Set the starting pint in the source data block RAM
			int sourceRAM = 0;

			//Set the maximum range for the current video RAM
			int maxRAM = SGE::Display::ResolutionX * SGE::Display::ResolutionY;

			//For each row of the source block
			for (int i = 0; i < sourceHeight; i++)
			{
				//Copy of row from the source over to the target
				memcpy(&SGE::Display::VideoRAM[targetRAM], &sourceDataBlock[sourceRAM], sourceWidth * 4);

				//Increment to the next row in the display
				targetRAM += SGE::Display::ResolutionX;

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
			int deltaX = endX - startX;									//X Delta
			int deltaY = endY - startY;									//Y Delta
			unsigned int pixelColor = PackColors(red, green, blue);		//Pixel Color data, packed together.
			int ramPosition = 0;										//Position in Video RAM
			float deltaXY = 0.0f;


			//Check for lines that cannot possibly exist on the +X, +Y quadrant
			if ((startX < 0 && endX < 0) ||														//Both X's are negative
				(startY < 0 && endY < 0) ||														//Both Y's are negative
				(startX >= SGE::Display::ResolutionX && endX >= SGE::Display::ResolutionX) ||	//Both X's are outside the resolution
				(startY >= SGE::Display::ResolutionY && endY >= SGE::Display::ResolutionY))		//Both Y's are outside the resolution
			{
				//This line doesn't exist anywhere we could possibly draw it.
				return;
			}

			//Check for straight lines outside what we can do anything about
			if ((startX < 0 && deltaX == 0) ||								//X is negative and it's going to stay that way
				(startY < 0 && deltaY == 0) ||								//Y is negative and it's going to stay that way
				(startX >= SGE::Display::ResolutionX && deltaX == 0) ||		//X is outside the resolution and it's going to stay that way
				(startY >= SGE::Display::ResolutionY && deltaY == 0))		//Y is outside the resoltuion and it's going to stay that way
			{
				//Can't draw this line
				return;
			}

			//Check to see if we need to prune the start point back to the right quardrant
			//Bring in the X
			if (startX < 0)
			{
				//Calculate new Y along X axis
				startX = 0;
				startY += (-startX * deltaY) / deltaX;
			}
			else if (startX >= SGE::Display::ResolutionX)
			{
				startX = SGE::Display::ResolutionX - 1;
				startY -= (-startX * deltaY) / deltaX;
			}

			//Bring in the Y 
			if (startY < 0)
			{
				//Calculate new X along Y axis
				startX += (-startY * deltaX) / deltaY;
				startY = 0;
			}
			else if (startY >= SGE::Display::ResolutionY)
			{
				startX -= (-startY * deltaX) / deltaY;
				startY = SGE::Display::ResolutionY - 1;
			}

			//Check to see if we need to prune the end point back to the right quardrant
			//Bring in the X
			if (endX < 0)
			{
				//Calculate new Y along X axis
				endX = 0;
				endY += (-endX * deltaY) / deltaX;
			}
			else if (endX >= SGE::Display::ResolutionX)
			{
				endX = SGE::Display::ResolutionX - 1;
				endY -= (-endX * deltaY) / deltaX;
			}

			//Bring in the Y 
			if (endY < 0)
			{
				//Calculate new X along Y axis
				endX += (-endY * deltaX) / deltaY;
				endY = 0;
			}
			else if (endY >= SGE::Display::ResolutionY)
			{
				endX -= (-endY * deltaX) / deltaY;
				endY = SGE::Display::ResolutionY;
			}

			//Double check the deltas
			//New deltas
			deltaX = endX - startX;
			deltaY = endY - startY;

			
			//Which is the bigger delta
			//Delta X is bigger and we will draw based on the X-axis
			if (deltaX * deltaX > deltaY * deltaY)
			{
				if (deltaX < 0)
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
				deltaXY = float(deltaY) / float(deltaX);

				//Start the starting RAM position
				ramPosition = startX + (SGE::Display::ResolutionX * startY);

				//Draw the line
				for (int i = 0;	(i <= deltaX); i++)
				{
					SGE::Display::VideoRAM[ramPosition + i + int (i * deltaXY) * SGE::Display::ResolutionX] = pixelColor;
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
				ramPosition = startX + (SGE::Display::ResolutionX * startY);

				//Draw the line
				for (int i = 0; (i <= deltaY); i++)
				{
					SGE::Display::VideoRAM[ramPosition + (i * SGE::Display::ResolutionX) + int(i * deltaXY)] = pixelColor;
				}
			}
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
				(startX >= SGE::Display::ResolutionX) ||	//If the X start is outside the display resolution
				(startY >= SGE::Display::ResolutionY))		//If the Y start is outside the display resolution
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

			if (startX + width >= SGE::Display::ResolutionX)
			{
				width = SGE::Display::ResolutionX - startX;
			}

			if (startY + height >= SGE::Display::ResolutionY)
			{
				height = SGE::Display::ResolutionY - startY;
			}
			
			//Get the starting point in video RAM based on desired location and size of the display
			int targetRAM = (startX + (startY * SGE::Display::ResolutionX));

			//Starting point
			int startingRAM = targetRAM;

			//Pack the colors up
			unsigned int targetColor = PackColors(rColor, gColor, bColor);

			//Draw the first row
			for (int i = 0; i < width; i++)
			{
				//Copy the color into video ram
				SGE::Display::VideoRAM[targetRAM + i] = targetColor;
			}
			
			//Then memcpy the rest of the rows from this one
			//First move to the next row
			targetRAM += SGE::Display::ResolutionX;

			//Loop through the remaining rows
			for (int i = 1; i < height; i++)
			{
				//Copy the first row to the rest of the rows
				memcpy(&SGE::Display::VideoRAM[targetRAM], &SGE::Display::VideoRAM[startingRAM], width * 4);

				//Next Row
				targetRAM += SGE::Display::ResolutionX;
			}
		}

		//Blank the video RAM with straight zeros.  Hence Zero Alpba Black.
		void ZBlank()
		{
			//Memset for the win!!
			memset(SGE::Display::VideoRAM, 0, SGE::Display::VideoRAMSize * sizeof(unsigned int));
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
			//unsigned int* targetPixelBuffer = nullptr;
			int targetPixelBufferSize = 0;

			//Scale the vertexes
			vertex1.x = int (vertex1.x * scalingFactor);
			vertex1.y = int (vertex1.y * scalingFactor);
			vertex2.x = int (vertex2.x * scalingFactor);
			vertex2.y = int (vertex2.y * scalingFactor);
			vertex3.x = int (vertex3.x * scalingFactor);
			vertex3.y = int (vertex3.y * scalingFactor);
			
			//Variables to hold the top most, bottom most, and middle point vertexes
			VertexPoint topMostVertex;
			VertexPoint bottomMostVertex;
			VertexPoint middlePointVertex;

			//Do some comparison to figure out the max, mins, and vertexes.
			//Basically figure out how this triangle is configured and arrangement
			//So we can make some decisions about how to draw it.
			//Find the topMostVertex

			//Vertex 1 is above Vertex 2
			if (vertex1.y < vertex2.y)
			{
				//Vertex 1 is above Vertex 3
				if (vertex1.y < vertex3.y)
				{
					//Vertex 1 is topmost
					topMostVertex = vertex1;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 2 is above Vertex 3
					middlePointVertex = vertex2.y < vertex3.y ? vertex2 : vertex3;
					bottomMostVertex  = vertex2.y < vertex3.y ? vertex3 : vertex2;
				}
				
				//Vertex 3 is above Vertex 1
				else
				{
					//Vertex 3 is topmost
					topMostVertex = vertex3;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 1 is above Vertex 2
					middlePointVertex = vertex1.y < vertex2.y ? vertex1 : vertex2;
					bottomMostVertex  = vertex1.y < vertex2.y ? vertex2 : vertex1;
				}
			}
			
			//Vertex 2 is higher than Vertex 1 (or equal)
			else
			{
				//Vertex 2 is above Vertex 3
				if (vertex2.y < vertex3.y)
				{
					//Vertex 2 is topmost
					topMostVertex = vertex2;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 1 is above Vertex 3
					middlePointVertex = vertex1.y < vertex3.y ? vertex1 : vertex3;
					bottomMostVertex  = vertex1.y < vertex3.y ? vertex3 : vertex1;
				}

				//Vertex 3 is above Vertex 2
				else
				{
					//Vertex 3 is topmost
					topMostVertex = vertex3;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 1 is above Vertex 2
					middlePointVertex = vertex1.y < vertex2.y ? vertex1 : vertex2;
					bottomMostVertex  = vertex1.y < vertex2.y ? vertex2 : vertex1;
				}
			}

			//Int Vector based on the DRAWING_DECIMAL_RESOLUTION
			int topMostToBottomMostVector = 0;
			int topMostToMiddlePointVector = 0;
			int middlePointToBottomMostVector = 0;

			//Int Vectors
			int topMostToBottomMostVectorInt = 0;
			int topMostToMiddlePointVectorInt = 0;
			int middlePointToBottomMostVectorInt = 0;

			//Error leftover from Int appropximation
			int topMostToBottomMostVectorError = 0;
			int topMostToMiddlePointVectorError = 0;
			int middlePointToBottomMostVectorError = 0;

			//Calculate the slopes for the lines in the triangle
			//And...
			//Catch any possible divde by 0 errors.

			//Line from the TopMost to the BottomMost
			if (bottomMostVertex.y - topMostVertex.y != 0)
			{
				//Figure out the delta between the points in relation to Y
				topMostToBottomMostVector = (DRAWING_DECIMAL_RESOLUTION * (bottomMostVertex.x - topMostVertex.x)) / (bottomMostVertex.y - topMostVertex.y);

				//Precalculate the Int, so we don't have to do weird casting in calculations below
				topMostToBottomMostVectorInt = topMostToBottomMostVector / DRAWING_DECIMAL_RESOLUTION;

				//Grab the error amount, so we don't have to recacluate everytime below, since it doesn't change.
				topMostToBottomMostVectorError = topMostToBottomMostVector % DRAWING_DECIMAL_RESOLUTION;
			}

			//Line from the TopMost to the Middle Poiint
			if (middlePointVertex.y - topMostVertex.y != 0)
			{
				//Figure out the delta between the points in relation to Y
				topMostToMiddlePointVector = (DRAWING_DECIMAL_RESOLUTION * (middlePointVertex.x - topMostVertex.x)) / (middlePointVertex.y - topMostVertex.y);

				//Precalculate the Int, so we don't have to do weird casting in calculations below
				topMostToMiddlePointVectorInt = topMostToMiddlePointVector / DRAWING_DECIMAL_RESOLUTION;

				//Grab the error amount, so we don't have to recacluate everytime below, since it doesn't change.
				topMostToMiddlePointVectorError = topMostToMiddlePointVector % DRAWING_DECIMAL_RESOLUTION;
			}

			//Line from the Middle Point to the Bottom Most
			if (bottomMostVertex.y - middlePointVertex.y != 0)
			{
				//Figure out the delta between the points in relation to Y
				middlePointToBottomMostVector = (DRAWING_DECIMAL_RESOLUTION * (bottomMostVertex.x - middlePointVertex.x)) / (bottomMostVertex.y - middlePointVertex.y);

				//Precalculate the Int, so we don't have to do weird casting in calculations below
				middlePointToBottomMostVectorInt = middlePointToBottomMostVector / DRAWING_DECIMAL_RESOLUTION;

				//Grab the error amount, so we don't have to recacluate everytime below, since it doesn't change.
				middlePointToBottomMostVectorError = middlePointToBottomMostVector % DRAWING_DECIMAL_RESOLUTION;
			}

			//Given the vertexes, figure out the pixel buffer needed
			int topMostPointVSMiddlePointX = topMostVertex.x - middlePointVertex.x;
			int bottomMostPointVSMiddlePointX = bottomMostVertex.x - middlePointVertex.x;

			//If negative, flip to get the absolute value
			(topMostPointVSMiddlePointX < 0) && (topMostPointVSMiddlePointX = -topMostPointVSMiddlePointX);

			//If negative, flip to get the absolute value
			(bottomMostPointVSMiddlePointX < 0) && (bottomMostPointVSMiddlePointX = -bottomMostPointVSMiddlePointX);
			
			//Figure out which one is bigger
			targetPixelBufferSize = (topMostPointVSMiddlePointX > bottomMostPointVSMiddlePointX) ? (topMostPointVSMiddlePointX + 1) : (bottomMostPointVSMiddlePointX + 1);

			//Load up the Row Buffer
			for (int i = 0; i < targetPixelBufferSize && i < SGE::Display::ResolutionX; i++)
			{
				SGE::Display::VideoRowBuffer[i] = targetColor;
			}

			//Start from the top most and go a long the lines between top and bottom, and top and middle.
			int currentTopMostToBottomMostX = topMostVertex.x + startX;
			int currentOtherLineX = topMostVertex.x + startX;

			//Error accumulators
			//To determine when adjustments to slope need to be made
			int currentTopMostToMostError = 0;
			int currentOtherLineError = 0;

			//Variable to keep track of current Y position
			int currentY = topMostVertex.y + startY;

			//Calculate points along the lines between TopMost and BottomMost, and TopMost and MiddlePoint
			for (; currentY < middlePointVertex.y + startY; currentY++)
			{
				//Given currentY, compare to the two points along the X axis
				int fillWidth = currentTopMostToBottomMostX - currentOtherLineX;

				//Flip the sign if needed
				//If fillWidth is below 0
				(fillWidth < 0) && (fillWidth = -fillWidth);

				//Add 4 to the fillWidth to make sure one 4-byte pixel is at least written per line.  (And to offset any 0 indexing logic.)
				fillWidth = (fillWidth + 1) * 4;

				//copy the row out
				memcpy(&SGE::Display::VideoRAM[(currentTopMostToBottomMostX < currentOtherLineX ? currentTopMostToBottomMostX : currentOtherLineX) + //Check to see which is furthest left
												SGE::Display::ResolutionX * (currentY)], SGE::Display::VideoRowBuffer, fillWidth);

				//Calculate the X points from Y using a modified Bresenham algorithm.
				currentTopMostToBottomMostX += topMostToBottomMostVectorInt;
				currentOtherLineX += topMostToMiddlePointVectorInt;

				//Accumulate error
				currentTopMostToMostError += topMostToBottomMostVectorError;
				currentOtherLineError += topMostToMiddlePointVectorError;

				//Check to see if error is high enough to warrant a correction
				if (currentTopMostToMostError > DRAWING_DECIMAL_RESOLUTION)
				{
					currentTopMostToBottomMostX++;
					currentTopMostToMostError -= DRAWING_DECIMAL_RESOLUTION;
				}
				else if (currentTopMostToMostError < -DRAWING_DECIMAL_RESOLUTION)
				{
					currentTopMostToBottomMostX--;
					currentTopMostToMostError += DRAWING_DECIMAL_RESOLUTION;
				}
				
				//Check to see if error is high enough to warrant a correction
				if (currentOtherLineError > DRAWING_DECIMAL_RESOLUTION)
				{
					currentOtherLineX++;
					currentOtherLineError -= DRAWING_DECIMAL_RESOLUTION;
				}
				else if (currentOtherLineError < -DRAWING_DECIMAL_RESOLUTION)
				{
					currentOtherLineX--;
					currentOtherLineError += DRAWING_DECIMAL_RESOLUTION;
				}
			}

			//Reset the currentOtherLineX to the middlePointVertex
			currentOtherLineX = middlePointVertex.x + startX;

			//Reset the error on the current line, since it is starting from a new point
			currentOtherLineError = 0;

			//Calculate points along the lines between TopMost and BottomMost, and MiddlePoint and BottomMost
			for (; currentY <= bottomMostVertex.y +startY; currentY++)
			{
				//Given currentY, compare to the two points along the X axis
				int fillWidth = currentTopMostToBottomMostX - currentOtherLineX;

				//Flip the sign if needed
				//If fillWidth is below 0
				(fillWidth < 0) && (fillWidth = -fillWidth);

				//Add 4 to the fillWidth to make sure one 4-byte pixel is at least written per line.  (And to offset any 0 indexing logic.)
				fillWidth = (fillWidth + 1) * 4;

				//Copy the row out
				memcpy(&SGE::Display::VideoRAM[(currentTopMostToBottomMostX < currentOtherLineX ? currentTopMostToBottomMostX : currentOtherLineX) + //Check to see which is furthest left
												SGE::Display::ResolutionX * (currentY)], SGE::Display::VideoRowBuffer, fillWidth);

				//Calculate the X points from Y using a modified Bresenham algorithm.
				currentTopMostToBottomMostX += topMostToBottomMostVectorInt;
				currentOtherLineX += middlePointToBottomMostVectorInt;

				//Accumulate error
				currentTopMostToMostError += topMostToBottomMostVectorError;
				currentOtherLineError += middlePointToBottomMostVectorError;

				//Check to see if error is high enough to warrant a correction
				if (currentTopMostToMostError > DRAWING_DECIMAL_RESOLUTION)
				{
					currentTopMostToBottomMostX++;
					currentTopMostToMostError -= DRAWING_DECIMAL_RESOLUTION;
				}
				else if (currentTopMostToMostError < -DRAWING_DECIMAL_RESOLUTION)
				{
					currentTopMostToBottomMostX--;
					currentTopMostToMostError += DRAWING_DECIMAL_RESOLUTION;
				}

				//Check to see if error is high enough to warrant a correction
				if (currentOtherLineError > DRAWING_DECIMAL_RESOLUTION)
				{
					currentOtherLineX++;
					currentOtherLineError -= DRAWING_DECIMAL_RESOLUTION;
				}
				else if (currentOtherLineError < -DRAWING_DECIMAL_RESOLUTION)
				{
					currentOtherLineX--;
					currentOtherLineError += DRAWING_DECIMAL_RESOLUTION;
				}
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
			for (int y = yMin; y <= yMax; y++)
			{
				for (int x = xMin; x <= xMax; x++)
				{
					if (partialProductY12 - partialProductX12 >= 0 &&
						partialProductY23 - partialProductX23 >= 0 &&
						partialProductY31 - partialProductX31 >= 0)
					{
						//Copy the color over.
						SGE::Display::VideoRAM[x + (y*SGE::Display::ResolutionX)] = targetColor;
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