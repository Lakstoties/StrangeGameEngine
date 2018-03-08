#pragma once
#include "display.h"
#include <fstream>
#include <chrono>

//
//  Strange Game Engine Main Namespace
//
namespace SGE
{
	namespace Render
	{
		//
		//  Structure to handle simple 2D vertex points
		//
		struct VertexPoint
		{
			int x;
			int y;
		};


		//
		//  Pixels, Colors, and other Visual bits!
		//

		//
		//  Color Format
		//


		//  32-bit Unsigned Int
		//     XX BB GG RR
		//  0x 00 00 00 00
		//  
		//  xx = Extra Data	- In theory it is supposed to be used for texture transparency, but that is disabled in OpenGL.  But can be used for others things now.
		//  BB = Blue Data
		//  GG = Green Data
		//  RR = Red Data


		namespace Colors
		{
			//
			//  Classic Named Colors from 3/4 bit days
			//
			namespace Named
			{
				//
				//  Values in use are from the standard defined for PuTTY
				//  Because I like PuTTY
				//  So there...
				//

				//  Regular Colors
				//  Data Formating Notes						  XXBBGGRR
				const SGE::Display::Video::pixel Black = 0x00000000;
				const SGE::Display::Video::pixel Red = 0x000000BB;
				const SGE::Display::Video::pixel Green = 0x0000BB00;
				const SGE::Display::Video::pixel Yellow = 0x0000BBBB;
				const SGE::Display::Video::pixel Blue = 0x00BB0000;
				const SGE::Display::Video::pixel Magenta = 0x00BB00BB;
				const SGE::Display::Video::pixel Cyan = 0x00BBBB00;
				const SGE::Display::Video::pixel White = 0x00BBBBBB;

				//  Bright Colors
				//  Data Formating Notes						  XXBBGGRR
				const SGE::Display::Video::pixel BrightBlack = 0x00555555;
				const SGE::Display::Video::pixel BrightRed = 0x005555FF;
				const SGE::Display::Video::pixel BrightGreen = 0x0055FF55;
				const SGE::Display::Video::pixel BrightYellow = 0x0055FFFF;
				const SGE::Display::Video::pixel BrightBlue = 0x00FF5555;
				const SGE::Display::Video::pixel BrightMagenta = 0x00FF55FF;
				const SGE::Display::Video::pixel BrightCyan = 0x00FFFF55;
				const SGE::Display::Video::pixel BrightWhite = 0x00FFFFFF;
			}


			//
			//  3 - 4 Bit Color Mode Map (8/16 Colors)
			//
			const SGE::Display::Video::pixel ColorMode4Bit[16] =
			{
				//Regular Colors
				//XXBBGGRR		Name				Hex		Bit Field
				0x00000000,		//Black				0		0000
				0x000000BB,		//Red				1		0001
				0x0000BB00,		//Green				2		0010
				0x0000BBBB,		//Yellow			3		0011
				0x00BB0000,		//Blue				4		0100
				0x00BB00BB,		//Magenta			5		0101
				0x00BBBB00,		//Cyan				6		0110
				0x00BBBBBB,		//White				7		0111

				//Bright Colors
				//XXBBGGRR							Hex		Bit Field
				0x00555555,		//Bright Black		8		1000
				0x005555FF,		//Bright Red		9		1001
				0x0055FF55,		//Bright Green		A		1010
				0x0055FFFF,		//Bright Yellow		B		1011
				0x00FF5555,		//Bright Blue		C		1100
				0x00FF55FF,		//Bright Magenta	D		1101
				0x00FFFF55,		//Bright Cyan		E		1110
				0x00FFFFFF		//Bright White		F		1111
			};


