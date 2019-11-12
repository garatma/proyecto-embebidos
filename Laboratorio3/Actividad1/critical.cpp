#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile int16_t anidamiento;

void critical_begin(void)
{
    // Verificar que las interrupciones no fueron interrumpidas por fuera del
    // módulo de región crítica. Esto puede suceder cuando se está ejecutando
    // una rutina de interrupción. En este caso, no deshabilitar las
    // interrupciones

    // Lectura atómica del registro
    uint8_t status_register = SREG;
    cli();

    // Si las interrupciones estaban habilitadas, y aún no fueron
    // deshabilitadas por el módulo de región crítica
    // o están deshabilitadas pero por el módulo de región crítica
    //       incrementar el anidamiento.
    if ((anidamiento == 0) && ((status_register & (0x80)) != 0)
            || (anidamiento != 0) && ((status_register & (0x80)) == 0))
    {
        anidamiento++;
    }
    // Si las interrupciones estaban deshabilitadas, y no fué el módulo de
	// región crítica, no hacer nada.
}

void critical_end(void)
{
    uint8_t status_register = SREG;
    cli();

    if (anidamiento > 0) {
        anidamiento--;
        if (anidamiento == 0)
            sei();
    }
    else if ((status_register & 0x80) != 0)
        sei();
}
