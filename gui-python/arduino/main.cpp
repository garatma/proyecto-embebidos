#include <avr/io.h>
#include "Arduino.h"
#define PACKET_LENGHT 5
#define DELAY_MAX 1



// TODO: evaluate if range of modifiers are reasonable
// if not, reduce them -> remove unsigned from effect_modifier

// TODO: ask if i can put waveform & delay_buffer in eeprom/flash


//defining the output PWM parameters
#define PWM_FREQ 0x00FF // pwm frequency - 31.3KHz
#define PWM_MODE 0 // Fast (1) or Phase Correct (0)
#define PWM_QTY 2 // 2 PWMs in parallel

// variables for effect/modifier switching
char input_serial [PACKET_LENGHT] = {-1,-1,-1,-1,-1};
int effect = 1, effect_modifier = 0;
bool change_effect = false;

// variables for adc conversion
int input;
unsigned int ADC_low, ADC_high;

// variables for daft punk octaver
int daft_punk_counter = 0, input_aux;

// variables for delay
byte delay_buffer[DELAY_MAX];
unsigned int delay_counter = 0;

// variables for tremolo
int speed = 1, sample = 0, divider = 0;
const byte waveform[] =
{
}; // this waveform is used to modulate the volume of the output signal.

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

bool new_effect()
{
	bool all_zeroes = true;
	int i = 0;
	while (i < PACKET_LENGHT && all_zeroes)
	{
		all_zeroes = input_serial[i] == '0';
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

void loop()
{
	/*
	if (Serial.available() > 0)
	{
		Serial.readBytes(input_serial, 5);

		if (new_effect())
		{
			change_effect = true;
			effect_modifier = 0;
		}
		else 
		{
			if (change_effect)
			{
				effect = string_to_int(input_serial);
				change_effect = false;
			}
			else
				effect_modifier = string_to_int(input_serial)-1;
		}
	}
	*/
}

void serialEvent() 
{
	Serial.readBytes(input_serial, 5);
	if (new_effect())
	{
		change_effect = true;
		effect_modifier = 0;
	}
	else 
	{
		if (change_effect)
		{
			effect = string_to_int(input_serial);
			change_effect = false;
		}
		else
			effect_modifier = string_to_int(input_serial)-1;
	}
}

ISR(TIMER1_CAPT_vect) 
{
	// get ADC data
	ADC_low = ADCL; // you need to fetch the low byte first
	ADC_high = ADCH;
	input = ((ADC_high << 8) | ADC_low) + 0x8000; // make a signed 16b value

	// switch effect
	switch (effect)
	{
		case 1:
			// bit crusher
			input = input<<effect_modifier;
			break;
		case 2:
			// booster
			input = map(input, -32768, +32768, -effect_modifier, effect_modifier);
			break;
		case 3:
			// daft punk octaver
			input_aux = input;
			input = 0;
			daft_punk_counter++;
			if (daft_punk_counter >= effect_modifier)
			{
				daft_punk_counter = 0;
				input = input_aux;
			}
			break;
		case 4:
			// delay
			// for now, no ADC_LOW

			// TODO: try assigning input
			delay_buffer[delay_counter] = ADC_high;
			delay_counter = (delay_counter + 1) % effect_modifier;
			// make a signed 16b value
		    input = (((delay_buffer[delay_counter] << 8) | ADC_low) + 0x8000) +
				    				   (((ADC_high << 8) | ADC_low) + 0x8000); 
			break;
		case 5:
			// distortion
			if (input > effect_modifier) input = effect_modifier;
			break;
		case 6:
			// fuzz
			if(input > effect_modifier) 
				input=32768;
		    else if(input < -effect_modifier) 
				input=-32768;
			break;
		case 7:
			// tremolo
			divider++;
			if (divider == 4)
			{
				divider = 0;
				sample = (sample + effect_modifier) % 1024;

				// the input signal is modulated here using a sinewave.
				// this is not optimized: double assignation for input and adc
				// variables
				ADC_low = map(ADC_low, 0,255, 0, waveform[sample]);
				ADC_high = map(ADC_high, 0,255, 0, waveform[sample]);
				input = ((ADC_high << 8) | ADC_low) + 0x8000; // make a signed 16b value
			}
			break;
		default:
			// default to clean
			// no-op
			break;
	}

	//write the PWM signal
	OCR1AL = ((input + 0x8000) >> 8); // convert to unsigned, send out high byte
	OCR1BL = input; // send out low byte
}
