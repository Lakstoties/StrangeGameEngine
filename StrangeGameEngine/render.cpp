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
								memcpy(&SGE::Display::VideoRAM[currentRAM], &targetColor, 4);
							}
							//Hop to the next spot in RAM regardless
							currentRAM++;
						}

						//Hop to the next row in RAM from the end of the current row
						currentRAM += (SGE::Display::ResolutionX - 8);
					}
					else
					{
						//Hop to the next row in RAM from the start of the current row
						currentRAM += (SGE::Display::ResolutionX);
					}
				}
			}

		}

		//Draw a pixel
		//Provided as a utility function, not the most efficent for mass writing, but good for spot stuff.
		void DrawPixel(
			int targetX,								//Target X location to draw
			int targetY,								//Target Y location to draw
			unsigned char rColor,						//8-bit (0-255) Red component of the pixel color
			unsigned char gColor,						//8-bit (0-255) Green component of the pixel color
			unsigned char bColor)						//8-bit (0-255) Blue component of the pixel color
		{
			//Set the location in video RAM
			int targetRAM = targetX + targetY * SGE::Display::ResolutionX;

			//As long as we haven't left valid video RAM ranges...
			if (targetRAM >= 0 && targetRAM < SGE::Display::ResolutionX * SGE::Display::ResolutionY)
			{
				//Write the pixel
				SGE::Display::VideoRAM[targetRAM] = PackColors(rColor, gColor, bColor);
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
			int startX,									//Target X location to start drawing from
			int startY,									//Target Y location to start drawing from
			int endX,									//Target X location to end drawing at
			int endY,									//Target Y location to end drawing at
			unsigned char rColor,						//8-bit (0-255) Red component of the pixel color
			unsigned char gColor,						//8-bit (0-255) Green component of the pixel color
			unsigned char bColor)						//8-bit (0-255) Blue component of the pixel color
		{
			//Check for a completely invalid line
			//If all points exist in the -x,-y quardrant... Fuck this shit... We're out!
			if (startX < 0 && startY < 0 && endX < 0 && endY < 0)
			{
				//This line cannot exist on the display in any shape or form.
				//No need to continue processing.
				return;
			}
			
			//Calculate the X and Y deltas
			int deltaX = endX - startX;
			int deltaY = endY - startY;

			//Determine the type of traversal
			//Variables to keep track important things
			int iterationDelta = 0;
			int otherDelta = 0;
			int deltaRAMStep = 0;
			int errorRAMStep = 0;
			int iterationTruncation = 0;

			//Check for any points outside the valid display bounds
			//If found, use deltas to calculate new valid point

			//Check startX and startY
			//Checking startX
			if (startX < 0)
			{
				//If deltaX is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaX == 0)
				{
					return;
				}

				//Calculate the new startY based on how far from 0 startX is.
				startY += (-startX * deltaY) / deltaX;

				//Set startX to 0
				startX = 0;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}
			else if (startX >= SGE::Display::ResolutionX)
			{
				//If deltaX is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaX == 0)
				{
					return;
				}

				//Calculate the new startY based on how far from the last horizontal pixel startX is.
				startY -= ((startX - (SGE::Display::ResolutionX - 1)) * deltaY) / deltaX;

				//Start startX to the last horizontal pixel.
				startX = SGE::Display::ResolutionX - 1;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}

			//Checking startY
			if (startY < 0)
			{
				//If deltaY is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaY == 0)
				{
					return;
				}
				
				//Calculate the new startX based on how far from 0 startY is.
				startX += (-startY * deltaX) / deltaY;

				//Set startY to 0
				startY = 0;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}
			else if (startY >= SGE::Display::ResolutionY)
			{
				//If deltaY is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaY == 0)
				{
					return;
				}

				//Calculate the new startX basedo n how far fromthe last vertical pixel startY is.
				startX -= ((startY - (SGE::Display::ResolutionY - 1)) * deltaX) / deltaY;

				//Set startY to the last vertical pixel.
				startY = SGE::Display::ResolutionY - 1;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}

			//If startX or startX are negative after attempts to calculate a point within the bounds of the display
			//Then this line ain't happening.
			if (startX < 0 || startY < 0)
			{
				//Point and slope done describe a valid lines to chart on the display
				return;
			}


			//Check endX and endY
			//Checking endX
			if (endX < 0)
			{
				//If deltaX is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaX == 0)
				{
					return;
				}

				//Calculate the new startY based on how far from 0 startX is.
				endY += (-endX * deltaY) / deltaX;

				//Set startX to 0
				endX = 0;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}
			else if (endX >= SGE::Display::ResolutionX)
			{
				//If deltaX is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaX == 0)
				{
					return;
				}

				//Calculate the new startY based on how far from the last horizontal pixel startX is.
				endY -= ((endX - (SGE::Display::ResolutionX - 1)) * deltaY) / deltaX;

				//Start startX to the last horizontal pixel.
				endX = SGE::Display::ResolutionX - 1;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}

			//Checking startY
			if (endY < 0)
			{
				//If deltaY is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaY == 0)
				{
					return;
				}

				//Calculate the new startX based on how far from 0 startY is.
				endX += (-endY * deltaX) / deltaY;

				//Set startY to 0
				endY = 0;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}
			else if (endY >= SGE::Display::ResolutionY)
			{
				//If deltaY is 0, there's NO WAY this line is getting to a valid point on the screen
				if (deltaY == 0)
				{
					return;
				}

				//Calculate the new startX basedo n how far fromthe last vertical pixel startY is.
				endX -= ((endY - (SGE::Display::ResolutionY - 1)) * deltaX) / deltaY;

				//Set startY to the last vertical pixel.
				endY = SGE::Display::ResolutionY - 1;

				//Recalculate the deltas
				deltaX = endX - startX;
				deltaY = endY - startY;
			}

			//If endX or endY are negative after attempts to calculate a point within the bounds of the display
			//Then this line ain't happening.
			if (endX < 0 || endY < 0)
			{
				//Point and slope done describe a valid lines to chart on the display
				return;
			}
			
			//Either X or Y traversal, depending on which delta is larger.
			//Multipy both by themselves, to get rid of negative signs
			//If X is the bigger delta
			if (deltaX * deltaX > deltaY * deltaY)
			{
				//Set the big and small delta
				iterationDelta = deltaX;
				otherDelta = deltaY;

				//Set the RAM stepping based on if the deltaX is negative or positive
				if (deltaX > 0)
				{
					deltaRAMStep = 1;
				}
				else
				{
					deltaRAMStep = -1;			
				}

				if (deltaY > 0)
				{
					errorRAMStep = SGE::Display::ResolutionX;
				}
				else
				{
					errorRAMStep = -SGE::Display::ResolutionX;
				}
			}
			//Else Y is the delta we need to iterate across.
			else
			{
				//Set the big and small delta
				iterationDelta = deltaY;
				otherDelta = deltaX;

				//Set the RAM stepping based on if the deltaY is negative or positive
				if (deltaY > 0)
				{
					deltaRAMStep = SGE::Display::ResolutionX;
				}
				else
				{
					deltaRAMStep = -SGE::Display::ResolutionX;
				}

				if (deltaX > 0)
				{
					errorRAMStep = 1;
				}
				else
				{
					errorRAMStep = -1;
				}
			}

			//Calculate the integer slope amount and error decimal
			int errorDelta = (otherDelta * DRAWING_DECIMAL_RESOLUTION) / iterationDelta;

			//Get the absolute value of the erroDelta, so make sure it is positive.
			if (errorDelta < 0)
			{
				errorDelta = -errorDelta;
			}
			
			//Initialize the current error
			int currentError = -DRAWING_DECIMAL_RESOLUTION;

			//Determine starting RAM location
			int currentRAMLocation = startX + startY * SGE::Display::ResolutionX;

			//Pack the colors up into useful data
			//Since if we have gotten this far, we are actually going to try to plot this thing
			unsigned int pixelData = PackColors(rColor, gColor, bColor);

			//Make sure the bigDelta sign if positive (get the cheap absolute value)
			if (iterationDelta < 0)
			{
				iterationDelta = -iterationDelta;
			}

			//Step through each point, going the direction of the bigger delta
			for (int i = 0; i <= iterationDelta; i++)
			{
				//Plot a point for the current location
				memcpy(&SGE::Display::VideoRAM[currentRAMLocation], &pixelData, 4);

				//Move to the next place in memory
				currentRAMLocation += deltaRAMStep;

				//Process the error
				currentError += errorDelta;

				//Check to see if enough error has accumulated to warrant correction.
				//In the negative direction
				if (currentError >= 0)
				{
					currentRAMLocation += errorRAMStep;
					currentError += -DRAWING_DECIMAL_RESOLUTION;
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
			DrawLine(startX, startY + height - 1, startX + width - 1, startY + height, rColor, gColor, bColor);
			
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
				memcpy(&SGE::Display::VideoRAM[targetRAM+i], &targetColor, 4);
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
			//Get the display RAM size in byte chunks
			int displayRAMSize = SGE::Display::ResolutionX * SGE::Display::ResolutionY * sizeof(unsigned int);

			//Memset for the win!!
			memset(SGE::Display::VideoRAM, 0, displayRAMSize);
		}

		//Blanks the video RAM completely with a choosen color
		void Blank(
			unsigned char rColor,					//8-bit (0-255) Red component of the color
			unsigned char gColor,					//8-bit (0-255) Green component of the color
			unsigned char bColor)					//8-bit (0-255) Blue component of the color
		{
			//Get the display RAM size in 32-bit chunks
			int displayRAMSize = SGE::Display::ResolutionX * SGE::Display::ResolutionY;

			//Pack up the color components into a 32-bit chunk
			unsigned int pixelValues = PackColors(rColor, gColor, bColor);

			//Fill the first "row"
			for (int i = 0; i < SGE::Display::ResolutionX; i++)
			{
				memcpy(SGE::Display::VideoRAM + i, &pixelValues, 4);
			}

			//File subsequent rows with the first
			for (int i = SGE::Display::ResolutionX; i < displayRAMSize; i = i + SGE::Display::ResolutionX)
			{
				memcpy(&SGE::Display::VideoRAM[i], SGE::Display::VideoRAM, SGE::Display::ResolutionX * 4);
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
			unsigned int* targetPixelBuffer = nullptr;
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
					if (vertex2.y < vertex3.y)
					{
						middlePointVertex = vertex2;
						bottomMostVertex = vertex3;
					}
					else
					{
						middlePointVertex = vertex3;
						bottomMostVertex = vertex2;
					}
				}
				
				//Vertex 3 is above Vertex 1
				else
				{
					//Vertex 3 is topmost
					topMostVertex = vertex3;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 1 is above Vertex 2
					if (vertex1.y < vertex2.y)
					{
						middlePointVertex = vertex1;
						bottomMostVertex = vertex2;
					}
					else
					{
						middlePointVertex = vertex2;
						bottomMostVertex = vertex1;
					}
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
					if (vertex1.y < vertex3.y)
					{
						middlePointVertex = vertex1;
						bottomMostVertex = vertex3;
					}
					else
					{
						middlePointVertex = vertex3;
						bottomMostVertex = vertex1;
					}
				}

				//Vertex 3 is above Vertex 2
				else
				{
					//Vertex 3 is topmost
					topMostVertex = vertex3;

					//Compare the remain two figure out the Bottom and middle
					//If Vertex 1 is above Vertex 2
					if (vertex1.y < vertex2.y)
					{
						middlePointVertex = vertex1;
						bottomMostVertex = vertex2;
					}
					else
					{
						middlePointVertex = vertex2;
						bottomMostVertex = vertex1;
					}
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
			if (topMostPointVSMiddlePointX < 0)
			{
				topMostPointVSMiddlePointX = -topMostPointVSMiddlePointX;
			}

			//If negative, flip to get the absolute value
			if (bottomMostPointVSMiddlePointX < 0)
			{
				bottomMostPointVSMiddlePointX = -bottomMostPointVSMiddlePointX;
			}
			
			//Figure out which one is bigger
			if (topMostPointVSMiddlePointX > bottomMostPointVSMiddlePointX)
			{
				targetPixelBufferSize = topMostPointVSMiddlePointX + 1;
			}
			else
			{
				targetPixelBufferSize = bottomMostPointVSMiddlePointX + 1;
			}

			//Create and load up the pixel buffer
			//targetPixelBuffer = new unsigned int[targetPixelBufferSize];
			targetPixelBuffer = (unsigned int*) malloc(sizeof(unsigned int) * targetPixelBufferSize);

			for (int i = 0; i < targetPixelBufferSize; i++)
			{
				memcpy(&targetPixelBuffer[i], &targetColor, 4);
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
				if (fillWidth < 0)
				{
					fillWidth = -fillWidth;
				}

				//Add 4 to the fillWidth to make sure one 4-byte pixel is at least written per line.  (And to offset any 0 indexing logic.)
				fillWidth = (fillWidth + 1) * 4;

				//Check to see which is furthest left
				if (currentTopMostToBottomMostX < currentOtherLineX)
				{
					memcpy(&SGE::Display::VideoRAM[currentTopMostToBottomMostX + SGE::Display::ResolutionX * (currentY)], targetPixelBuffer, fillWidth);
				}
				else
				{
					memcpy(&SGE::Display::VideoRAM[currentOtherLineX + SGE::Display::ResolutionX * (currentY)], targetPixelBuffer, fillWidth);
				}

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
				if (fillWidth < 0)
				{
					fillWidth = -fillWidth;
				}

				//Add 4 to the fillWidth to make sure one 4-byte pixel is at least written per line.  (And to offset any 0 indexing logic.)
				fillWidth = (fillWidth + 1) * 4;

				//Check to see which is furthest left
				if (currentTopMostToBottomMostX < currentOtherLineX)
				{
					memcpy(&SGE::Display::VideoRAM[currentTopMostToBottomMostX + SGE::Display::ResolutionX * (currentY)], targetPixelBuffer, fillWidth);
				}
				else
				{
					memcpy(&SGE::Display::VideoRAM[currentOtherLineX + SGE::Display::ResolutionX * (currentY)], targetPixelBuffer, fillWidth);
				}

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

			//Clean up the pixel buffer
			free(targetPixelBuffer);
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
						memcpy(&SGE::Display::VideoRAM[x + (y*SGE::Display::ResolutionX)], &targetColor, 4);
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