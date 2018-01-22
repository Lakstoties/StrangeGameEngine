#include "include\SGE\gui.h"
#include <cstring>

namespace SGE
{
	namespace GUI
	{
		void TextBox::Draw()
		{
			//Start and end points for rows
			unsigned int startRow = 0;
			unsigned int endRow = rows;

			//Start and end points for columns
			unsigned int startColumn = 0;
			unsigned int endColumn = columns;


			//
			//Trim starting points based on upper left corner position
			//

			//Figure out how many columns are clipped out completely
			if (XPosition < 0)
			{
				//Flip the sign
				
				//Divide by 8 to get the number of whole columns cut

				//Add to the startRow
			}

			//Figure out how many rows are clipped out completely
			if (YPosition < 0)
			{
				//Flip the sign
				
				//Divide by 8

			}

			//Calculate VideoRAM starting point
			unsigned int videoRAMPosition;

			//Draw the background
			for (int i = startRow; i < endRow; i++)
			{
				for (int j = startColumn; j < endColumn; j++)
				{
					//If there's something other than pure black in the background
					if (BackgroundColorArray[i*j] != 0)
					{

					}
				}
			}

			//Draw the characters
		}

		//Reset the buffers used to store characters.
		void TextBox::ResetBuffers(unsigned int newRows, unsigned int newColumns)
		{
			//Check for any invalid row and colums sizes
			if (newRows == 0 || newColumns == 0)
			{
				return;
			}

			//
			//Delete any old stuff
			//

			//Characters
			//Delete the buffer array
			delete CharacterRowBuffers;

			//Foreground Color
			//Delete the buffer array
			delete ForegroundColorArray;

			//Background Color
			//Delete the buffer array
			delete BackgroundColorArray;

			//
			//Assign new rows and colums sizes
			//

			columns = newColumns;
			rows = newRows;

			//
			//Create the new stuff
			//
			
			//
			//Characters
			//
			//Create the buffer array
			CharacterRowBuffers = new char[rows * columns];

			//Zero out the array
			std::memset(CharacterRowBuffers, 0, rows * columns);

			//
			//Foreground Color
			//
			//Create the buffer array
			ForegroundColorArray = new unsigned int[rows * columns];

			//Zero out the array
			std::memset(ForegroundColorArray, 0, rows * columns);

			//
			//Background Color
			//
			//Create the buffer array
			BackgroundColorArray = new unsigned int[rows * columns];

			//Zero out the array
			std::memset(BackgroundColorArray, 0, rows * columns);

		}
	}

	Menu::Menu(int targetMenuX, int targetMenuY, int targetMenuWidth, int targetMenuHeight, int targetMargin, int targetItemHeight, int targetTextBoxMargin, int targetNumberOfSelections, char** targetMenuItems)
	{
		//Assign all the vales
		menuX = targetMenuX;
		menuY = targetMenuY;
		menuHeight = targetMenuHeight;
		menuWidth = targetMenuWidth;
		margin = targetMargin;
		itemHeight = targetItemHeight;
		textBoxMargin = targetTextBoxMargin;
		numberOfSelections = targetNumberOfSelections;

		//Grab the values from the char arrays
		menuItemText = new char*[numberOfSelections];

		size_t tempCount = 0;

		for (int i = 0; i < numberOfSelections; i++)
		{
			//Grab the target menu item's length
			tempCount = strlen(targetMenuItems[i]) + 1;

			//Create a place to put the new item
			menuItemText[i] = new char[tempCount];

			//Copy it over
			strcpy(menuItemText[i], targetMenuItems[i]);
		}

		//Derived bits that once need to be calculated once or so...
		menuXMargin = menuX + margin;
		menuYMargin = menuY + margin;

		//Set up the cursor info

		cursorActive = false;
		rowTextCursorLocation = 0;
	}


