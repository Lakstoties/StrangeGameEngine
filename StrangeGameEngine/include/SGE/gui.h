#pragma once
#include "render.h"

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace GUI
	{
		class TextBox
		{

		public:
			//  The number of rows in the text box
			unsigned int rows = 0;

			//  The number of columns in the text box
			unsigned int columns = 0;

			//  The spacing between characters between columns
			unsigned int ColumnSpacing = 8;

			//  The spacing between character between rows
			unsigned int RowSpacing = 8;

			//Buffers to store characters
			char* CharacterArray = NULL;

			//Color Array for Foreground
			SGE::Display::Video::pixel* ForegroundColorArray = NULL;
			
			//Color Array for Background
			SGE::Display::Video::pixel* BackgroundColorArray = NULL;

			//Upper Left corner to align to
			int XPosition = 0;
			int YPosition = 0;

			TextBox(unsigned int numberOfRows, unsigned int numberOfColumns, int xPosition, int yPosition);
			~TextBox();

			void Draw();
			void DeleteBuffers();
			void CreateBuffers(unsigned int newRows, unsigned int newColumns);
			void ResetBuffers();
		};
	}

	class Menu
	{
	private:
		int menuX;
		int menuY;
		int menuWidth;
		int menuHeight;
		int margin;
		int itemHeight;
		int textBoxMargin;
		char **menuItemText;
		unsigned char numberOfSelections;

		//Derived bits that once need to be calculated once or so...
		int menuXMargin;
		int menuYMargin;

		//Row Text Cursor
		unsigned int rowTextCursorLocation;
		bool cursorActive;

		void CheckCursorPosition();

	public:

		unsigned char selection = 0;


		//Background Color
		SGE::Display::Video::pixel backgroundColor;

		//Border Color
		SGE::Display::Video::pixel borderColor;

		//Highlight Color
		SGE::Display::Video::pixel highlightColor;

		//Regular Text Color
		SGE::Display::Video::pixel regularColor;

		//Highlight Text Color
		SGE::Display::Video::pixel highlightTextColor;

		Menu(int targetMenuX, int targetMenuY, int targetMenuWidth, int targetMenuHeight, int targetMargin, int targetItemHeight, int targetTextBoxMargin, int targetNumberOfSelections, char** targetMenuItems);

		void Draw();

		void NextSelection();

		void PreviousSelection();

		char* GetMenuSelection();

		void SetMenuSelection(char* targetString);

		void CursorOn();

		void CursorOff();

		void MoveCursor(int location);

		int GetCursorLocation();
	};
}