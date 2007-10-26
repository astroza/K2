#ifndef HAL_TIMER
/* Cabecera siempre incluida por otros objetos */
#include <hal_type.h>

extern volatile uint32_t hal_timer_ticks;
void hal_timer_init();

#endif
