#include "include\SGE\gui.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace GUI
	{
		TextBox::TextBoxData& TextBox::operator ()(int row, int column)
		{
			return Characters[(row * columns) + column];
		}

		TextBox::TextBoxData TextBox::operator ()(int row, int column) const
		{
			return Characters[(row * columns) + column];
		}


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
			if (CurrentCellSize == CellSize::Mode8x8)
			{
				//Draw the characters
				for (int i = (int)startRow; i < (int)endRow; i++)
				{
					for (int j = (int)startColumn; j < (int)endColumn; j++)
					{
						//Draw the characters upon the screen
						SGE::Render::Draw8x8CharacterFilled(&CurrentCharacterROM[(unsigned char)Characters[(i * columns) + j].Character], XPosition + j * 8, YPosition + i * 8, Characters[(i * columns) + j].ForeColor, Characters[(i * columns) + j].BackColor);
					}
				}
			}
			else if (CurrentCellSize == CellSize::Mode8x16)
			{
				//Draw the characters
				for (int i = (int)startRow; i < (int)endRow; i++)
				{
					for (int j = (int)startColumn; j < (int)endColumn; j++)
					{
						//Draw the characters upon the screen
						SGE::Render::Draw8x16CharacterFilled(&CurrentCharacterROM[(unsigned char)Characters[(i * columns) + j].Character * 2], XPosition + j * 8, YPosition + i * 16, Characters[(i * columns) + j].ForeColor, Characters[(i * columns) + j].BackColor);
					}
				}
			}
		}

		//
		//  Create new buffers based on new rows and column amounts
		//
		void TextBox::CreateBuffers(unsigned int newRows, unsigned int newColumns)
		{
			//
			//  Check for any invalid row and columns sizes
			//
			if (newRows == 0 || newColumns == 0)
			{
				return;
			}

			//
			//  Assign new rows and columns sizes
			//

			columns = newColumns;
			rows = newRows;

			//
			//  Create new buffer
			//
			Characters.clear();
			Characters.resize(newRows * newColumns);
		}

		//
		//  Default constructor
		//
		TextBox::TextBox(unsigned int numberOfRows, unsigned int numberOfColumns, int xPosition, int yPosition)
		{
			//Set Default Character room
			CurrentCharacterROM = SGE::Render::CHARACTER_8x8_ROM;

			//  Create the buffers
			CreateBuffers(numberOfRows, numberOfColumns);

			//  Set the positions
			XPosition = xPosition;
			YPosition = yPosition;
		}

		TextBox::~TextBox() = default;
	}

	Menu::Menu(int targetMenuX, int targetMenuY, int targetMenuWidth, int targetMenuHeight, int targetMargin, int targetItemHeight, int targetTextBoxMargin, const std::vector<std::string>& targetMenuItems)
	{
		//Assign all the vales
		menuX = targetMenuX;
		menuY = targetMenuY;
		menuHeight = targetMenuHeight;
		menuWidth = targetMenuWidth;
		margin = targetMargin;
		itemHeight = targetItemHeight;
		textBoxMargin = targetTextBoxMargin;
		numberOfSelections = targetMenuItems.size();

		//Grab the values from the char arrays
		menuItemText = targetMenuItems;

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
				SGE::Render::DrawBox(menuXMargin, menuYMargin + (itemHeight * i), menuWidth - (margin * 2), itemHeight, highlightColor);

				//Draw the selected characters
				SGE::Render::DrawString(menuItemText[i], SGE::Render::CHARACTER_8x8_ROM, 8, menuXMargin + textBoxMargin, menuYMargin + (itemHeight * i) + textBoxMargin, highlightTextColor);

				//If the cursor is active draw it
				if (cursorActive)
				{
					//Draw the text box for the cursor
					SGE::Render::DrawBox(menuXMargin + textBoxMargin + 8 * (int)rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, 8, 8, highlightTextColor);
					//Draw the selected character
					SGE::Render::Draw8x8Character(&SGE::Render::CHARACTER_8x8_ROM[(unsigned char)menuItemText[i][rowTextCursorLocation]], menuXMargin + textBoxMargin + 8 * (int)rowTextCursorLocation, menuYMargin + (itemHeight*i) + textBoxMargin, highlightColor);
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
		//Adjust cursor location
		//Check the length of the next selection

		if (rowTextCursorLocation >= menuItemText[selection].length())
		{
			rowTextCursorLocation = (unsigned int) menuItemText[selection].length() - 1;
		}
	}



	std::string Menu::GetMenuSelection()
	{
		if (menuItemText.empty())
		{
			return "";
		}

		if (menuItemText[selection].empty())
		{
			return "";
		}

		return menuItemText[selection];
	}


	void Menu::SetMenuSelection(const std::string& targetString)
	{
		if (targetString.empty())
		{
			return;
		}

		menuItemText[selection] = targetString;
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

	int Menu::GetCursorLocation() const
	{
		return (int)rowTextCursorLocation;
	}
}


