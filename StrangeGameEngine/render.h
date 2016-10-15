#pragma once
#include "display.h"
#include <fstream>

namespace SGE
{
	namespace Render
	{
		namespace FileFormatStructs
		{
			namespace Bitmap
			{
				//Bitmap File Header
				struct BMPFileHeader
				{
					char idField[2];		//ID Field,  should be "BM" for Bitmap.
					unsigned int bmpSize;	//Bitmap file size
					unsigned short reserved1;		//Reserved data that's application specific
					unsigned short reserved2;		//Reserved data that's application specific
					unsigned int offset;	//The offset/address where the bitmap image data can be found.
				};

				//Basic Bitmap info header... Mostly all the info we really need... hopefully.
				struct BMPInfoHeader
				{
					unsigned int sizeOfHeader;		//Size of the header, used to indicate version of the header used and extended information that may exist
					int bitmapWidth;				//Width of the bitmap in pixels
					int bitmapHeight;				//Height of the bitmap in pixels
					unsigned short colorPanes;		//Number of color panes
					unsigned short bitsPerPixel;	//Bits per pixel or color depth of image
					unsigned int compressionMethod;	//Compress method in use
					unsigned int imageSize;			//Size of the raw bitmap data
					int horizontalResolution;		//Print horizontal resolution
					int verticalResolution;			//Print vertical resolution
					unsigned int colorsInPalette;	//Number of colors in the color palette
					unsigned int importantColors;	//Number of important colors
				};
			}
		}

		//Struct to hold internal image data
		class ImageData
		{
		private:

		public:
			unsigned int height = 0;
			unsigned int width = 0;
			unsigned int imageDataSize = 0;
			unsigned int* imageData = nullptr;

			ImageData();
			~ImageData();
		};

		//Class that contains converted bitmap data from a file
		class RenderBitmapFile
		{
		private:
			FileFormatStructs::Bitmap::BMPFileHeader bitmapHeader;
			FileFormatStructs::Bitmap::BMPInfoHeader bitmapInfo;

		public:
			ImageData image;

			RenderBitmapFile();
			~RenderBitmapFile();

			int LoadFile(char* targetFilename);
		};

		//Class that contains image data pages and other functions to assist with animation
		class AnimationBook
		{
		private:
			//Number of pages in the book
			unsigned int numberOfPages = 0;

			//Data array for image data
			ImageData* pages = nullptr;

		public:
			AnimationBook();
			~AnimationBook();

			bool SetNumberOfPages(unsigned int targetNumberOfPages);
		};

		//Structure to handle simple 2D vertex points
		struct VertexPoint
		{
			int x;
			int y;
		};

