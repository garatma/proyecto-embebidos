#include <stdint.h>
#include <avr/io.h>
#include "Arduino.h"

int16_t valor = 0;
bool valor_nuevo = false;

ISR(ADC_vect)
{
	if (!valor_nuevo)
	{
		valor = ADC;
		valor_nuevo = true;
	}
}

int main()
{
	init();

	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX &= ~(1 << MUX2) & ~(1 << MUX1) & ~(1 << MUX0);
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADSC);

	Serial.begin(9600);

	while (1)
	{
		if (valor_nuevo)
		{
			Serial.println(valor);
			valor_nuevo = false;
		}
	}
	return 0;
}
