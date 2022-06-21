/*
 * GccApplication1.c
 *
 * Created: 01.06.2022 22:26:22
 * Author : jesse
 */ 

#define debug
#define F_CPU 16E6
#include <avr/io.h>
#include <avr/interrupt.h>
#include "tft_display.h"
#include "game_controller.h"

#define LED_ON		PORTC |= 1<<PORTC4
#define LED_OFF	PORTC &= ~(1<<PORTC4)
#define BUTTON_PRESS !(PIND & (1<<PIND1))

static volatile uint16_t timerCounter = 0;


int main(void)
{
	init();
    while (1) {}
}

void init(void) {
	Sonic_init();
	Display_init();
	LED_init();
	InitGame();
	Buttons_init();
}

void Sonic_init() {
	// D0 als Output
	DDRD |= 1;
	// C5 als Input
	DDRC &= ~(1<<5);
}

void LED_init() {
	// C1-4 as output
	DDRC |= 0b11110;
}

void Buttons_init() {
	// Button 1 as Input
	DDRD &= ~(1<<1);
	PORTD |= 1<<1;

	// Interrupt //
	sei();
	
	// Button1
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT17);
	
	// Sonic
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PCINT13);
	
	// Timer0 A Match enable
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 250;
	OCR0B = 1;
	// Configure CTC Mode
	TCCR0A |= (1<<WGM01);
	TCCR0A &= ~(1<<WGM00);
	TCCR0B &= ~(1<<WGM02);
	// Prescaler on 64
	TCCR0B |= 0b11;
	TCCR0B &= ~(1<<2);
	// Prescaler 1024
	//TCCR0B |= 0b101;
	//TCCR0B &= ~(1<<1);
}

// Button 1 ISR
ISR(PCINT2_vect) {
	//LED_ON;
	jumping = 1;
	PORTD |= 1;
}

// Sonic ISR
ISR(PCINT1_vect) {
	//static volatile uint8_t measured = 0; 
	TIMSK0 ^= (1<<OCIE0B);
	if (!(TIMSK0 & (1<<OCIE0B)))
	{
		if (((timerCounter * 4) / 58) < 6) {
			LED_ON;
		}
	}
	timerCounter == 0;
}

ISR(TIMER0_COMPB_vect){
	timerCounter++;
}

ISR(TIMER0_COMPA_vect){
	static volatile uint8_t gameCounter = 0;
	static volatile uint8_t jumpCounter = 0;
	static volatile uint8_t overCounter = 0;
	
	if (!gameOver) {
		// game ticks
		if (gameCounter == 25)
		{
			moveObstacles();
			// jumping
			if (jumping)
			{
				PORTD &= ~(1);
				//LED_ON;
				jump(jumpCounter);
				jumpCounter++;
				if (jumpCounter == 36)
				{
					jumping = 0;
					jumpCounter = 0;
				};
			};
			gameCounter = 0;
		};
		gameCounter++;
	} else {
		if (overCounter == 255)
		{
			BUZZER_OFF;
			LED_OFF;
			if (BUTTON_PRESS)
			{
				InitGame();
			}	
		} else {
			overCounter++;
		}
	}
	
	// TODO measure ultrasonic time
	// jede ms
	// TODO gameticks
	//
}