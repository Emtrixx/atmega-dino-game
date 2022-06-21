/*
 * tft_display.c
 *
 * Created: 01.06.2022 22:29:45
 *  Author: jesse
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "tft_display.h"

#define SPI_DDR DDRB
#define CS PINB2
#define MOSI PINB3
#define SCK PINB5
#define D_C PIND2 //Display: Data/Command
#define Reset PIND3 //Display: Reset

const uint8_t TFT_MAX_X = 131;
const uint8_t TFT_MAX_Y = 175;

void SPI_init(){
	//set CS, MOSI and SCK to output:
	SPI_DDR |= (1 << CS) | (1 << MOSI) | (1 << SCK);

	//enable SPI, set as master, and clock to fosc/4:
	SPCR = (1 << SPE) | (1 << MSTR);
}

void Display_init() {
	DDRD |= (1<<D_C)|(1<<Reset); //output: PD2 -> Data/Command; PD3 -> Reset
	SPI_init();
	const uint16_t InitData[] ={
		//Initialisierungsdaten fuer 256 Farben Modus
		0xFDFD, 0xFDFD,
		//pause 75ms
		0xEF00, 0xEE04, 0x1B04, 0xFEFE, 0xFEFE,
		0xEF90, 0x4A04, 0x7F1F, 0xEE04, 0x4306,
		//pause 75ms
		0xEF90, 0x0983, 0x0800, 0x0BAF, 0x0A00,
		0x0500, 0x0600, 0x0700, 0xEF00, 0xEE0C,
		0xEF90, 0x0080, 0xEFB0, 0x4902, 0xEF00,
		0x7F01, 0xE181, 0xE202, 0xE276, 0xE183,
		0x8001, 0xEF90, 0x0000
	};
	_delay_ms(300);
	PORTD &= !(1<<Reset);			//Reset-Eingang des Displays auf Low => Beginn Hardware-Reset
	_delay_ms(75);
	PORTB |= (1<<CS);				//SSEL auf High
	_delay_ms(75);
	PORTD |= (1<<D_C);				//Data/Command auf High
	_delay_ms(75);
	PORTD |= (1<<Reset);			//Reset-Eingang des Displays auf High => Ende Hardware Reset
	_delay_ms (75);
	SendCommandSeq(&InitData[0], 2);
	_delay_ms (75);
	SendCommandSeq(&InitData[2], 10);
	_delay_ms (75);
	SendCommandSeq(&InitData[12], 23);}

void SendCommandSeq(const uint16_t * data, uint16_t Anzahl){
	uint32_t index;
	uint8_t SendeByte;
	for (index=0; index<Anzahl; index++){
		PORTD |= (1<<D_C);						//Data/Command auf High => Kommando-Modus, keine Farbdaten rectangle setzen)
		SendeByte = (data[index] >> 8) & 0xFF;	//High-Byte des Kommandos
		SPISend8Bit(SendeByte);
		SendeByte = data[index] & 0xFF;			//Low-Byte des Kommandos
		SPISend8Bit(SendeByte);
		PORTD &= ~(1<<D_C);						//Data/Command auf Low => Daten-Modus
	}
}

void SPISend8Bit(uint8_t data){
	PORTB &= ~(1<<CS); //CS low
	SPDR = data; //load data into register
	while(!(SPSR & (1 << SPIF))); //wait for transmission complete
	PORTB |= (1<<CS); //CS high
}

void TFT_Window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, enum TFT_Orientation orientation) {
	uint16_t data[] =
	{
		0xEF08,
		0x1800,
		0x1200, // x1
		0x1500, // x2
		0x1300, // y1
		0x1600, // y2
	};
	
	switch(orientation) {
		default:
		case TFT_Portrait:
		data[2] |= x1;
		data[3] |= x2;
		data[4] |= y1;
		data[5] |= y2;
		break;
		case TFT_Portrait180:
		data[1] |= 0x03;
		data[2] |= TFT_MAX_X - x1;
		data[3] |= TFT_MAX_X - x2;
		data[4] |= TFT_MAX_Y - y1;
		data[5] |= TFT_MAX_Y - y2;
		break;
		case TFT_Landscape:
		data[1] |= 0x05;
		data[2] |= TFT_MAX_X - y1;
		data[3] |= TFT_MAX_X - y2;
		data[4] |= x1;
		data[5] |= x2;
		break;
		case TFT_Landscape180:
		data[1] |= 0x03;
		data[2] |= y1;
		data[3] |= y2;
		data[4] |= TFT_MAX_Y - x1;
		data[5] |= TFT_MAX_Y - x2;
		break;
	}
	SendCommandSeq(data, 6);
}