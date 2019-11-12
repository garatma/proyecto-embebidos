#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "fnqueue.h"
#include "teclado.h"

volatile struct estructura_teclado estado_teclado_var;
struct adc_cfg adc0;

void key_up_callback(void (*handler)(), int16_t tecla)
{
	estado_teclado_var.handlers_up[tecla] = handler;
}

void key_down_callback(void (*handler)(), int16_t tecla)
{
	estado_teclado_var.handlers_down[tecla] = handler;
}

// Convert ADC value to key number
int16_t get_key(int16_t input)
{
	int8_t key = -1;
	if ( 0 < input && input < 100 )
		key = 3;
	else if ( 100 <= input && input < 220 )
	   	key = 0;
	else if ( 220 <= input && input < 360 )
	   	key = 1;
	else if ( 360 <= input && input < 535 )
	   	key = 2;
	else if ( 535 <= input && input < 760 )
	   	key = 4;
	return key;
}

void procesar_entrada(int16_t valor)
{
	int8_t key;
	key = get_key(valor);
    if ( 0 <= key && key <= 4 )
    {
		if ( estado_teclado_var.handlers_down != -1 )
			estado_teclado_var.handlers_down[key]();
		estado_teclado_var.last_key = key;
    }
    else
	{
		if ( estado_teclado_var.handlers_down != -1 )
		{
			if ( estado_teclado_var.last_key != -1 )
				estado_teclado_var.handlers_up[estado_teclado_var.last_key]();
			estado_teclado_var.last_key = -1;
		}
	}
}

void teclado_init()
{
	estado_teclado_var.last_key = -1;  // no se apretó ninguna tecla
	for ( int i = 0; i < 5; i++ )
	{
		estado_teclado_var.handlers_up[i] = -1;
		estado_teclado_var.handlers_down[i] = -1;
	}

	adc0.canal = 0;
	adc0.callback = &procesar_entrada;
	adc_init(&adc0);
}
