/*
 * tft_display.h
 *
 * Created: 01.06.2022 22:35:48
 *  Author: jesse
 */ 


#ifndef TFT_DISPLAY_H_
#define TFT_DISPLAY_H_
#include <avr/io.h>

//SPI-Definitionen
#define SPI_DDR DDRB
#define CS      PINB2
#define MOSI    PINB3
#define SCK     PINB5
#define D_C		PIND2		//display: Data/Command
#define Reset	PIND3		//display: Reset

//Farbdefinitionen fuer 8-Bit-Modus
#define TFT_8BitGreen 		    	0x1C
#define TFT_8BitCyan				0x1F
#define TFT_8BitBlack 		    	0x00
#define TFT_8BitBlue				0x03
#define TFT_8BitPurple				0xE2
#define TFT_8BitRed	        		0xE0
#define TFT_8BitWhite 		    	0xFF

//Pixel-Anzahl des Displays
#define TFT_MAX_X  131
#define TFT_MAX_Y  175

//Datentyp-Definitionen
#define U8  unsigned char
#define U16	unsigned short
#define U32	unsigned int
#define S8  char
#define S16 short
#define S32 int

//Display-Ausrichtungen
#define TFT_Portrait	 0		//Hochformat
#define TFT_Portrait180  1		//Hochformat um 180° gedreht
#define TFT_Landscape 	 2		//Querformat
#define TFT_Landscape180 3		//Querformat um 180° gedreht

extern void Display_init();

extern void SendCommandSeq(const uint16_t * data, uint16_t Anzahl);

extern void SPISend8Bit(uint8_t data);

extern void TFT_Print(char* Text, U8 X, U8 Y, U8 Scale, U8 ForeColor, U8 BackColor, U8 Display_Orientation);

extern void TFT_Window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, U8 orientation);

#endif /* TFT_DISPLAY_H_ */