			//
			//  256 Color Mode Map
			//
			//  These colors were dervied from Wikipedia under ANSI escape codes
			//  May be tweaked for effect later
			//	Leaving them alone for now
			const SGE::Display::Video::pixel ColorMode8Bit[256] =
			{
				//Standard Colors (0-7)
				//XXBBGGRR		//Hex	//Dec
				0x00000000,		//00	000
				0x00000080,		//01	001
				0x00008000,		//02	002
				0x00008080,		//03	003
				0x00800000,		//04	004
				0x00800080,		//05	005
				0x00808000,		//06	006
				0x00c0c0c0,		//07	007

				//High-Insensity Colors (8-15)
				//XXBBGGRR
				0x00808080,		//08	008
				0x000000FF,		//09	009
				0x0000FF00,		//0A	010
				0x0000FFFF,		//0B	011
				0x00FF0000,		//0C	012
				0x00FF00FF,		//0D	013
				0x00FFFF00,		//0E	014
				0x00FFFFFF,		//0F	015

				//216 Colors (16-231)
				//XXBBGGRR		//Hex
				0x00000000,		//10	016
				0x005F0000,		//11	017
				0X00870000,		//12	018
				0X00AF0000,		//13	019
				0x00D70000,		//14	020
				0x00FF0000,		//15	021

				0x00005F00,		//16	022
				0x005F5F00,		//17	023
				0x00875F00,		//18	024
				0x00AF5F00,		//19	025
				0x00D75F00,		//1A	026
				0x00FF5F00,		//1B	027

				0x00008700,		//1C	028
				0x005F8700,		//1D	029
				0x00878700,		//1E	030
				0x00AF8700,		//1F	031
				0x00D78700,		//20	032
				0x00FF8700,		//21	033

				0x0000AF00,		//22	034
				0x005FAF00,		//23	035
				0x0087AF00,		//24	036
				0x00AFAF00,		//25	037
				0x00D7AF00,		//26	038
				0x00FFAF00,		//27	039

				0x0000D700,		//28	040
				0x005FD700,		//29	041
				0x0087D700,		//2A	042
				0x00AFD700,		//2B	043
				0x00D7D700,		//2C	044
				0x00FFD700,		//2D	045

				0x0000FF00,		//2E	046
				0x005FFF00,		//2F	047
				0x0087FF00,		//30	048
				0x00AFFF00,		//31	049
				0x00D7FF00,		//32	050
				0x00FFFF00,		//33	051

				//XXBBGGRR		//Hex
				0x0000005F,		//34	052
				0x005F005F,		//35	053
				0X0087005F,		//36	054
				0X00AF005F,		//37	055
				0x00D7005F,		//38	056
				0x00FF005F,		//39	057
		
				0x00005F5F,		//3A	058
				0x005F5F5F,		//3B	059
				0x00875F5F,		//3C	060
				0x00AF5F5F,		//3D	061
				0x00D75F5F,		//3E	062
				0x00FF5F5F,		//3F	063

				0x0000875F,		//40	064
				0x005F875F,		//41	065
				0x0087875F,		//42	066
				0x00AF875F,		//43	067
				0x00D7875F,		//44	068
				0x00FF875F,		//45	069

				0x0000AF5F,		//46	070
				0x005FAF5F,		//47	071
				0x0087AF5F,		//48	072
				0x00AFAF5F,		//49	073
				0x00D7AF5F,		//4A	074
				0x00FFAF5F,		//4B	075

				0x0000D75F,		//4C	076
				0x005FD75F,		//4D	077
				0x0087D75F,		//4E	078
				0x00AFD75F,		//4F	079
				0x00D7D75F,		//50	080
				0x00FFD75F,		//51	081

				0x0000FF5F,		//52	082
				0x005FFF5F,		//53	083
				0x0087FF5F,		//54	084
				0x00AFFF5F,		//55	085
				0x00D7FF5F,		//56	086
				0x00FFFF5F,		//57	087

				//XXBBGGRR		//Hex
				0x00000087,		//58	088
				0x005F0087,		//59	089
				0X00870087,		//5A	090
				0X00AF0087,		//5B	091
				0x00D70087,		//5C	092
				0x00FF0087,		//5D	093

				0x00005F87,		//5E	094
				0x005F5F87,		//5F	095
				0x00875F87,		//60	096
				0x00AF5F87,		//61	097
				0x00D75F87,		//62	098
				0x00FF5F87,		//63	099

				0x00008787,		//64	100
				0x005F8787,		//65	101
				0x00878787,		//66	102
				0x00AF8787,		//67	103
				0x00D78787,		//68	104
				0x00FF8787,		//69	105

				0x0000AF87,		//6A	106
				0x005FAF87,		//6B	107
				0x0087AF87,		//6C	108
				0x00AFAF87,		//6D	109
				0x00D7AF87,		//6E	110
				0x00FFAF87,		//6F	111

				0x0000D787,		//70	112
				0x005FD787,		//71	113
				0x0087D787,		//72	114
				0x00AFD787,		//73	115
				0x00D7D787,		//74	116
				0x00FFD787,		//75	117

				0x0000FF87,		//76	118
				0x005FFF87,		//77	119
				0x0087FF87,		//78	120
				0x00AFFF87,		//79	121
				0x00D7FF87,		//7A	122
				0x00FFFF87,		//7B	123

				//XXBBGGRR		//Hex
				0x000000AF,		//7C	124
				0x005F00AF,		//7D	125
				0X008700AF,		//7E	126
				0X00AF00AF,		//7F	127
				0x00D700AF,		//80	128
				0x00FF00AF,		//81	129

				0x00005FAF,		//82	130
				0x005F5FAF,		//83	131
				0x00875FAF,		//84	132
				0x00AF5FAF,		//85	133
				0x00D75FAF,		//86	134
				0x00FF5FAF,		//87	135

				0x000087AF,		//88	136
				0x005F87AF,		//89	137
				0x008787AF,		//8A	138
				0x00AF87AF,		//8B	139
				0x00D787AF,		//8C	140
				0x00FF87AF,		//8D	141

				0x0000AFAF,		//8E	142
				0x005FAFAF,		//8F	143
				0x0087AFAF,		//90	144
				0x00AFAFAF,		//91	145
				0x00D7AFAF,		//92	146
				0x00FFAFAF,		//93	147

				0x0000D7AF,		//94	148
				0x005FD7AF,		//95	149
				0x0087D7AF,		//96	150
				0x00AFD7AF,		//97	151
				0x00D7D7AF,		//98	152
				0x00FFD7AF,		//99	153

				0x0000FFAF,		//9A	154
				0x005FFFAF,		//9B	155
				0x0087FFAF,		//9C	156
				0x00AFFFAF,		//9D	157
				0x00D7FFAF,		//9E	158
				0x00FFFFAF,		//9F	159

				//XXBBGGRR		//Hex
				0x000000D7,		//A0	160
				0x005F00D7,		//A1	161
				0X008700D7,		//A2	162
				0X00AF00D7,		//A3	163
				0x00D700D7,		//A4	164
				0x00FF00D7,		//A5	165

				0x00005FD7,		//A6	166
				0x005F5FD7,		//A7	167
				0x00875FD7,		//A8	168
				0x00AF5FD7,		//A9	169
				0x00D75FD7,		//AA	170
				0x00FF5FD7,		//AB	171

				0x000087D7,		//AC	172
				0x005F87D7,		//AD	173
				0x008787D7,		//AE	174
				0x00AF87D7,		//AF	175
				0x00D787D7,		//B0	176
				0x00FF87D7,		//B1	177

				0x0000AFD7,		//B2	178
				0x005FAFD7,		//B3	179
				0x0087AFD7,		//B4	180
				0x00AFAFD7,		//B5	181
				0x00D7AFD7,		//B6	182
				0x00FFAFD7,		//B7	183

				0x0000D7D7,		//B8	184
				0x005FD7D7,		//B9	185
				0x0087D7D7,		//BA	186
				0x00AFD7D7,		//BB	187
				0x00D7D7D7,		//BC	188
				0x00FFD7D7,		//BD	189

				0x0000FFD7,		//BE	190
				0x005FFFD7,		//BF	191
				0x0087FFD7,		//C0	192
				0x00AFFFD7,		//C1	193
				0x00D7FFD7,		//C2	194
				0x00FFFFD7,		//C3	195

				//XXBBGGRR		//Hex
				0x000000FF,		//C4	196
				0x005F00FF,		//C5	197
				0X008700FF,		//C6	198
				0X00AF00FF,		//C7	199
				0x00D700FF,		//C8	200
				0x00FF00FF,		//C9	201

				0x00005FFF,		//CA	202
				0x005F5FFF,		//CB	203
				0x00875FFF,		//CC	204
				0x00AF5FFF,		//CD	205
				0x00D75FFF,		//CE	206
				0x00FF5FFF,		//CF	207

				0x000087FF,		//D0	208
				0x005F87FF,		//D1	209
				0x008787FF,		//D2	210
				0x00AF87FF,		//D3	211
				0x00D787FF,		//D4	212
				0x00FF87FF,		//D5	213

				0x0000AFFF,		//D6	214
				0x005FAFFF,		//D7	215
				0x0087AFFF,		//D8	216
				0x00AFAFFF,		//D9	217
				0x00D7AFFF,		//DA	218
				0x00FFAFFF,		//DB	219

				0x0000D7FF,		//DC	220
				0x005FD7FF,		//DD	221
				0x0087D7FF,		//DE	222
				0x00AFD7FF,		//DF	223
				0x00D7D7FF,		//E0	224
				0x00FFD7FF,		//E1	225

				0x0000FFFF,		//E2	226
				0x005FFFFF,		//E3	227
				0x0087FFFF,		//E4	228
				0x00AFFFFF,		//E5	229
				0x00D7FFFF,		//E6	230
				0x00FFFFFF,		//E7	231

				//Grayscale Colors (232-255)
				//XXBBGGRR		//Hex
				0x00080808,		//E8	232
				0x00121212,		//E9	233
				0x001C1C1C,		//EA	234
				0x00262626,		//EB	235
				0x00303030,		//EC	236
				0x003A3A3A,		//ED	237
				0x00444444,		//EE	238
				0x004E4E4E,		//EF	239
				0x00585858,		//F0	240
				0x00626262,		//F1	241
				0x006C6C6C,		//F2	242
				0x00767676,		//F3	243
				0x00808080,		//F4	244
				0x008A8A8A,		//F5	245
				0x00949494,		//F6	246
				0x009E9E9E,		//F7	247
				0x00A8A8A8,		//F8	248
				0x00B2B2B2,		//F9	249
				0x00BCBCBC,		//FA	250
				0x00C6C6C6,		//FB	251
				0x00D0D0D0,		//FC	252
				0x00DADADA,		//FD	253
				0x00E4E4E4,		//FE	254
				0x00EEEEEE		//FF	255
			};

		}

