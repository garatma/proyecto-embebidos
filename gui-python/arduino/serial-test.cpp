#include <avr/io.h>
#include "Arduino.h"
#define PACKET_LENGHT 5

//defining the output PWM parameters
#define PWM_FREQ 0x00FF // pwm frequency - 31.3KHz
#define PWM_MODE 0 // Fast (1) or Phase Correct (0)
#define PWM_QTY 2 // 2 PWMs in parallel

char in [PACKET_LENGHT] = {-1,-1,-1,-1,-1};
unsigned int effect = 0, effect_modifier = 0;
bool change_effect = false;

int input_adc;
unsigned int ADC_low, ADC_high;

void setup()
{
	Serial.begin(9600);

	// setup ADC
	ADMUX = 0x60; // left adjust, adc0, internal vcc
	ADCSRA = 0xe5; // turn on adc, ck/32, auto trigger
	ADCSRB = 0x07; // t1 capture for trigger
	DIDR0 = 0x01; // turn off digital inputs for adc0

	// setup PWM
	TCCR1A = (((PWM_QTY - 1) << 5) | 0x80 | (PWM_MODE << 1)); //
	TCCR1B = ((PWM_MODE << 3) | 0x11); // ck/1
	TIMSK1 = 0x20; // interrupt on capture interrupt
	ICR1H = (PWM_FREQ >> 8);
	ICR1L = (PWM_FREQ & 0xff);
	DDRB |= ((PWM_QTY << 1) | 0x02); // turn on outputs
	sei(); // turn on interrupts - not really necessary with arduino
}

void loop()
{
}

bool new_effect()
{
	bool all_zeroes = true;
	int i = 0;
	while (i < PACKET_LENGHT && all_zeroes)
	{
		all_zeroes = in[i] == '0';
		i++;
	}
	return all_zeroes;
}

int string_to_int(char string [PACKET_LENGHT])
{
	int number = 0, pow = 1, i = 0;
	for (i = PACKET_LENGHT-1; i > -1; i--)
	{
		number += pow*(string[i]-'0');
		pow *= 10;
	}
	return number;
}

void serialEvent() 
{
	Serial.readBytes(in, 5);
	cli();
	if (change_effect)
	{
		change_effect = false;	
		effect = string_to_int(in);
	}
	else
	{
		if (new_effect())
			change_effect = true;
		else
			effect_modifier = string_to_int(in)-1;
	}
	sei();
}

ISR(TIMER1_CAPT_vect) 
{
	// get ADC data
	ADC_low = ADCL; // you need to fetch the low byte first
	ADC_high = ADCH;
	//construct the input sumple summing the ADC low and high byte.
	input_adc = ((ADC_high << 8) | ADC_low) + 0x8000; // make a signed 16b value

	//// All the Digital Signal Processing happens here: ////

	if (effect == 0)
	switch (effect)
	{
		case 0:
			//The bit_crush_variable goes from 0 to 16 and the input signal is crushed in the next instruction:
			input_adc = input_adc<<effect_modifier;
			break;
	}

	//write the PWM signal
	OCR1AL = ((input_adc + 0x8000) >> 8); // convert to unsigned, send out high byte
	OCR1BL = input_adc; // send out low byte
}
