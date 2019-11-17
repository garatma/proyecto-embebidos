#include "Arduino.h"

int entrada = -1;

void setup()
{
	Serial.begin(9600);
}

void loop()
{
	if (Serial.available() > 0)
	{
		entrada = Serial.read();
		if ( entrada == 'e' )
		{
			if (Serial.available() > 0)
				entrada = Serial.read();
			Serial.print("recibí el efecto ");
			Serial.print(entrada);
		}
		else
		{
			Serial.print("recibí el valor: ");
			Serial.print(entrada);
		}
	}
}
