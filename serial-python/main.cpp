#include "Arduino.h"
#define LED 13

int entrada = -1;

void setup()
{
	Serial.begin(9600);
	pinMode(LED, OUTPUT);
}

void loop()
{
	if (Serial.available() > 0)
	{
		entrada = Serial.read();
		Serial.print("recibí un ");
		Serial.println(entrada-'0');
	}
	entrada -= '0';
	if (entrada == 1)
		digitalWrite(LED,HIGH);
	else if (entrada == 0)
		digitalWrite(LED,LOW);
}
