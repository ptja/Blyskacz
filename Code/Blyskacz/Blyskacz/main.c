#include <stdint-gcc.h>
#include <stdbool.h>

//Fck = 9,6MHz
//Divide by 8 ON
#define F_CPU 1200000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

#define delay_20ms 2
#define delay_200ms 20
#define delay_2s 200

#define PROG_3_2V 588
#define PROG_3_0V 518

//unsigned int short EEMEM 

// global parameters
uint8_t Tryb = 255; // pe³na jasnoœæ
uint16_t ZmierzoneNapiecie;


void SleepSomeTime(uint8_t ileRazy) 
{
	while (ileRazy--)
	{
		_delay_ms(10);
	}
}


void Blysk()
{
	OCR0B = Tryb;
	SleepSomeTime(delay_20ms);
	OCR0B = 0;
}


void SekwencjaBlyskow() {
	Blysk();
	if (ZmierzoneNapiecie>PROG_3_0V) {
		SleepSomeTime(delay_200ms);
		Blysk();
		if (ZmierzoneNapiecie>PROG_3_2V) {
			SleepSomeTime(delay_200ms);
			Blysk();
		}
	}
	SleepSomeTime(delay_2s);
}


void ZmierzNapiecie() {
	ADMUX&= ~(1<<MUX1); //pomiar napiêcia na dzielniku ADC 1 - skasuj MUX1, MUX0 = 1
	ADMUX|= (1<<REFS0); //Vref = 1,1V - REFS0 = 1

	ADCSRA|= (1<<ADSC); //start konwersji
	while (ADCSRA &(1<<ADSC)); //czekaj na koniec konwersji
	ZmierzoneNapiecie = ADC;
}

int main()
{
	ADCSRA|= (1<<ADEN); //enable ADC
	ADCSRA|= (1<<ADPS1) | (1<<ADPS0); //prescaler = 8

	TCCR0B|=(1<<CS00); //Timer0 prescaler = 1
	TCCR0A|=(1<<WGM00)|(1<<COM0B1); //phase correct PWM
	DDRB|= (1<<PB1); //PB1 is  set as output

	while(true)
	{
		ZmierzNapiecie();
		SekwencjaBlyskow();
	}
}