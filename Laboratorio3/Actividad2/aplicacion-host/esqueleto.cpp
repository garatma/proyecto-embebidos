void convertir_volt_temp(int16_t valor)
{
	if ( leer_temperatura )
	{
		leer_temperatura = false;
		actual = (int8_t) valor/2.048;
		Serial.write(actual);
	}
}

void serialEvent()
{
	if ( Serial.available() > 0 )
		modo = Serial.read();
}

ISR(TIMER2_ovf_vect)
{
	ciclos++;
	if ( ciclos == 62 )
	{
		// interrumpir cada segundo
		ciclos = 0;
		leer_temperatura = true;
	}
}

int main()
{
	adc_init(1,leer_temperatura);
	while (1)
		fnqueue_run();	// leer la temperatura del adc
	return 0;
}
	
