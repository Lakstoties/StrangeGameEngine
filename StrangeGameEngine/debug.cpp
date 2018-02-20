#include "include\SGE\debug.h"
#include "include\SGE\render.h"
#include "include\SGE\controls.h"

namespace SGE
{
	namespace Debug
	{
		//
		//  Debug function that draws a box with the status elements for the mouse
		//
		void DrawMouseStatusBox(int xCornerPosition, int yCornerPosition)
		{
			char temp[100] = { 0 };
			//
			//Draw Background Box
			//
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 145, 58, SGE::Render::Colors::ColorMode8Bit[2]);
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 145, 58, SGE::Render::Colors::ColorMode8Bit[22]);

			//
			//Position Labels
			//

			//Print string for Mouse's X
			sprintf(temp, "Mouse Position X: %i", SGE::Controls::Mouse::PositionX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 05, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y
			sprintf(temp, "Mouse Position Y: %i", SGE::Controls::Mouse::PositionY);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 15, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print String for Mouse's X Scroll
			sprintf(temp, "Scroll Position X: %i", SGE::Controls::Mouse::ScrollX);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 25, SGE::Render::Colors::ColorMode8Bit[157]);

			//Print string for Mouse's Y Scroll
			sprintf(temp, "Scroll Position Y: %i", SGE::Controls::Mouse::ScrollY);
			SGE::Render::DrawString(temp, SGE::Render::CHARACTER_8x8_ROM, 6, xCornerPosition + 4, yCornerPosition + 35, SGE::Render::Colors::ColorMode8Bit[157]);

			//
			//
			//Print Mouse button array
			//
			//

			//
			//Left Mouse Button
			//

			//Draw Label
			SGE::Render::Draw8x8Character('L', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 5, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 15,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[0] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Right Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character('R', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 25, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 35,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[1] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Center Mouse Button
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character('M', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 45, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 55,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[2] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #1
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character('A', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 65, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 75,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[3] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)

																																		//
																																		//Extra Button #2
																																		//

																																		//Draw Label
			SGE::Render::Draw8x8Character('B', SGE::Render::CHARACTER_8x8_ROM, xCornerPosition + 85, yCornerPosition + 45, SGE::Render::Colors::ColorMode8Bit[157]);

			//Draw Light
			SGE::Render::DrawBox(
				xCornerPosition + 95,							//X Upper Left Corner Position
				yCornerPosition + 45,							//Y Upper Left Corner Position
				5,												//Width
				8,												//Height
				SGE::Controls::Mouse::Buttons[4] ? SGE::Render::Colors::ColorMode8Bit[160] : SGE::Render::Colors::ColorMode8Bit[52]);	//Color (Changes if active or not)
		}




		//
		//  Debug function that draws the classic 4 bit palette in a box for testing purposes.
		//
		void DrawColorPalette4bit(int xCornerPosition, int yCornerPosition)
		{
			const int width = 330;
			const int height = 30;


			//Draw Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::BrightBlack);

			//Draw Border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::White);

			//Draw Swatches
			for (int i = 0; i < 16; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 5 + i * 20, yCornerPosition + 5, 20, 20, SGE::Render::Colors::ColorMode4Bit[i]);
			}
		}

		//
		// Debug funciton that draws classic 8 bit palette in a box for testing purposes.
		//
		void DrawColorPalette8bit(int xCornerPosition, int yCornerPosition)
		{
			const int width = 513;
			const int height = 125;


			//Draw Background
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::BrightBlack);

			//Draw Border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, width, height, SGE::Render::Colors::Named::White);

			//Draw Standard Colors
			for (int i = 0; i < 8; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 05 + i * 31, yCornerPosition + 5, 30, 10, SGE::Render::Colors::ColorMode8Bit[i]);
			}

