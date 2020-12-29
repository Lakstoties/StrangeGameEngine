#pragma once
#include "api.h"
#include "render.h"
#include <vector>

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
			//
			//  Nice little struct to hold all the relevant data in a sensible fashion
			//
			class TextBoxData
			{
			public:
				char Character = 0;
				SGE::Display::Video::pixel ForeColor = 0;
				SGE::Display::Video::pixel BackColor = 0;
			};

			const unsigned long long* CurrentCharacterROM = nullptr;

			enum class CellSize
			{
				Mode8x8,
				Mode8x16
			};
			
			CellSize CurrentCellSize = CellSize::Mode8x8;

		private:

			//
			//  2D Vector of TextBoxData to hold all the bits.
			//
			std::vector<TextBoxData> Characters;

		public:

			//  The number of rows in the text box
			unsigned int rows = 0;

			//  The number of columns in the text box
			unsigned int columns = 0;

			//Upper Left corner to align to
			int XPosition = 0;
			int YPosition = 0;

			TextBox(unsigned int numberOfRows, unsigned int numberOfColumns, int xPosition, int yPosition);

			~TextBox();

			void Draw();
			void CreateBuffers(unsigned int newRows, unsigned int newColumns);

			//  Overload Parenthesis Operator for easy access

			TextBoxData& operator ()(int row, int column);
			TextBoxData operator ()(int row, int column) const;
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
		std::vector<std::string> menuItemText;
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
		SGE::Display::Video::pixel backgroundColor = 0;

		//Border Color
		SGE::Display::Video::pixel borderColor = 0;

		//Highlight Color
		SGE::Display::Video::pixel highlightColor = 0;

		//Regular Text Color
		SGE::Display::Video::pixel regularColor = 0;

		//Highlight Text Color
		SGE::Display::Video::pixel highlightTextColor = 0;

        Menu(int targetMenuX, int targetMenuY, int targetMenuWidth, int targetMenuHeight, int targetMargin, int targetItemHeight, int targetTextBoxMargin, const std::vector<std::string>& targetMenuItems);

		void Draw();

		void NextSelection();

		void PreviousSelection();

		std::string GetMenuSelection();

		void SetMenuSelection(const std::string& targetString);

		void CursorOn();

		void CursorOff();

		void MoveCursor(int location);

		int GetCursorLocation() const;
	};
}