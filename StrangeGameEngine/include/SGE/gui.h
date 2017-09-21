#pragma once
#include "render.h"

namespace SGE
{
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
		unsigned char rowTextCursorLocation;
		bool cursorActive;

		void CheckCursorPosition();

	public:

		unsigned char selection = 0;


		//Background Color
		unsigned char backgroundRColor;
		unsigned char backgroundGColor;
		unsigned char backgroundBColor;

		//Border Color
		unsigned char borderRColor;
		unsigned char borderGColor;
		unsigned char borderBColor;

		//Highlight Color
		unsigned char highlightRColor;
		unsigned char highlightGColor;
		unsigned char highlightBColor;

		//Regular Text Color
		unsigned char textRColor;
		unsigned char textGColor;
		unsigned char textBColor;

		//Highlight Text Color
		unsigned char highlightTextRColor;
		unsigned char highlightTextGColor;
		unsigned char highlightTextBColor;

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