	void Menu::Draw()
	{
		//Draw Menu Back
		SGE::Render::DrawBox(menuX, menuY, menuWidth, menuHeight, backgroundColor);

		//Draw Menu Border
		SGE::Render::DrawRectangle(menuX, menuY, menuWidth, menuHeight, borderColor);

		//Write Text
		for (int i = 0; i < numberOfSelections; i++)
		{

			//If it's just normal text
			if (i != selection)
			{
				SGE::Render::DrawString(menuItemText[i], SGE::Render::CHARACTER_8x8_ROM, 8, menuXMargin + textBoxMargin, menuYMargin + (itemHeight * i) + textBoxMargin, regularColor);
			}

			//Else it's the current selection text
			else
			{
				//Draw the selected text box
				SGE::Render::DrawBox(menuXMargin, menuYMargin + (itemHeight * i), menuWidth - (margin << 1), itemHeight, highlightColor);

				//Draw the selected characters
				SGE::Render::DrawString(menuItemText[i], SGE::Render::CHARACTER_8x8_ROM, 8, menuXMargin + textBoxMargin, menuYMargin + (itemHeight * i) + textBoxMargin, highlightTextColor);

				//If the cursor is active draw it
				if (cursorActive)
				{
					//Draw the text box for the cursor
					SGE::Render::DrawBox(menuXMargin + textBoxMargin + 8 * rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, 8, 8, highlightTextColor);
					//Draw the selected character
					SGE::Render::Draw8x8Character(menuItemText[i][rowTextCursorLocation], SGE::Render::CHARACTER_8x8_ROM, menuXMargin + textBoxMargin + 8 * rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, highlightColor);
				}
			}
		}
	}


	void Menu::NextSelection()
	{
		selection = (selection + 1) % numberOfSelections;

		CheckCursorPosition();
	}

	void Menu::PreviousSelection()
	{
		selection = (selection - 1) % numberOfSelections;

		if (selection == 0xFF)
			selection = (numberOfSelections - 1);

		CheckCursorPosition();
	}

	//This function checks the cursor's position and makes sure it make sense for the current row text selection
	//If it's farther out than the text, it'll move it to the end of the text.
	void Menu::CheckCursorPosition()
	{
		size_t temp = 0;

		//Adjust cursor location
		//Check the length of the next selection
		temp = strlen(menuItemText[selection]);


		if (rowTextCursorLocation >= temp)
		{
			rowTextCursorLocation = (unsigned int) temp - 1;
		}
	}



	char* Menu::GetMenuSelection()
	{
		size_t tempCount = 0;
		char* tempString;

		if (menuItemText == nullptr)
		{
			return nullptr;
		}

		if (menuItemText[selection] == nullptr)
		{
			return nullptr;
		}

		//Get the length of the string with terminating null
		tempCount = strlen(menuItemText[selection]) + 1;

		//Create a place to put the string
		tempString = new char[tempCount];

		//Copy it over
		strcpy(tempString, menuItemText[selection]);

		return tempString;
	}


	void Menu::SetMenuSelection(char* targetString)
	{
		size_t tempCount = 0;

		if (targetString == nullptr)
		{
			return;
		}

		//Get the length of the new string with terminating null
		tempCount = strlen(targetString) + 1;

		//Clear out old material
		delete menuItemText[selection];

		//Create a new place to put the new string
		menuItemText[selection] = new char[tempCount];

		//Copy the string over
		strcpy(menuItemText[selection], targetString);
	}


	void Menu::CursorOn()
	{
		cursorActive = true;
	}

	void Menu::CursorOff()
	{
		cursorActive = false;
	}

	void Menu::MoveCursor(int location)
	{
		if (location < 0)
		{
			rowTextCursorLocation = 0;
		}
		else
		{
			rowTextCursorLocation = location;
		}

		CheckCursorPosition();
	}

	int Menu::GetCursorLocation()
	{
		return rowTextCursorLocation;
	}
}


