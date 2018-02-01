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
		private:
			//The number of rows in the text box
			unsigned int rows = 0;

			//The number of columns in the text box
			unsigned int columns = 0;

		public:

			//The spacing between characters between columns
			unsigned int ColumnSpacing = 0;

			//The spacing between character between rows
			unsigned int RowSpacing = 0;

			//Buffers to store characters
			char* CharacterRowBuffers = nullptr;

			//Color Array for Foreground
			SGE::Display::Video::pixel* ForegroundColorArray = nullptr;
			
			//Color Array for Background
			SGE::Display::Video::pixel* BackgroundColorArray = nullptr;

			//Upper Left corner to align to
			int XPosition = 0;
			int YPosition = 0;


			void Draw();
			void ResetBuffers(unsigned int newRows, unsigned int newColumns);
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