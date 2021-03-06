﻿#include "include\SGE\render.h"
#include "include\SGE\inputs.h"
#include <vector>
#include <cmath>

//
//  Strange Game Engine Main Namespace
//
namespace SGE::Render
{
    //
    //  Draw Simple Mouse Cursor
    //
    void DrawMouseSimpleCursor(unsigned int cursorRadius, SGE::Display::Video::pixel cursorColor)
    {
        //
        //  Check to make sure the cursor can even be drawn.
        //
        if ((SGE::Inputs::Mouse::PositionX <= -(int)cursorRadius) ||
            (SGE::Inputs::Mouse::PositionY <= -(int)cursorRadius) ||
            (SGE::Inputs::Mouse::PositionX >= SGE::Display::Video::X + (int)cursorRadius) ||
            (SGE::Inputs::Mouse::PositionY >= SGE::Display::Video::Y + (int)cursorRadius))
        {
            //This can't be draw, too far off the drawable area.
            return;
        }

        //
        //  Set the start and end points for drawing the cursor box
        //
        int startX = SGE::Inputs::Mouse::PositionX - (int)cursorRadius;
        int startY = SGE::Inputs::Mouse::PositionY - (int)cursorRadius;
        int endX = SGE::Inputs::Mouse::PositionX + (int)cursorRadius;
        int endY = SGE::Inputs::Mouse::PositionY + (int)cursorRadius;

        //
        //  Prune down to a valid drawing area.
        //
        if (startX < 0) { startX = 0; }
        if (startY < 0) { startY = 0; }
        if (endX >= SGE::Display::Video::X) { endX = SGE::Display::Video::X; }
        if (endY >= SGE::Display::Video::Y) { endY = SGE::Display::Video::Y; }

        //
        //  Draw the cursor
        //
        for (int drawY = startY; drawY < endY; drawY++)
        {
            for (int drawX = startX; drawX < endX; drawX++)
            {
                SGE::Display::Video::RAM[drawY * SGE::Display::Video::X + drawX] = cursorColor;
            }
        }
    }


    //
    //  Draws a string of characters
    //
    void DrawString(
        const std::string& characters,
        const unsigned long long* characterROM,		//64-bit Character ROM array to map characters against
        const int characterSpacing,						//The amount space to give each character, standard spacing is 8 to go along with the 8X8 character size
        const int targetX,								//Target X location to start drawing from (Upper Left Corner)
        const int targetY,								//Target Y location to start drawing from (Upper Left Corner)
        const SGE::Display::Video::pixel targetColor) 	//Target pixel data for the color
    {
        //Initialize the stringPosition to the starting point for any string.
        int stringPosition = 0;

        //While we haven't hit the null termination for the string.
        for (char character : characters)
        {
            //Call the Draw the 8x8 character
            Draw8x8Character(&characterROM[(unsigned char)character], targetX + stringPosition*characterSpacing, targetY, targetColor);

            //Move to the next position in the string
            stringPosition++;
        }
    }

    void DrawString8x16(std::string characters, const unsigned long long* characterROM, int characterSpacing, int targetX, int targetY, SGE::Display::Video::pixel color)
    {
        //While we haven't hit the null termination for the string.
        for (int i = 0; i < characters.length(); i++)
        {
            //Call the Draw the 8x16 character
            Draw8x16Character(&characterROM[(unsigned char)characters[i]*2], targetX + i * characterSpacing, targetY, color);
        }
    }