			//Draw High-intesity Colors
			for (int i = 8; i < 16; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 13 + i * 31, yCornerPosition + 5, 30, 10, SGE::Render::Colors::ColorMode8Bit[i]);
			}

			//Draw the 216 Colors
			//These are arranages a 6 x 36 grid
			for (int i = 0; i < 6; i++)
			{
				for (int j = 0; j < 36; j++)
				{
					//Draw Color Block
					SGE::Render::DrawBox(xCornerPosition + 05 + j * 14, yCornerPosition + 20 + i * 14, 13, 13, SGE::Render::Colors::ColorMode8Bit[(i * 36) + 16 + j]);
				}
			}

			//Draw the Grayscale color
			for (int i = 0; i < 24; i++)
			{
				SGE::Render::DrawBox(xCornerPosition + 05 + i * 21, yCornerPosition + 110, 20, 10, SGE::Render::Colors::ColorMode8Bit[232 + i]);
			}

		}

		//
		//  Helper function to draw keyboard keys for the Keyboard Status function
		//
		void DrawKeyboardKey(int xCornerPosition, int yCornerPosition, char* label, const int key)
		{
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 19, 19, SGE::Controls::Keyboard::Status[key] ? SGE::Render::Colors::Named::White : SGE::Render::Colors::Named::Black);
			SGE::Render::DrawString(label, SGE::Render::CHARACTER_8x8_ROM, 5, xCornerPosition + 1, yCornerPosition + 5, SGE::Controls::Keyboard::Status[key] ? SGE::Render::Colors::Named::Black : SGE::Render::Colors::Named::BrightWhite);
		}

		//
		//  Debug function that draws a keyboard to indicate the current status of all keyboard keys
		//
		void DrawKeyboardStatus(int xCornerPosition, int yCornerPosition)
		{
			//Draw background box
			SGE::Render::DrawBox(xCornerPosition, yCornerPosition, 500, 150, SGE::Render::Colors::Named::BrightBlack);

			//Draw background border
			SGE::Render::DrawRectangle(xCornerPosition, yCornerPosition, 500, 150, SGE::Render::Colors::Named::White);

			//
			//  Draw keys
			//

			//
			//  First Row - Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,   PntScr, ScrLck,  Pause
			//

			//Escape
			DrawKeyboardKey(xCornerPosition + 5, yCornerPosition + 5, (char *)"Esc", SGE::Controls::Keymap::KEY_ESCAPE);


			//F1
			DrawKeyboardKey(xCornerPosition + 30, yCornerPosition + 5, (char *)"F1", SGE::Controls::Keymap::KEY_F1);

			//F2
			DrawKeyboardKey(xCornerPosition + 50, yCornerPosition + 5, (char *)"F2", SGE::Controls::Keymap::KEY_F2);

			//F3
			DrawKeyboardKey(xCornerPosition + 70, yCornerPosition + 5, (char *)"F3", SGE::Controls::Keymap::KEY_F3);

			//F4
			DrawKeyboardKey(xCornerPosition + 90, yCornerPosition + 5, (char *)"F4", SGE::Controls::Keymap::KEY_F4);


			//F5
			DrawKeyboardKey(xCornerPosition + 115, yCornerPosition + 5, (char *)"F5", SGE::Controls::Keymap::KEY_F5);

			//F6
			DrawKeyboardKey(xCornerPosition + 135, yCornerPosition + 5, (char *)"F6", SGE::Controls::Keymap::KEY_F6);

			//F7
			DrawKeyboardKey(xCornerPosition + 155, yCornerPosition + 5, (char *)"F7", SGE::Controls::Keymap::KEY_F7);

			//F8
			DrawKeyboardKey(xCornerPosition + 175, yCornerPosition + 5, (char *)"F8", SGE::Controls::Keymap::KEY_F8);


			//F9
			DrawKeyboardKey(xCornerPosition + 200, yCornerPosition + 5, (char *)"F9", SGE::Controls::Keymap::KEY_F9);

			//F10
			DrawKeyboardKey(xCornerPosition + 220, yCornerPosition + 5, (char *)"F10", SGE::Controls::Keymap::KEY_F10);

			//F11
			DrawKeyboardKey(xCornerPosition + 240, yCornerPosition + 5, (char *)"F11", SGE::Controls::Keymap::KEY_F11);

			//F12
			DrawKeyboardKey(xCornerPosition + 260, yCornerPosition + 5, (char *)"F12", SGE::Controls::Keymap::KEY_F12);


			//
			//  Second Row - `, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -, =, BckSpc, Ins, Hom, PgUp, NumLck, /, *, -
			//

			// `

			//1

			//2

			//3

			//4

			//5

			//6

			//7

			//8

			//9

			//0

			//-

			//=

			//BkS  (Backspace)

			//Ins

			//Hom

			//PgU

			//NmL

			// /

			//*

			//Nm-

			//
			//  Third Row
			//
		}

	}
}