		class AnimationBlock
		{
		public:
			//buffer array to store the images and data
			unsigned int** ImageBuffers = nullptr;

			//Number of images
			unsigned int NumberOfImages = 0;

			//Resolution of images
			//X
			unsigned int ResolutionX = 0;

			//Y
			unsigned int ResolutionY = 0;

			//Individual Image Buffer Size
			unsigned int ImageBufferSize = 0;

			//Status Flag for Animation
			bool Animating = false;

			//Animation rate
			unsigned int MillisecondsPerFrame = 0;

			//Reverse Animation
			bool ReverseAnimation = false;

			//Current image from buffer
			unsigned int CurrentImageBuffer = 0;

			//Timer
			std::chrono::time_point<std::chrono::steady_clock> previousTime;
			std::chrono::milliseconds deltaTime = std::chrono::milliseconds(0);


			void CreateBuffers(unsigned int number, unsigned int XSize, unsigned int YSize);
			void DeleteBuffers();
			bool LoadBuffer(unsigned int number, unsigned int* sourceBuffer, unsigned int XOffset, unsigned int YOffset);
			void StartAnimation();
			void ResetAnimation();
			void DrawAnimation(int XPosition, int YPosition);
		};

		//
		//  Draw Simple Mouse Cursor
		//
		void DrawMouseSimpleCursor(unsigned int cursorRadius, SGE::Display::Video::pixel cursorColor);