    void Draw8x8CharacterFilled(
        const unsigned long long* character,
        const int targetX,
        const int targetY,
        const SGE::Display::Video::pixel targetForegroundColor,
        const SGE::Display::Video::pixel targetBackgroundColor
    )
    {
        //Get a pointer to what we are interested in.
        //Cast down to an unsigned char so to work with 8-bit at a time of the 64-bit
        const unsigned char* characterToDraw = (unsigned char*) character;

        //  Special Case
        if (targetY < 0 || (targetY + 7) >= SGE::Display::Video::Y || targetX < 0 || (targetX + 7) >= SGE::Display::Video::X)
        {
            for (int i = 0; i < 8; i++)
            {
                for (unsigned int j = 0; j < 8; j++)
                {
                    if ((targetY + i >= 0) && (targetY + i < SGE::Display::Video::Y) && (targetX + j >= 0) && (targetX + j < SGE::Display::Video::X))
                    {
                        SGE::Display::Video::RAM[targetX + j + (targetY + i) * SGE::Display::Video::X] = (characterToDraw[i] & 0x01u << j) ? targetForegroundColor : targetBackgroundColor;
                    }
                }
            }
        }

        //  Normal Case
        else
        {
            //Figure out the target RAM
            SGE::Display::Video::pixel* targetRAM = &SGE::Display::Video::RAM[targetX + targetY * SGE::Display::Video::X];

            for (int i = 0; i < 8; i++)
            {
                targetRAM[0] = (characterToDraw[i] & 0x01u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[1] = (characterToDraw[i] & 0x02u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[2] = (characterToDraw[i] & 0x04u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[3] = (characterToDraw[i] & 0x08u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[4] = (characterToDraw[i] & 0x10u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[5] = (characterToDraw[i] & 0x20u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[6] = (characterToDraw[i] & 0x40u) ? targetForegroundColor : targetBackgroundColor;
                targetRAM[7] = (characterToDraw[i] & 0x80u) ? targetForegroundColor : targetBackgroundColor;

                //  Next to next Row
                targetRAM += SGE::Display::Video::X;
            }
        }
    }

    void Draw8x16CharacterFilled(
            const unsigned long long* character,
            const int targetX, const int targetY,
            const SGE::Display::Video::pixel targetForegroundColor,
            const SGE::Display::Video::pixel targetBackgroundColor)
    {
        //Pretty simple process, just use what's already there.

        //  Draw the top half
        Draw8x8CharacterFilled(character, targetX, targetY, targetForegroundColor, targetBackgroundColor);

        //  Draw the bottom half
        Draw8x8CharacterFilled(character+1, targetX, targetY + 8, targetForegroundColor, targetBackgroundColor);
    }

    //
    //  Draw a single character
    //
    void Draw8x8Character(
            const unsigned long long* character,			//64-bit Character ROM array to map characters against
            const int targetX,								//Target X location to start drawing from (Upper Left Corner)
            const int targetY,								//Target Y location to start drawing from (Upper Left Corner)
            const SGE::Display::Video::pixel targetColor)	//Target pixel data for the color
    {
        //Get a pointer to what we are interested in.
        //Cast down to an unsigned char so to work with 8-bit at a time of the 64-bit
        const unsigned char* characterToDraw = (unsigned char*)character;

        //  Is there even a character to draw?
        if (*character == 0)
        {
            //  Nothing to draw
            return;
        }

        //  Special Case
        else if (targetY < 0 || (targetY + 7) >= SGE::Display::Video::Y || targetX < 0 || (targetX + 7) >= SGE::Display::Video::X)
        {
            for (int i = 0; i < 8; i++)
            {
                for (unsigned int j = 0; j < 8; j++)
                {
                    if ((characterToDraw[i] & 0x01u << j) && (targetY + i >= 0) && (targetY + i < SGE::Display::Video::Y) && (targetX + j >= 0) && (targetX + j < SGE::Display::Video::X))
                    {
                        SGE::Display::Video::RAM[targetX + j + (targetY + i) * SGE::Display::Video::X] = targetColor;
                    }
                }
            }
        }

        //  Normal Case
        else
        {
            //Figure out the target RAM
            SGE::Display::Video::pixel* targetRAM = &SGE::Display::Video::RAM[targetX + targetY * SGE::Display::Video::X];

            for (int i = 0; i < 8; i++)
            {
                if (characterToDraw[i])
                {
                    if (characterToDraw[i] & 0x01u) { targetRAM[0] = targetColor; }
                    if (characterToDraw[i] & 0x02u) { targetRAM[1] = targetColor; }
                    if (characterToDraw[i] & 0x04u) { targetRAM[2] = targetColor; }
                    if (characterToDraw[i] & 0x08u) { targetRAM[3] = targetColor; }
                    if (characterToDraw[i] & 0x10u) { targetRAM[4] = targetColor; }
                    if (characterToDraw[i] & 0x20u) { targetRAM[5] = targetColor; }
                    if (characterToDraw[i] & 0x40u) { targetRAM[6] = targetColor; }
                    if (characterToDraw[i] & 0x80u) { targetRAM[7] = targetColor; }
                }

                //  Next to next Row
                targetRAM += SGE::Display::Video::X;
            }
        }
    }

    //
    //  Draw 8 x 16 Character
    //
    void Draw8x16Character(const unsigned long long* character, const int targetX, const int targetY, const SGE::Display::Video::pixel color)
    {
        //  Draw the top half
        Draw8x8Character(character, targetX, targetY, color);

        //  Draw the bottom half
        Draw8x8Character(character + 1, targetX, targetY + 8, color);
    }


    //
    //  Draw Block Data - copy a block of image data over
    //
    void DrawDataBlock(int targetX, int targetY, int sourceWidth, int sourceHeight, SGE::Display::Video::pixel* sourceDataBlock)
    {
        //
        //  Sanity Checks
        //

        //  Are we completely off in the -X area?
        if ((targetX + sourceWidth) <= 0)
        {
            // Can't draw this
            return;
        }

        //  Are we completely off in the -Y area?
        if ((targetY + sourceHeight) <= 0)
        {
            // Can't draw this either
            return;
        }

        //  Are we completely off in the Beyond X area?
        if ((targetX >= SGE::Display::Video::X))
        {
            // Yep, can't draw this either
            return;
        }

        //  Are we completely off in the Beyond Y area?
        if ((targetY >= SGE::Display::Video::Y))
        {
            // Yes, yes, can't draw this either
        }

        //
        //  In theory, there's something to draw here.
        //

        int startX = targetX;
        int startY = targetY;
        int width = sourceWidth;
        int height = sourceHeight;
        int sourceXOffset = 0;
        int sourceYOffset = 0;

        //
        //  Check for partial draw situations
        //

        //
        //  Check for overdraw in -X
        //
        if (startX < 0)
        {
            sourceXOffset -= startX;
            width += startX;
            startX = 0;
        }

        //
        //  Check for overdraw in +X
        //
        if ((startX + sourceWidth) >= SGE::Display::Video::X)
        {
            width = SGE::Display::Video::X - startX;
        }

        //
        //  Check for overdraw in -Y
        //
        if (startY < 0)
        {
            sourceYOffset -= startY;
            height += startY;
            startY = 0;
        }

        //
        //  Check for overdraw in the Y
        //
        if ((startY + sourceHeight) >= SGE::Display::Video::Y)
        {
            height = SGE::Display::Video::Y - startY;
        }


        //Set the starting point in VideoRAM
        int targetRAM = startX + (startY * SGE::Display::Video::X);

        //Set the starting pint in the source data block RAM
        int sourceRAM = sourceXOffset + (sourceYOffset * sourceWidth);

        //For each row of the source block
        for (int i = 0; i < height; i++)
        {
            //Copy of row from the source over to the target
            std::copy(sourceDataBlock + sourceRAM, sourceDataBlock + sourceRAM + width, SGE::Display::Video::RAM + targetRAM);

            //Increment to the next row in the display
            targetRAM += SGE::Display::Video::X;

            //Increment to the next row in the source
            sourceRAM += sourceWidth;
        }
    }

    void DrawLine(
        int startX,
        int startY,
        int endX,
        int endY,
        const SGE::Display::Video::pixel color)
    {
        int ramPosition;										//Position in Video RAM
        float deltaXY;

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
                int temp;

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
            ramPosition = startX + (SGE::Display::Video::X * startY);

            //Draw the line
            for (int i = 0; (i <= deltaX); i++)
            {
                SGE::Display::Video::RAM[ramPosition + i + (int)((float)i * deltaXY) * SGE::Display::Video::X] = color;
            }
        }

        //Delta Y is bigger and we will draw based on the Y-axis
        else
        {
            if (deltaY < 0)
            {
                //Swap the points if there's a negative X delta
                int temp;

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
            ramPosition = startX + (SGE::Display::Video::X * startY);

            //Draw the line
            for (int i = 0; (i <= deltaY); i++)
            {
                SGE::Display::Video::RAM[ramPosition + (i * SGE::Display::Video::X) + (int)((float)i * deltaXY)] = color;
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
            (startX >= SGE::Display::Video::X && endX >= SGE::Display::Video::X) ||	//Both X's are outside the resolution
            (startY >= SGE::Display::Video::Y && endY >= SGE::Display::Video::Y))	//Both Y's are outside the resolution
        {
            //This line doesn't exist anywhere we could possibly draw it.
            return false;
        }

        //Check for straight lines outside what we can do anything about
        if ((startX < 0 && deltaX == 0) ||										//X is negative and it's going to stay that way
            (startY < 0 && deltaY == 0) ||										//Y is negative and it's going to stay that way
            (startX >= SGE::Display::Video::X && deltaX == 0) ||		//X is outside the resolution and it's going to stay that way
            (startY >= SGE::Display::Video::Y && deltaY == 0))		//Y is outside the resolution and it's going to stay that way
        {
            //Can't draw this line
            return false;
        }

        //Check to see if we need to prune the start point back to the right quadrant
        //Bring in the X
        if (startX < 0)
        {
            //Calculate new Y along X axis
            startX = 0;
            startY += (-startX * deltaY) / deltaX;
        }
        else if (startX >= SGE::Display::Video::X)
        {
            startX = SGE::Display::Video::X - 1;
            startY -= (-startX * deltaY) / deltaX;
        }

        //Bring in the Y
        if (startY < 0)
        {
            //Calculate new X along Y axis
            startX += (-startY * deltaX) / deltaY;
            startY = 0;
        }
        else if (startY >= SGE::Display::Video::Y)
        {
            startX -= (-startY * deltaX) / deltaY;
            startY = SGE::Display::Video::Y - 1;
        }

        //Check to see if we need to prune the end point back to the right quadrant
        //Bring in the X
        if (endX < 0)
        {
            //Calculate new Y along X axis
            endX = 0;
            endY += (-endX * deltaY) / deltaX;
        }
        else if (endX >= SGE::Display::Video::X)
        {
            endX = SGE::Display::Video::X - 1;
            endY -= (-endX * deltaY) / deltaX;
        }

        //Bring in the Y
        if (endY < 0)
        {
            //Calculate new X along Y axis
            endX += (-endY * deltaX) / deltaY;
            endY = 0;
        }
        else if (endY >= SGE::Display::Video::Y)
        {
            endX -= (-endY * deltaX) / deltaY;
            endY = SGE::Display::Video::Y;
        }


        //In theory we've trimmed the line down to valid points within the viewable area of the screen
        return true;
    }

    //Draw a hollow rectangle of a given color and in a desired location of a desired size
    void DrawRectangle(
        int startX,									//Starting X to draw from (Upper Left Corner)
        int startY,									//Starting Y to draw from (Upper Left Corner)
        int width,									//Width of rectangle to draw
        int height,									//Height of rectangle to draw
        const SGE::Display::Video::pixel color)		//Pixel color data to use
    {

        //
        //  Sanity check to see if we can even draw this thing
        //
        if (startX + width < 0 ||							//If the start X is further off screen than the width
            startX >= SGE::Display::Video::X ||	//If the start X is beyond the maximum drawable
            startY + height < 0 ||							//If the start Y is further off screen than the height
            startY >= SGE::Display::Video::Y)		//If the start Y is beyond the maximum drawable
        {
            //You kidding me?  We can't draw this shit!
            return;
        }

        //
        //  Prune down the rectangle to acceptable drawing limits
        //

        //
        //  For X's outside normal ranges
        //


        //If the starting X is in the negative zone
        if (startX < 0)
        {
            //The drawing width is adjusted based on how far in the negative the startX is
            width += startX;

            //And the starting X is now 0
            startX = 0;
        }

        //If the rectangle wander off the drawable area
        if (startX + width >= SGE::Display::Video::X)
        {
            //THe drawing width is adjusted by how far off the drawable edge it goes
            width -= (startX + width) - SGE::Display::Video::X;
        }

        //
        //  For Y's outside normal ranges
        //

        //If the starting X is in the negative zone
        if (startY < 0)
        {
            //The drawing width is adjusted based on how far in the negative the startX is
            height += startY;

            //And the starting X is now 0
            startY = 0;
        }

        //If the rectangle wander off the drawable area
        if (startY + height >= SGE::Display::Video::Y)
        {
            //THe drawing width is adjusted by how far off the drawable edge it goes
            height -= (startY + height) - SGE::Display::Video::Y;
        }

        //
        //  Time to draw it!
        //

        //
        //  Start with the top and bottom rows
        //

        //Figure out the top and bottom row start positions in video ram
        SGE::Display::Video::pixel* topLeftRAMPointer = &SGE::Display::Video::RAM[startX + (startY * SGE::Display::Video::X)];
        SGE::Display::Video::pixel* bottomLeftRAMPointer = &SGE::Display::Video::RAM[startX + ((startY + height - 1) * SGE::Display::Video::X)];
        SGE::Display::Video::pixel* topRightRAMPointer = &topLeftRAMPointer[width - 1];

        //Start the rows
        for (int i = 0; i < width; i++)
        {
            bottomLeftRAMPointer[i] = color;
            topLeftRAMPointer[i] = color;
        }

        //Start the columns
        for (int i = 0; i < height * SGE::Display::Video::X; i += SGE::Display::Video::X)
        {
            topRightRAMPointer[i] = color;
            topLeftRAMPointer[i] = color;
        }
    }


    //Draws a filled rectangle or box of a given color and in a desired location of a desired size
    void DrawBox(
        int startX,									//Starting X to draw from (Upper Left Corner)
        int startY,									//Starting Y to draw from (Upper Left Corner)
        int width,									//Width of box to draw
        int height,									//Height of box to draw
        const SGE::Display::Video::pixel color)		//Pixel color data
    {
        //Check for boxes we cannot draw...
        if ((startX + width < 0) ||						//If the X never graces the +x, +y quadrant
            (startY + height < 0) ||					//If the Y never graces the +x, +y quadrant
            (startX >= SGE::Display::Video::X) ||	//If the X start is outside the display resolution
            (startY >= SGE::Display::Video::Y))		//If the Y start is outside the display resolution
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

        if (startX + width >= SGE::Display::Video::X)
        {
            width = SGE::Display::Video::X - startX;
        }

        if (startY + height >= SGE::Display::Video::Y)
        {
            height = SGE::Display::Video::Y - startY;
        }


        //  Get the starting point in video RAM based on desired location and size of the display
        SGE::Display::Video::pixel* startPointInRAM = SGE::Display::Video::RAM + startX + (startY * SGE::Display::Video::X);

        //
        //  Straight assignment
        //
        for (int j = 0; j < height * SGE::Display::Video::X; j += SGE::Display::Video::X)
        {
            for (int i = 0; i < width; i++)
            {
                startPointInRAM[i + j] = color;
            }
        }
    }

    //Blank the video RAM with straight zeros.  Hence Zero Alpha Black.
    void ZBlank()
    {
        //std::fill for it all
        std::fill(SGE::Display::Video::RAM, SGE::Display::Video::RAM + SGE::Display::Video::X * SGE::Display::Video::Y, (SGE::Display::Video::pixel) 0x00000000);
    }

    //Packs the Red, Green, and Blue 8-bit components with a dummy Alpha value into a 32-bit unsigned int
    unsigned int PackColors(
        unsigned char redValue,			//8-bit (0-255) Red component of the color
        unsigned char greenValue,		//8-bit (0-255) Green component of the color
        unsigned char blueValue)		//8-bit (0-255) Blue component of the color
    {
        //Bit shift this shit and return it.
        //Will work so long as Alpha blending is disabled in OpenGL
        return (unsigned int)blueValue << 16u | (unsigned int)greenValue << 8u | (unsigned int)redValue;
    }


    void DrawVectorShape(int startX, int startY, float scalingFactor, int numberOfVertexes, VertexPoint vertexes[], SGE::Display::Video::pixel color)
    {
        //Go through the vertex point list and draw lines
        for (int i = 0; i < numberOfVertexes; i++)
        {
            //Draw a line between two points on the vertex, wrapping the last and first at the very end.
            DrawLine(
                startX + (int)((float)(vertexes[i].x) * scalingFactor),
                startY + (int)((float)(vertexes[i].y) * scalingFactor),
                startX + (int)((float)(vertexes[(i + 1) % numberOfVertexes].x) * scalingFactor),
                startY + (int)((float)(vertexes[(i + 1) % numberOfVertexes].y) * scalingFactor),
                color);
        }
    }

    void DrawFilledTriangles(int startX, int startY, float scalingFactor, VertexPoint* vertexArray, unsigned int numberOfVertexes, SGE::Display::Video::pixel color)
    {
        //
        //  Create a temporary array to store scaled vertexes and leave the original ones alone
        //
        auto* tempScaledVertexes = new VertexPoint[numberOfVertexes];

        //
        //  Figure out how many full triangle we have
        //
        int numberOfFullTriangles = (int)numberOfVertexes / 3;
        int numberOfVertexesToUse = numberOfFullTriangles * 3;

        //
        //  Scale the vertexes and shift them over in reference to the start point
        //
        for (int i = 0; i < (int)numberOfVertexes; i++)
        {
            tempScaledVertexes[i].x = (int)((float)vertexArray[i].x * scalingFactor);
            tempScaledVertexes[i].y = (int)((float)vertexArray[i].y * scalingFactor);
        }


        //
        //  Draw the vertexes
        //
        for (int i = 0; i < numberOfVertexesToUse; i = i + 3)
        {
            DrawFilledTriangleFast(startX, startY, tempScaledVertexes[i], tempScaledVertexes[i + 1], tempScaledVertexes[i + 2], color);
        }

        //
        //  Delete Temp Vertex Array
        //
        delete[] tempScaledVertexes;
    }

    //Uses a variant of the Bresenham algorithm to calculate the two X points along the X-axis for each Y
    //Then mass memcpy from a created pixel buffer to fill in the gaps.
    void DrawFilledTriangleFast(int startX, int startY, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, SGE::Display::Video::pixel color)
    {
        //
        //  Offset the vertexes
        //
        vertex1.x += startX;
        vertex2.x += startX;
        vertex3.x += startX;

        vertex1.y += startY;
        vertex2.y += startY;
        vertex3.y += startY;

        //
        //  Pixel Buffer to mass copy memory from
        //
        int targetPixelBufferSize;

        //
        //  Row buffer to store and copy pixel data from
        //
        static SGE::Display::Video::pixel* rowBuffer = nullptr;

        //
        //  Row buffer size to compare in the future to expand the row buffer as needed
        //
        static int rowBufferSize = 0;

        //
        //  Variables to hold the top most, bottom most, and middle point vertexes
        //
        VertexPoint topVertex;
        VertexPoint bottomVertex;
        VertexPoint middleVertex;

        //
        //  Floats to hold deltas for the three line segments
        //

        float topToBottomLineDelta;
        float topToMiddleLineDelta;
        float middleToBottomLineDelta;

        //
        //  Sanity Checks for drawing
        //

        if ((vertex1.y < 0 && vertex2.y < 0 && vertex3.y < 0) ||		//Do all these vertexes have Y's in the negatives?
            (vertex1.x < 0 && vertex2.x < 0 && vertex3.x < 0) ||		//Do all these vertexes have X's in the negatives?
            (vertex1.y >= SGE::Display::Video::Y && vertex2.y >= SGE::Display::Video::Y && vertex3.y >= SGE::Display::Video::Y) ||	//Do all these vertexes have Y's greater than our Y resolution?
            (vertex1.x >= SGE::Display::Video::Y && vertex2.x >= SGE::Display::Video::Y && vertex3.x >= SGE::Display::Video::X))	//Do all these vertexes have X's greater than our X resolution?
        {
            //We can't draw this!
            //No part of it will ever exist within the screen
            //Eject!
            return;
        }


        //
        //Calculate the ideal orientation of the vertexes for drawing from lowest Y down the screen.
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

                //By elimination Vertex 1 is the middle vertex
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
        //For our purposes of logic, a zero will work just as well...  Sorry math majors.
        if (std::isinf(topToBottomLineDelta))		{ topToBottomLineDelta = 0.0f; }
        if (std::isinf(topToMiddleLineDelta))		{ topToMiddleLineDelta = 0.0f; }
        if (std::isinf(middleToBottomLineDelta))	{ middleToBottomLineDelta = 0.0f; }

        //
        //  Setup pixel buffers
        //

        //Determine greatest distance between triangle points
        int lowestX = vertex1.x;
        int greatestX = vertex1.x;

        //Lowest checks
        if (vertex2.x < lowestX) { lowestX = vertex2.x; }
        if (vertex3.x < lowestX) { lowestX = vertex3.x; }

        //Greatest checks
        if (vertex2.x > greatestX) { (greatestX = vertex2.x); }
        if (vertex3.x > greatestX) { (greatestX = vertex3.x); }

        targetPixelBufferSize = greatestX - lowestX;

        //
        //  Create a row buffer if it hasn't already
        //
        if (rowBuffer == nullptr)
        {
            rowBuffer = new SGE::Display::Video::pixel[targetPixelBufferSize];
            rowBufferSize = targetPixelBufferSize;
        }
        else if (rowBufferSize < targetPixelBufferSize)
        {
            delete[] rowBuffer;
            rowBuffer = new SGE::Display::Video::pixel[targetPixelBufferSize];
            rowBufferSize = targetPixelBufferSize;
        }


        //
        //  Load up the Row Buffer
        //
        for (int i = 0; i < targetPixelBufferSize && i < SGE::Display::Video::X; i++)
        {
            rowBuffer[i] = color;
        }

        //
        // Start drawing the line points pairs
        //

        //Top-To-Middle Line and Top-To-Bottom Line pairing
        for (int currentScreenY = (topVertex.y < 0) ? 0 : topVertex.y;									//Start screenY at topVertex Y, or 0 if topVertex Y is negative
            currentScreenY <= middleVertex.y && currentScreenY < SGE::Display::Video::Y;		//Keep drawing to the middleVertex Y or until the end of the viewable screen
            currentScreenY++)
        {
            //
            //  Figure out Top-To-Middle line point
            //

            int currentTopToMiddleX = topVertex.x + (int)((float)(currentScreenY - topVertex.y) * topToMiddleLineDelta);

            //Check for any out of bounds issues and correct
            if (currentTopToMiddleX < 0) { currentTopToMiddleX = 0; }																		//Check to see if currentTopToBottomX is negative
            if (currentTopToMiddleX >= SGE::Display::Video::X) { currentTopToMiddleX = SGE::Display::Video::X - 1; }	//Check to see if currentTopToBottomX is beyond the resolution

            //
            //  Figure out Top-To-Bottom line point
            //

            int currentTopToBottomX = topVertex.x + (int)((float)(currentScreenY - topVertex.y) * topToBottomLineDelta);

            //Check for any out of bounds issues and correct
            if (currentTopToBottomX < 0) { currentTopToBottomX = 0; }																		//Check to see if currentTopToBottomX is negative
            if (currentTopToBottomX >= SGE::Display::Video::X) { currentTopToBottomX = SGE::Display::Video::X - 1; }	//Check to see if currentTopToBottomX is beyond the resolution

            //
            //  Figure out dimensions for memory copy operations
            //

            //What offset are we moving to
            int copyRowDestination = (currentScreenY * SGE::Display::Video::X) +					//Starting a the memory location for the current row (Y) we are on
                ((currentTopToBottomX < currentTopToMiddleX) ? currentTopToBottomX : currentTopToMiddleX);		//Offset by the smallest X

            //What offset are we moving from
            //int copyRowSource = (int) (currentTopToBottomX < currentTopToMiddleX) ? (currentScreenY * topToBottomLineDelta) : (currentScreenY * topToMiddleLineDelta);		//Offset by the smallest X delta
            int copyRowSource = 0;

            //And how much
            int copyRowLength = currentTopToBottomX - currentTopToMiddleX;

            //Logic for sign flip
            if (copyRowLength < 0) { (copyRowLength = -copyRowLength); }

            //Add a little extra
            copyRowLength++;

            //
            //  Mass move data
            //
            std::copy(&rowBuffer[copyRowSource], &rowBuffer[copyRowSource + copyRowLength], &SGE::Display::Video::RAM[copyRowDestination]);
        }

        //Middle-To-Bottom and Top-To-Bottom Line pairing
        for (int currentScreenY = middleVertex.y < 0 ? 0 : middleVertex.y;									//Start screenY at middleVertex Y, or 0 if middleVertex Y is negative
            currentScreenY <= bottomVertex.y && currentScreenY < SGE::Display::Video::Y;			//Keep drawing to the bottomVertex Y or until the end of the viewable screen
            currentScreenY++)
        {
            //
            //  Figure out Top-To-Middle line point
            //

            int currentMiddleToBottom = middleVertex.x + (int)((float)(currentScreenY - middleVertex.y) * middleToBottomLineDelta);

            //Check for any out of bounds issues and correct
            if (currentMiddleToBottom < 0) { currentMiddleToBottom = 0; }																		//Check to see if currentTopToBottomX is negative
            if (currentMiddleToBottom >= SGE::Display::Video::X) { currentMiddleToBottom = SGE::Display::Video::X - 1; }	//Check to see if currentTopToBottomX is beyond the resolution

            //
            //  Figure out Top-To-Bottom line point
            //

            int currentTopToBottomX = topVertex.x + (int)((float)(currentScreenY - topVertex.y) * topToBottomLineDelta);

            //Check for any out of bounds issues and correct
            if (currentTopToBottomX < 0) { currentTopToBottomX = 0; }																			//Check to see if currentTopToBottomX is negative
            if (currentTopToBottomX >= SGE::Display::Video::X) { currentTopToBottomX = SGE::Display::Video::X - 1; }		//Check to see if currentTopToBottomX is beyond the resolution

            //
            //  Figure out dimensions for memory copy operations
            //

            //What offset are we moving to
            int copyRowDestination = currentScreenY * SGE::Display::Video::X +						//Starting a the memory location for the current row (Y) we are on
                ((currentTopToBottomX < currentMiddleToBottom) ? currentTopToBottomX : currentMiddleToBottom);		//Offset by the smallest X

                                                                                                            //What offset are we moving from
            //int copyRowSource = (int)(currentTopToBottomX < currentMiddleToBottom) ? (currentScreenY * topToBottomLineDelta) : (currentScreenY * topToMiddleLineDelta);		//Offset by the smallest X delta
            int copyRowSource = 0;

                                                                                                                                                                            //And how much
            int copyRowLength = currentTopToBottomX - currentMiddleToBottom;

            //Logic for sign flip
            if (copyRowLength < 0) { (copyRowLength = -copyRowLength); }

            copyRowLength++;

            //
            //  Mass move data
            //
            std::copy(rowBuffer + copyRowSource, rowBuffer + copyRowSource + copyRowLength, &SGE::Display::Video::RAM[copyRowDestination]);
        }
    }


    //Uses the Barycentric Algorithm to calculate what pixels are within a defined triangle
    //Accurate and true.... but expensive as can be computationally
    //Useful for double checking the Fast Triangle draw function.
    void DrawFilledTriangleTrue(VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, SGE::Display::Video::pixel color)
    {
        //Set the initial maximums and minimums to the first vertex
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

        //Partial cross products geared towards the Y element of the looping
        int partialProductY12 = spanningVector12.x * (yMin - vertex1.y);
        int partialProductY23 = spanningVector23.x * (yMin - vertex2.y);
        int partialProductY31 = spanningVector31.x * (yMin - vertex3.y);

        //Partial cross products geared towards the X element of the looping
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
                    SGE::Display::Video::RAM[x + (y*SGE::Display::Video::X)] = color;
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
}

