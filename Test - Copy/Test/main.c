/*
 * Test.c
 *
 * Created: 1/15/2026 12:15:59 PM
 * Author : Eliph
 */ 

#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "SSD/SSD.h"

//GLOBAL:
uint8_t CN = 0x00;
uint8_t RL = 0x00;
uint8_t RN1 = 0x01;
uint8_t RN2 = 0x20;
uint8_t AID = 0x10;


//DEBOUNCE GLOBAL:
volatile uint8_t FCB4 = 0x00;
volatile uint8_t SCB4 = 0x01;
volatile uint8_t FCC0 = 0x00;
volatile uint8_t FCC1 = 0x00;
volatile uint8_t SCC0 = 0x01;
volatile uint8_t SCC1 = 0x01;
volatile uint8_t ST00 = 0x00;
volatile uint8_t ST01 = 0x00;
volatile uint8_t L0 = 0x01;
volatile uint8_t L1 = 0x01;
volatile uint8_t GO = 0x00;


//SETUP:
void SETUP(void){
	cli();
		
	//PORTB as output:
	DDRB = 0xFF;
	DDRC = 0xFF;
	DDRD = 0xFF;
	
	//PORTB: Input -> PB4
	DDRB &= ~(1 << PORTB4);
	
	//PORTC: Input -> PC0 and PC1. 
	DDRC &= ~((1 << PORTC0)|(1 << PORTC1));
	
	//PULL_UP:PB4/PC0/PC1
	PORTB |= (1<<PORTB4);
	PORTC |= (1<<PORTC0) | (1<<PORTC1);
	
	//PC: Enable.
	PCICR |= (1 << PCIE0) | (1 << PCIE1);
	
	//PORTB and PORTC: PB4/PC0/PC1.
	PCMSK0 |= (1 << PCINT4);
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
	
	//DISPLAY INITIAL:
	PORTD = 0xC9;
	
	//TIMERS: 16 prescaler -> 1MHz (One tick every 1us).
	CLKPR = (1 << CLKPCE);
	CLKPR = (1 << CLKPS2);
	
	//TIMER 0: 10ms
	TCCR0B |= (1<<CS00) | (1<<CS01);
	TCNT0 = 100;
	TIMSK0 = (1<<TOIE0);
	
	//TIMER 1: 1s
	TCCR1B |= (1<<CS10) | (1<<CS11);
	TCNT1 = 49911;
	TIMSK1 |= (1<<TOIE1);
	
	sei();
}

//ISR:
ISR(TIMER0_OVF_vect){
	TCNT0 = 100;
	SCB4 = (PINB & (1<<PINB4));
	SCC0 = (PINC & (1<<PINC0));
	SCC1 = (PINC & (1<<PINC1));
	
	if(RL == 0x01){
		if(FCC0 == SCC0 && SCC0 == 0 && L0 == 0x01){
			//PORTB: 0-4.
			PORTB = (PORTB & 0xF0) | RN1;
			if (RN1 == 0x08) {
				PORTB = (PORTB & 0xF0) | 0x0F;
				//P1: WIN.
				RL = 0x00;
				PORTD = 0x06;
				PORTB = (PORTB & 0xF0) | 0x0F;
				PORTC &= 0xC3;
				
			}
			else {
				RN1 <<= 1;   // 0x01 ? 0x02 ? 0x04 ? 0x08
			}
			
			L0 = 0x00;
		}
		else {}
		
		if(FCC1 == SCC1 && SCC1 == 0 && L1 == 0x01){
			//PORTC: 2-5.
			PORTC = (PORTC & ~0x3C) | RN2;

			if (RN2 == 0x04) {
				PORTC = (PORTC & ~0x3C) | 0x3C;   // FULL
				//P2: WIN.
				RL = 0x00;
				PORTD = 0x5B;
				PORTC = (PORTC & 0xC3) | 0x3C;
				PORTB &= 0xF0;
			}
			else {
				RN2 >>= 1;   // shift toward PC2
			}

			L1 = 0x00;
		}
		else {}
	}
}
ISR(TIMER1_OVF_vect){
	TCNT1 = 49911;		
		
	switch(CN)
	{
		case 0x01:
		CN++;
		//5:
		MAP_7SD(0x05);
		break;

		case 0x02:
		CN++;
		//4:
		MAP_7SD(0x04);
		break;
		
		case 0x03:
		CN++;
		//3:
		MAP_7SD(0x03);
		break;
		
		case 0x04:
		CN++;
		//2:
		MAP_7SD(0x02);
		break;
		
		case 0x05:
		// code
		CN++;
		//1:
		MAP_7SD(0x01);
		break;
		
		case 0x06:
		//0:
		CN = 0x00;
		//1:
		MAP_7SD(0x00);
		
		//Release B1 and B2:
		RL = 0x01;
		break;

		default:
		//If nothing matches...
		break;
	}
}
ISR(PCINT0_vect){
	if (!(PINB & (1<<PINB4))){
		FCB4 = (PINB & (1<<PINB4));
	}
}	
ISR(PCINT1_vect){	
	if(RL == 0x01){
		if (!(PINC & (1<<PINC0))){
			FCC0 = (PINC & (1<<PINC0));
		}
		else{}
		
		if (!(PINC & (1<<PINC1))){
			FCC1 = (PINC & (1<<PINC1));
		}
		else{}
		
		if (!(PINC & (1<<PINC0)) && L0 == 0){
			//LATCH:
			L0 = 0x01;
		}
		else{}
			
		if (!(PINC & (1<<PINC1)) && L1 == 0){
			//LATCH:
			L1 = 0x01;
		}
		else{}
	}
	
}

//MAIN_LOOP:
int main(void)
{
	//SETUP:
	SETUP();
	
    //MAIN_LOOP:
    while (1) 
    {
		//PB4: STARTER.
		if (FCB4 == SCB4 && SCB4 == 0){
			//TIMER COUNTER:
			CN = 0x01;

			//RELEASE:
			RL = 0x00;
			
			//RESET:
			RN1 = 0x01;
			RN2 = 0x20;
			AID = 0x10;
			
			L0 = 0x01;
			L1 = 0x01;

			FCC0 = FCC1 = 0x00;
			SCC0 = SCC1 = 0x01;
			
			GO = 0x00;
			
			//CLEAN:
			PORTB &= 0xF0;
			PORTC &= 0xC3;
			
		}
		else{}
		
		
    }
}