		//
		//  Draw a character to the screen
		//
		void Draw8x8Character(const unsigned long long &character, int targetX, int targetY, const SGE::Display::Video::pixel &color);

		//
		//  Draw a string of characters to the screen, from a null terminated string
		//
		void DrawString(char* characters, const unsigned long long characterROM[], int characterSpacing, int targetX, int targetY, SGE::Display::Video::pixel color);

		//
		//  Draw a block of data from a source to target video ram
		//
		void DrawDataBlock(int targetX, int targetY, int sourceWidth, int sourceHeight, unsigned int* sourceDataBlock);

		//
		//  Function that prunes a line represented by two points to within the viewable area of the game screen
		//
		inline bool PruneLine(int& startX, int& startY, int& endX, int& endY);

		//
		//  Draw a line from one point to another
		//
		void DrawLine(int startX, int startY, int endX, int endY, SGE::Display::Video::pixel color);

		//
		//  Draw a rectangle
		//
		void DrawRectangle(int startX, int startY, int width, int height, SGE::Display::Video::pixel color);

		//
		//  Draw a filled box
		//
		void DrawBox(int startX, int startY, int width, int height, SGE::Display::Video::pixel color);

		//
		//  Blank the video ram with zero alpha black.  Or the blackest of blacks... it's like you can't get any blacker.
		//
		void ZBlank();

		//
		//  Pack the byte colors in to a 4 byte pixel to use.
		//
		SGE::Display::Video::pixel PackColors(unsigned char redValue, unsigned char greenValue, unsigned char blueValue);

		//
		//  Arbitrary Vector Shape drawing
		//
		void DrawVectorShape(int startX, int startY, float scalingFactor, int numberOfVertexes, VertexPoint vertexes[], SGE::Display::Video::pixel color);

		//
		//  Draw a filled triangle
		//
		void DrawFilledTriangleTrue(int startX, int startY, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, SGE::Display::Video::pixel color);

		//
		//  Draw a filled triangle (New method)
		//
		void DrawFilledTriangleFast(int startX, int startY, VertexPoint vertex1, VertexPoint vertex2, VertexPoint vertex3, SGE::Display::Video::pixel color);

		//
		//  Draw a list of triangles
		//
		void DrawFilledTriangles(int startX, int startY, float scalingFactor, VertexPoint* vertexArray, unsigned int numberOfVertexes, SGE::Display::Video::pixel color);

		//
		//  Character ROM for an 8x8 character set
		//

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