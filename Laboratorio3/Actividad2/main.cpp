#include <LiquidCrystal.h>
#include "Arduino.h"
#include "adc.h"
#include "fnqueue.h"
#define MAX_MUESTRAS 60

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int8_t indice = 0,
	   modo = 1,
	   cantidad_censados = 0;

float max = 0,
	  min = 125,
	  prom = 0,
	  actual = 0,
	  suma = 0;

float muestras[MAX_MUESTRAS];

volatile int8_t ciclos = 0;
volatile bool leer_temperatura = false;

// cuentas de la conversión en archivo cuentas.txt
void convertir_entrada_a_temperatura(int16_t valor)
{
	cli();
	if ( leer_temperatura )
	{
		leer_temperatura = false;
		sei();
		actual = valor/2.048;
		if ( actual < min ) min = actual;
		else if ( actual > max ) max = actual;
		if ( cantidad_censados < MAX_MUESTRAS )
			cantidad_censados++;
		else
			suma -= muestras[indice];

		muestras[indice] = actual;
		suma += actual;
		prom = suma/cantidad_censados;

		indice++;
		if ( indice == MAX_MUESTRAS )
			indice = 0;
		if ( Serial.availableForWrite() > 0 )
			Serial.print((int)actual);
	}
	else
		sei();
}

void imprimir()
{
	lcd.clear();
	switch (modo)
	{
		case 1:
			lcd.print("Actual: ");
			lcd.print((int)actual);
			break;
		case 2:
			lcd.print("Maxima: ");
			lcd.print((int)max);
			break;
		case 3:
			lcd.print("Minima: ");
			lcd.print((int)min);
			break;
		default:
			lcd.print("Promedio: ");
			lcd.print((int)prom);
			break;
	}
	lcd.print(" C");
}

void serialEvent()
{
	if ( Serial.available() )
		modo = Serial.read() - 48;
}

ISR(TIMER2_OVF_vect)
{
	ciclos++;
	if ( ciclos == 62 )
	{
		fnqueue_add(imprimir);
		ciclos = 0;
		leer_temperatura = true;
	}
}

void setup()
{
	init();
	pinMode(10, OUTPUT);

	analogWrite(10, 180);
	lcd.begin(16,2);
	lcd.setCursor(0,0);

	for ( int8_t i = 0; i < MAX_MUESTRAS; i++ )
		muestras[i] = 0;

	fnqueue_init();

	struct adc_cfg adc1;
	adc1.canal = 1;
	adc1.callback = &convertir_entrada_a_temperatura;
	adc_init(&adc1);

	TCCR2A = 0;
	TCCR2B = 0;
	TIMSK2 |= (1 << TOIE2);
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	Serial.begin(9600);
}

void loop()
{
	fnqueue_run();
}
