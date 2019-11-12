#include <stdbool.h>
#include <stdint.h>
#define CANTIDAD_CANALES_ADC 6

struct adc_cfg
{
	void (* callback)(int16_t);
	int8_t canal;
	uint16_t valor;
	bool activo;
	bool hay_conversion;
};

bool adc_init(struct adc_cfg * cfg);
