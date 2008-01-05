/* Hardware Abstraction Layer - HAL
 * Felipe Astroza - 2007
 * file: hal_timer.c
 * Arch: AVR { atmega8, atmega168 }
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <hal_type.h>

/* Nuestro hal_timer_ticks aumenta cada 10 usec, tiempo apropiado para escalar */
volatile uint32_t hal_timer_ticks = 0;

void hal_timer_init()
{
#if defined(__AVR_ATmega168__)
	TCCR0B = (0 << CS02) | (0 <<CS01) | (1 << CS00); /* Prescaler clk/1 */
	TIMSK0 |= (1 << TOIE0);
#else
	TCCR0 = (0 << CS02) | (0 <<CS01) | (1 << CS00); /* Prescaler clk/1 */
	TIMSK |= (1 << TOIE0);
#endif
	TCNT0 = 96;
}

ISR(TIMER0_OVF_vect)
{

		    /* T_base * (256 - TCNT0) = tiempo_de_ticks
		     * (1/(frecuencia/prescaler)) * (256 - TCNT0) = tiempo_de_ticks
		     * (1/(16000000/1)) * (256 - 96) = 10usec
		     * 62.5ns * 160 = 10usec
		     */
	TCNT0 = 96;
	hal_timer_ticks++;
}
