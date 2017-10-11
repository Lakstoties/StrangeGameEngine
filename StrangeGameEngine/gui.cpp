#include "include\SGE\gui.h"
#include <cstring>

namespace SGE
{

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
		SGE::Render::DrawBox(menuX, menuY, menuWidth, menuHeight, backgroundRColor, backgroundGColor, backgroundBColor);

		//Draw Menu Border
		SGE::Render::DrawRectangle(menuX, menuY, menuWidth, menuHeight, borderRColor, borderGColor, borderBColor);

		//Write Text
		for (int i = 0; i < numberOfSelections; i++)
		{

			//If it's just normal text
			if (i != selection)
			{
				SGE::Render::DrawString(menuItemText[i], SGE::Render::CHARACTER_8x8_ROM, 8, menuXMargin + textBoxMargin, menuYMargin + (itemHeight * i) + textBoxMargin, textRColor, textGColor, textBColor);
			}

			//Else it's the current selection text
			else
			{
				//Draw the selected text box
				SGE::Render::DrawBox(menuXMargin, menuYMargin + (itemHeight * i), menuWidth - (margin << 1), itemHeight, highlightRColor, highlightGColor, highlightBColor);

				//Draw the selected characters
				SGE::Render::DrawString(menuItemText[i], SGE::Render::CHARACTER_8x8_ROM, 8, menuXMargin + textBoxMargin, menuYMargin + (itemHeight * i) + textBoxMargin, highlightTextRColor, highlightTextGColor, highlightTextBColor);

				//If the cursor is active draw it
				if (cursorActive)
				{
					//Draw the text box for the cursor
					SGE::Render::DrawBox(menuXMargin + textBoxMargin + 8 * rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, 8, 8, highlightTextRColor, highlightTextGColor, highlightTextBColor);
					//Draw the selected character
					SGE::Render::Draw8x8Character(menuItemText[i][rowTextCursorLocation], SGE::Render::CHARACTER_8x8_ROM, menuXMargin + textBoxMargin + 8 * rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, highlightRColor, highlightGColor, highlightBColor);
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


