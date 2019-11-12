#include <LiquidCrystal.h>
#include "Arduino.h"
#include "adc.h"
#include "fnqueue.h"
#include "teclado.h"
#define MAX_MUESTRAS 100

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int8_t indice = 0,
	   modo = 0,
	   cantidad_censados = 0;

float min = 200,
	  max = 0,
	  prom = 0,
	  suma = 0,
	  actual = 0;
float muestras[100];

volatile int8_t ciclos = 0;
volatile bool leer_temperatura = false;

void pasar_a_max();
void pasar_a_min();
void pasar_a_prom();
void pasar_a_actual();

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
	}
	else
		sei();
}

void imprimir()
{
	lcd.clear();
	switch (modo)
	{
		case 0:
			lcd.print("Actual: ");
			lcd.print(actual);
			break;
		case 1:
			lcd.print("Maxima: ");
			lcd.print(max);
			break;
		case 2:
			lcd.print("Minima: ");
			lcd.print(min);
			break;
		case 3:
			lcd.print("Promedio: ");
			lcd.print(prom);
			break;
	}
	lcd.print(" C");
}

ISR(TIMER2_OVF_vect)
{
	ciclos++;
	if ( ciclos == 9 )
	{
		fnqueue_add(imprimir);
		// pasaron 0.15ms
		ciclos = 0;
		leer_temperatura = true;
	}
}

void cambiar_modo(void (* down)(), void (* up)())
{
	key_up_callback(down, TECLA_DOWN);
	key_up_callback(up, TECLA_UP);
}

void pasar_a_max()
{
	modo = 1;
	cambiar_modo(&pasar_a_actual, &pasar_a_min);
}

void pasar_a_min()
{
	modo = 2;
	cambiar_modo(&pasar_a_max, &pasar_a_prom);
}

void pasar_a_prom()
{
	modo = 3;
	cambiar_modo(&pasar_a_min, &pasar_a_actual);
}

void pasar_a_actual()
{
	modo = 0;
	cambiar_modo(&pasar_a_prom, &pasar_a_max);
}

void nada()
{
}

void callback_init()
{
	key_down_callback(&nada, TECLA_UP);
	key_down_callback(&nada, TECLA_DOWN);
	key_down_callback(&nada, TECLA_SELECT);
	key_down_callback(&nada, TECLA_RIGHT);
	key_down_callback(&nada, TECLA_LEFT);

	key_up_callback(&nada, TECLA_SELECT);
	key_up_callback(&nada, TECLA_RIGHT);
	key_up_callback(&nada, TECLA_LEFT);


	key_up_callback(&pasar_a_max, TECLA_UP);
	key_up_callback(&pasar_a_prom, TECLA_DOWN);
}

int main()
{
	init();
	pinMode(10, OUTPUT);

	analogWrite(10, 180);
	lcd.begin(16,2);
	lcd.setCursor(0,0);

	for ( int8_t i = 0; i < MAX_MUESTRAS; i++ )
		muestras[i] = 0;

	fnqueue_init();
	teclado_init();
	callback_init();

	struct adc_cfg adc1;
	adc1.canal = 1;
	adc1.callback = &convertir_entrada_a_temperatura;
	adc_init(&adc1);

	TCCR2A = 0;
	TCCR2B = 0;
	TIMSK2 |= (1 << TOIE2);
	TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

	while (1)
		fnqueue_run();
	return 0;
}