		//Draw a character to the screen
		void Draw8x8Character(SGE::VirtualDisplay* targetDisplay, char character, const unsigned long long characterROM[], int targetX, int targetY, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a string of characters to the screen, from a null terminated string
		void DrawString(SGE::VirtualDisplay* targetDisplay, char* characters, const unsigned long long characterROM[], int characterSpacing, int targetX, int targetY, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a single pixel on the screen
		void DrawPixel(SGE::VirtualDisplay* targetDisplay, int targetX, int targetY, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a block of data from a source to target video ram
		void DrawDataBlock(SGE::VirtualDisplay* targetDisplay, int targetX, int targetY, int sourceWidth, int sourceHeight, unsigned int* sourceDataBlock);

		//Draw a line from one point to another
		void DrawLine(SGE::VirtualDisplay* targetDisplay, int startX, int startY, int endX, int endY, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a line row
		void DrawRow(SGE::VirtualDisplay* targetDisplay, int startX, int startY, int width, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a line column
		void DrawColumn(SGE::VirtualDisplay* targetDisplay, int startX, int startY, int height, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a rectangle
		void DrawRectangle(SGE::VirtualDisplay* targetDisplay, int startX, int startY, int width, int height, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a filled box
		void DrawBox(SGE::VirtualDisplay* targetDisplay, int startX, int startY, int width, int height, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Blank the video ram a particular color
		void Blank(SGE::VirtualDisplay* targetDisplay, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Blank the video ram with zero alpha black.  Or the blackest of blacks... it's like you can't get any blacker.
		void ZBlank(SGE::VirtualDisplay* targetDisplay);

		//Pack the byte colors in to a 4 byte pixel to use.
		unsigned int PackColors(unsigned char redValue, unsigned char greenValue, unsigned char blueValue);

		//Arbitrary Vector Shape drawing
		void DrawVectorShape(SGE::VirtualDisplay* targetDisplay, int startX, int startY, float scalingFactor, int numberOfVertexes, VertexPoint vertexes[], unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Draw a filled triangle
		void DrawFilledTriangle(SGE::VirtualDisplay* targetDisplay, int startX, int startY, float scalingFactor, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, unsigned char rColor, unsigned char gColor, unsigned char bColor);

		//Character ROM for an 8x8 character set
		//The 8x8 Character ROM that maps to the extendned ASCII standard
		//Each 64-bit unsigned is a 8x8 bit array
		//     0 1 2 3 4 5 6 7 MSB
		//
		// 0 - 0 0 0 0 0 0 0 0	= 0x00
		// 1 - 0 0 0 1 1 0 0 0	= 0x18
		// 2 - 0 0 1 0 0 1 0 0	= 0x24
		// 3 - 0 0 0 1 1 0 0 0  = 0x18
		// 4 - 0 0 1 0 0 1 0 0  = 0x24
		// 5 - 0 0 1 0 0 1 0 0  = 0x24
		// 6 - 0 0 0 1 1 0 0 0  = 0x18
		// 7 - 0 0 0 0 0 0 0 0  = 0x00
		// MSB
		const unsigned long long CHARACTER_8x8_ROM[] = {
			0x0000000000000000, 	//00 - NUL - CTRL-@
			0x0000000000000000, 	//01 - SOH - CTRL-A
			0x0000000000000000, 	//02 - STX - CTRL-B
			0x0000000000000000, 	//03 - ETX - CTRL-C
			0x0000000000000000, 	//04 - EQT - CTRL-D
			0x0000000000000000, 	//05 - ENQ - CTRL-E
			0x0000000000000000, 	//06 - ACK - CTRL-F
			0x0000000000000000,		//07 - BEL - CTRL-G
			0x0000000000000000, 	//08 - BS  - CTRL-H
			0x0000000000000000,		//09 - HT  - CTRL-I
			0x0000000000000000, 	//0A - LF  - CTRL-J
			0x0000000000000000, 	//0B - VT  - CTRL-K
			0x0000000000000000, 	//0C - FF  - CTRL-L
			0x0000000000000000, 	//0D - CR  - CTRL-M
			0x0000000000000000, 	//0E - SO  - CTRL-N
			0x0000000000000000,		//0F - SI  - CTRL-O
			0x0000000000000000, 	//10 - DLE - CTRL-P
			0x0000000000000000, 	//11 - DC1 - CTRL-Q
			0x0000000000000000, 	//12 - DC2 - CTRL-R
			0x0000000000000000,		//13 - DC3 - CTRL-S
			0x0000000000000000, 	//14 - DC4 - CTRL-T
			0x0000000000000000, 	//15 - NAK - CTRL-U
			0x0000000000000000, 	//16 - SYN - CTRL-V
			0x0000000000000000,		//17 - ETB - CTRL-W
			0x0000000000000000, 	//18 - CAN - CTRL-X
			0x0000000000000000, 	//19 - EM  - CTRL-Y
			0x0000000000000000, 	//1A - SUB - CTRL-Z
			0x0000000000000000, 	//1B - ESC - CTRL-[
			0x0000000000000000, 	//1C - FS  - CTRL-'\'
			0x0000000000000000,		//1D - GS  - CTRL-]
			0x0000000000000000, 	//1E - RS  - CTRL-^
			0x0000000000000000,		//1F - US  - CTRL-_
			0x0000000000000000,		//20 - ' ' - Blank Space
			0x0018001818181800,		//21 - !
			0x0000000000242400,		//22 - "
			0x00247E24247E2400,		//23 - #
			0x083C4A3C12523C10,		//24 - $
			0x0062640810264600,		//25 - %
			0x0058245418241800,		//26 - &
			0x0000000000101000,		//27 - '
			0x1008040404040810,		//28 - (
			0x0408101010100804,		//29 - )
			0x004A2C781E345200,		//2A - *
			0x0018187E7E181800,		//2B - +
			0x0C18180000000000,		//2C - ,
			0x0000007E7E000000,		//2D - -
			0x0018180000000000,		//2E - .
			0x0102040810204080,		//2F - /
			0x0018242424241800,		//30 - 0
			0x003C101014181000,     //31 - 1
			0x003C041820241800,     //32 - 2
			0x0018242010241800,		//33 - 3
			0x0020203C24242000,		//34 - 4
			0x001824201C043C00,		//35 - 5
			0x001824241C043800,		//36 - 6
			0x0004081020203C00,		//37 - 7
			0x0018242418241800,		//38 - 8
			0x001C203824241800,		//39 - 9
			0x0000181800181800,		//3A - :
			0x000C181800181800,		//3B - ;
			0x0030180C0C183000,		//3C - <
			0x00007E00007E0000,		//3D - =
			0x000C183030180C00,		//3E - >
			0x0018001860423C00,		//3F - ?
			0x003C027A5A423C00,		//40 - @
			0x0066667E663C1800,		//41 - A
			0x003E66663E663E00,		//42 - B
			0x003C460606463C00,		//43 - C
			0x003E666666663E00,		//44 - D
			0x007E063E06067E00,		//45 - E
			0x0006063E06067E00,		//46 - F
			0x003C467606463C00,		//47 - G
			0x0066667E7E666600,		//48 - H
			0x007E181818187E00,		//49 - I
			0x001C3E3230307E00,		//4A - J
			0x0066361E1E366600,		//4B - K
			0x007E7E0606060600,		//4C - L
			0x00425A5A7E664200,		//4D - M
			0x0062727A5E4E4600,		//4E - N
			0x003C666666663C00,		//4F - O
			0x0006063E66663E00,		//50 - P
			0x005C265646463C00,		//51 - Q
			0x0066663E46463E00,		//52 - R
			0x003C42380E463C00,		//53 - S
			0x00181818187E7E00,		//54 - T
			0x003C7E6666666600,		//55 - U
			0x00183C7E66666600,		//56 - V
			0x0042667E5A424200,		//57 - W
			0x00663C183C664200,		//58 - X
			0x001818183C664200,		//59 - Y
			0x007E0E1C38707E00,		//5A - Z
			0x1C0404040404041C,		//5B - [
			0x8040201008040201,		//5C - '\'
			0x1C1010101010101C,		//5D - ]
			0x0000000000422418,		//5E - ^
			0x7E00000000000000,		//5F - _
			0x000000000000180C,		//60 - `
			0x0038242418000000,		//61 - a
			0x001C24241C040400,		//62 - b
			0x0038040438000000,		//63 - c
			0x0038242438202000,		//64 - d
			0x0018043C18000000,		//65 - e
			0x0004040E04241800,		//66 - f
			0x1820382424180000,		//67 - g
			0x0024241C04040400,		//68 - h
			0x0010101800100000,		//69 - i
			0x1824203000200000,		//6A - j
			0x00140C1404040400,		//6B - k
			0x0010080808080800,		//6C - l
			0x002A2A2A16000000,		//6D - m
			0x0024242418000000,		//6E - n
			0x0018242418000000,		//6F - o
			0x041C242418000000,		//70 - p
			0x2038242418000000,		//71 - q
			0x0004040418000000,		//72 - r
			0x001C201C04380000,		//73 - s
			0x001008081C080000,		//74 - t
			0x0018242424000000,		//75 - u
			0x0018182424000000,		//76 - v
			0x00245A4242000000,		//77 - w
			0x0024181824000000,		//78 - x
			0x1820382424000000,		//79 - y
			0x003C08103C000000,		//7A - z
			0x3808080404080838,		//7B - {
			0x1818181818181818,		//7C - |
			0x1C1010202010101C,		//7D - }
			0x000000000000324C,		//7E - ~
			0x007E424224180000,		//7F - ⌂
			0x083C420202423C00,		//80 - Ç
			0x0018242424002400,		//81 - ü
			0x0018043C18002000,		//82 - é
			0x0038242418002810,		//83 - â
			0x0038242418002400,		//84 - ä
			0x0038242418000402,		//85 - à
			0x0038242418001818,		//86 - å
			0x0838040438000000,		//87 - ç
			0x0018043C18001408,		//88 - ê
			0x0018043C18002400,		//89 - ë
			0x0018043C18000402,		//8A - è
			0x0038101018002400,		//8B - ï
			0x0038101018001408,		//8C - î
			0x0038101018000402,		//8D - ì
			0x0066667E66241842,		//8E - Ä
			0x0066667E66241818,		//8F - Å
			0x007E063E067E1020,		//90 - É
			0x007609F999760000,		//91 - æ
			0x0072127E12147800,		//92 - Æ
			0x0018242418001408,		//93 - ô
			0x0018242418002400,		//94 - ö
			0x0018242418000402,		//95 - ò
			0x0018242424001408,		//96 - û
			0x0018242424000804,		//97 - ù
			0x1820382424002400,		//98 - ÿ
			0x003C666666663C42,		//99 - Ö
			0x003C666666660042,		//9A - Ü
			0x0838141414382000,		//9B - ¢
			0x003E04040E241800,		//9C - £
			0x187E187E18244200,		//9D - ¥
			0x0000000000000000,		//9E - ₧
			0x060808083C084830,		//9F - ƒ
			0x0038242418001020,		//A0 - á
			0x0038101018001020,		//A1 - í
			0x0018242418001020,		//A2 - ó
			0x0018242424001020,		//A3 - ú
			0x0024242418001428,		//A4 - ñ
			0x0062524A46421428,		//A5 - Ñ
			0x0000003C00382418,		//A6 - ª
			0x0000003C00182418,		//A7 - º
			0x3C42021C00100000,		//A8 - ¿
			0x000000027E000000,		//A9 - ⌐
			0x000000407E000000,		//AA - ¬
			0xE16284E817224382,		//AB - ½
			0x81E2A40817224382,		//AC - ¼
			0x1010101000100000,		//AD - ¡
			0x0048241212244800,		//AE - «
			0x0012244848241200,		//AF - »

			0x00AA005500AA0055,		//B0 - ░

			0xAA55AA55AA55AA55,		//B1 - ▒

			0xFF55FF55FF55FF55,     //B2 - ▓

			0x1818181818181818,     //B3 - │
			0x1818181F1F181818,		//B4 - ┤
			0x18181F00001F1818,		//B5 - ╡
			0x2424242727242424,		//B6 - ╢
			0x2424243F3F000000,		//B7 - ╖
			0x18181F00001F0000,		//B8 - ╕
			0x2424272020272424,		//B9 - ╣
			0x2424242424242424,		//BA - ║
			0x24242720203F0000,		//BB - ╗
			0x00003F2020272424,		//BC - ╝
			0x0000002727242424,		//BD - ╜
			0x00001F00001F1818,		//BE - ╛
			0x0000000000000000,		//BF - ┐
			0x0000000000000000,		//C0 - └
			0x0000000000000000,		//C1 - ┴
			0x0000000000000000,		//C2 - ┬
			0x0000000000000000,		//C3 - ├
			0x0000000000000000,		//C4 - ─
			0x0000000000000000,		//C5 - ┼
			0x0000000000000000,		//C6 - ╞
			0x0000000000000000,		//C7 - ╟
			0x0000000000000000,		//C8 - ╚
			0x0000000000000000,		//C9 - ╔
			0x0000000000000000,		//CA - ╩
			0x0000000000000000,		//CB - ╦
			0x0000000000000000,		//CC - ╠
			0x0000000000000000,		//CD - ═
			0x0000000000000000,		//CE - ╬
			0x0000000000000000,		//CF - ╧
			0x0000000000000000,		//D0 - ╨
			0x0000000000000000,		//D1 - ╤
			0x0000000000000000,		//D2 - ╥
			0x0000000000000000,		//D3 - ╙
			0x0000000000000000,		//D4 - ╘
			0x0000000000000000,		//D5 - ╒
			0x0000000000000000,		//D6 - ╓
			0x0000000000000000,		//D7 - ╫
			0x0000000000000000,		//D8 - ╪
			0x0000000000000000,		//D9 - ┘
			0x0000000000000000,		//DA - ┌
			0x0000000000000000,		//DB - █
			0x0000000000000000,		//DC - ▄
			0x0000000000000000,		//DD - ▌
			0x0000000000000000,		//DE - ▐
			0x0000000000000000,		//DF - ▀
			0x0000000000000000,		//E0 - α
			0x0000000000000000,		//E1 - ß
			0x0000000000000000,		//E2 - Γ
			0x0000000000000000,		//E3 - π
			0x0000000000000000,		//E4 - Σ
			0x0000000000000000,		//E5 - σ
			0x0000000000000000,		//E6 - µ
			0x0000000000000000,		//E7 - τ
			0x0000000000000000,		//E8 - Φ
			0x0000000000000000,		//E9 - Θ
			0x0000000000000000,		//EA - Ω
			0x0000000000000000,		//EB - δ
			0x0000000000000000,		//EC - ∞
			0x0000000000000000,		//ED - φ
			0x0000000000000000,		//EE - ε
			0x0000000000000000,		//EF - ∩
			0x0000000000000000,		//F0 - ≡
			0x0000000000000000,		//F1 - ±
			0x0000000000000000,		//F2 - ≥
			0x0000000000000000,		//F3 - ≤
			0x0000000000000000,		//F4 - ⌠
			0x0000000000000000,		//F5 - ⌡
			0x0000000000000000,		//F6 - ÷
			0x0000000000000000,		//F7 - ≈
			0x0000000000000000,		//F8 - °
			0x0000000000000000,		//F9 - ∙
			0x0000000000000000,		//FA - ·
			0x0000000000000000,		//FB - √
			0x0000000000000000,		//FC - ⁿ
			0x0000000000000000,		//FD - ²
			0x0000000000000000,		//FE - ■
			0x0000000000000000		//FF - 
		};
	}
}