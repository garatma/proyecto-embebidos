#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "fnqueue.h"

struct adc_cfg adc[CANTIDAD_CANALES_ADC];
volatile int8_t canal_actual;
bool inicializado[CANTIDAD_CANALES_ADC] = {false, false,
	 									   false, false,
										   false, false};
int8_t primer_canal;
bool primera_inicializacion = true;

void cambiar_canal(int8_t canal)
{
	// deshabilitar interrupciones?
	switch (canal) {
		case 0:
			ADMUX &= ~(1 << MUX2) & ~(1 << MUX1) & ~(1 << MUX0);	// 000
			break;
		case 1:
			ADMUX &= ~(1 << MUX2) & ~(1 << MUX1); // 00x
			ADMUX |= (1 << MUX0);	// 001
			break;
		case 2:
			ADMUX &= ~(1 << MUX2) & ~(1 << MUX0);	// 0x0
			ADMUX |= (1 << MUX1);	// 010
			break;
		case 3:
			ADMUX &= ~(1 << MUX2);	// 0xx
			ADMUX |= (1 << MUX1) | (1 << MUX0);	// 011
			break;
		case 4:
			ADMUX &= ~(1 << MUX1) & ~(1 << MUX0);	// x00
			ADMUX |= (1 << MUX2);	// 100
			break;
		case 5:
			ADMUX &= ~(1 << MUX1);	// x0x
			ADMUX |= (1 << MUX2) | (1 << MUX0);	// 101
			break;
	}
}

bool adc_init(struct adc_cfg * cfg)
{
	int8_t canal = cfg -> canal;
	bool error = false;
	if ( -1 < canal && canal < CANTIDAD_CANALES_ADC )
	{
	    adc[canal] = *cfg;
	    adc[canal].activo = true;
		inicializado[canal] = true;
		adc[canal].hay_conversion=false;

	    // si no hay conversion iniciar
	    if ( primera_inicializacion )
	    {
	    	canal_actual = canal;
			primer_canal = canal;
	        primera_inicializacion = false;
			cambiar_canal(canal_actual);
			// configuración báisca y común para todos los canales.
			ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
			ADMUX |= (1 << REFS0);	// voltaje referencia.
			// ADMUX &= ~(1 << ADLAR); // left aligned (sheet: 24.9.3.1/2).
			ADCSRA |= (1 << ADEN);  // enable ADC.
			ADCSRA |= (1 << ADIE);  // habilitar interrupciones.
			ADCSRA |= (1 << ADSC);	// iniciar conversión.
	    }
	}
	else
		error = true;
	return error;
}

// retorna la posición donde está el canal inicializado entre inicio y fin, si
// es que lo encuentra.
int8_t obtener_proximo()
{
	int8_t i=canal_actual+1;
	bool encontre=false;
	while (i<CANTIDAD_CANALES_ADC && !encontre)
	{
		encontre = inicializado[i]==true;
		i++;
	}
	if(!encontre)
	{
		i=0;
		while (i<=canal_actual && !encontre)
		{
			encontre = inicializado[i]==true;
			i++;
		}
	}
	return i-1;
}

void procesar()
{
	cli();
	int8_t canal = canal_actual;
	int16_t valor_aux = adc[canal].valor;
	canal_actual = obtener_proximo();
	sei();
	cambiar_canal(canal_actual);
	ADCSRA |= (1 << ADSC);	// iniciar conversión.
	adc[canal].callback(valor_aux);
}

ISR(ADC_vect)
{
	adc[canal_actual].valor = ADC;
	fnqueue_add(procesar);
}
