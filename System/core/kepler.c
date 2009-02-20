/*
 * Kepler ejecuta solo una vez las funciones de init_array y corre las tareas del vector task_array circularmente
 */
#include <stdint.h>
#include <kepler.h>

/* Las direcciones en AVR son de 16 bits */
extern uint16_t init_array_start[];
extern uint16_t init_array_stop[];

extern uint16_t task_array_start[];
extern uint16_t task_array_stop[];
/* *_start y *_stop: Son las direcciones del comienzo y final del segmento que contiene
 * punteros a funciones 
 */

/* Ejecuta todas las funciones de iniciacion */
void run_init()
{
	uint16_t *current = init_array_start;
	ktask next_init;

	do {
		next_init = (ktask)*current;
		current++;
		next_init();
	} while(current < init_array_stop);
}
	

int __attribute__((used)) main()
{
	uint16_t *current;
	ktask next_task;

	run_init();

	/* A hacer las tareas :D */
	while(1) {
		current = task_array_start;
		do {
			next_task = (ktask)*current;
			current++;
			next_task();
		} while(current < task_array_stop);
		/* Recuerden: las tareas no PUEDEN ser bloqueantes */
	}
}
