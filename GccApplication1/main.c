/*
 * GccApplication1.c
 *
 * Created: 01.06.2022 22:26:22
 * Author : jesse
 */ 

#define debug
#include <avr/io.h>
#include <avr/interrupt.h>
#include "tft_display.h"
#include "game_controller.h"

#define LED_ON		PORTC |= 1<<PORTC4
#define LED_OFF	PORTC &= ~(1<<PORTC4)
#define BUTTON_1_PRESS !(PIND & (1<<PIND1))
#define TRIGGER_ON PORTD |= (1<<PORTD0)
#define TRIGGER_OFF PORTD &= ~(1<<PORTD0)
#define TIMER2_ON	TCCR2B |= (1<<CS21)
#define TIMER2_OFF	TCCR2B &= ~(1<<CS21)
#define ECHO_PIN (PINC>>PINC5)&1

static volatile uint16_t timerCounter = 0;
static volatile uint8_t triggerOnBool = 0;
static volatile uint8_t idleBool = 0;
static volatile uint16_t absentCounter = 0;


int main(void)
{
	init();
    while (1) {}
}

void init(void) {
	// Interrupt
	sei();
	
	Sonic_init();
	Display_init();
	InitGame();
	Buttons_init();
	Timer_init();
	
	//debug
	// C1-4 as output
	DDRC |= 0b11110;
}

void Sonic_init() {
	// D0 as Output
	DDRD |= 1;
	// C5 as Input
	DDRC &= ~(1<<5);
	// Interrupt
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PCINT13);
}

void Buttons_init() {
	// Button 1 as Input
	DDRD &= ~(1<<1);
	PORTD |= 1<<1;

	// Button1
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT17);
}

Timer_init(){
	// Timer0 A Match enable
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 250;
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
	
	//Timer2 A Match enable
	TIMSK2 |= (1<<OCIE2A);
	OCR2A = 116;
	// Configure CTC Mode
	TCCR2A |= (1<<WGM21);
	TCCR2A &= ~(1<<WGM20);
	TCCR2B &= ~(1<<WGM22);
	//Prescaler aus
	//TCCR2B &= ~(1<<CS21);
	TCCR2B &= ~(1<<CS20);
	TCCR2B &= ~(1<<CS22);
}

// Button 1 ISR
ISR(PCINT2_vect) {
	jumping = 1;
}

// Sonic ISR
ISR(PCINT1_vect) {
	if (ECHO_PIN){
		timerCounter = 0;
	} else {
		TIMER2_OFF;
		//logInt(timerCounter);
		//TCNT2 = 0;
		if (timerCounter < 80) {
			idleBool = 0;
		} else {
			LED_OFF;
			absentCounter++;
			if (absentCounter > 5)
			{
				idleBool = 1;
				absentCounter = 0;
			}
		}
		timerCounter = 0;
	}
}

void logInt(uint16_t counter) {
	TFT_Window(85, 42, 170, 60, TFT_Landscape);
	for(int i = 0; i < 1643; i++) {
		SPISend8Bit(0xFF);
	};
	char str[12];
	sprintf(str, "%d", counter);
	TFT_Print(str, 85, 42, 2, TFT_8BitBlack, TFT_8BitWhite, TFT_Landscape);
}

void screensaver(uint16_t counter) {
	if (counter == 1)
	{
		TFT_Window(0, 0, 175, 131, TFT_Landscape);
		for(int i = 0; i < 23232; i++) {
			SPISend8Bit(0xFF);
		};
	}
	uint8_t colors[7] = {TFT_8BitBlack,TFT_8BitGreen, TFT_8BitCyan,TFT_8BitBlue,TFT_8BitPurple,TFT_8BitRed,TFT_8BitWhite};
	
	char str[] = "SCREENSAVER";
	TFT_Print(str, 22, 60, 2, colors[(counter / 5)%7], TFT_8BitWhite, TFT_Landscape);
}


// Timer 2 - Distance Counter
ISR(TIMER2_COMPA_vect){
	if (triggerOnBool)
	{
		//LED_ON;
		TRIGGER_OFF;
		triggerOnBool = 0;
	}
	//LED_ON;
	timerCounter++;
}

// Timer 0 - Game Loop
ISR(TIMER0_COMPA_vect){
	static volatile uint8_t gameCounter = 0;
	static volatile uint8_t jumpCounter = 0;
	static volatile uint8_t overCounter = 0;
	static volatile uint8_t measureCounter = 0;
	static volatile uint16_t idleCounter = 0;
	static volatile uint8_t initBool = 0;
	
	// Polling for debugging
	//if (BUTTON_1_PRESS) {
		//counter1++;
		//if (counter1 == 100)
		//{
			//counter1 = 0;
			//jumping = 1;
			////TRIGGER_ON;
		//}
	//} else
	//{
		////TRIGGER_OFF;
		//counter1 = 0;
	//}
	//
	
	if (!gameOver) {
		// game ticks
		if (gameCounter == (15))
		{
			// jumping
			if (jumping)
			{
				jump(jumpCounter);
				jumpCounter++;
				if (jumpCounter == 38)
				{
					jumping = 0;
					jumpCounter = 0;
				};
			};
			moveObstacles();
			gameCounter = 0;
		};
		gameCounter++;
	} else {
		if (overCounter == 255)
		{
			BUZZER_OFF;
			if (measureCounter == 8)
			{
				measureDistance();	
				measureCounter = 0;
			}
			measureCounter++;
			
			if (idleBool)
			{
				initBool = 0;
				idleCounter++;
				screensaver(idleCounter);
			} else if (!initBool) {
				InitGame();
				initBool = 1;
				idleCounter = 0;
			} else {
				idleCounter = 0;
			}
			
			if (BUTTON_1_PRESS)
			{
				idleBool = 0;
				absentCounter = 0;
				idleCounter = 0;
				jumpCounter = 0;
				overCounter = 0;
				measureCounter = 0;
				startGame();
				TIMER2_OFF;
			}
		} 
		overCounter++;
	}
}

void measureDistance() {
	triggerOnBool = 1;
	TCNT2 = 0;
	TRIGGER_ON;
	TIMER2_ON;
}