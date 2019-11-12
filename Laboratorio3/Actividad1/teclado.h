#define TECLA_UP     	0 	// tecla up del LCD Keypad Shield.
#define TECLA_DOWN    	1   // tecla down del LCD Keypad Shield.
#define TECLA_LEFT    	2   // tecla left del LCD Keypad Shield.
#define TECLA_RIGHT   	3   // tecla right del LCD Keypad Shield.
#define TECLA_SELECT  	4 	// tecla select del LCD Keypad Shield.
#define CANTIDAD_TECLAS 5 	// cantidad de teclas en el teclado.

void key_down_callback(void (*handler)(), int16_t tecla);
void key_up_callback(void (*handler)(), int16_t tecla);
void teclado_init();

struct estructura_teclado {
	volatile int16_t last_key; // última tecla apretada.
	void (*handlers_up[CANTIDAD_TECLAS])(); // arreglo de handlrs para key_up.
	void (*handlers_down[CANTIDAD_TECLAS])();	// arreglo de handlrs para
                        					  	// key_down.
};
