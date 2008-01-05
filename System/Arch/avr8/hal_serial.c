/* Hardware Abstraction Layer - HAL
 * Felipe Astroza - 2007
 * file: hal_serial.c
 * Arch: AVR{ atmega8, atmega168 }
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <hal_type.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

static void rx_dummy(uint8_t);
static void (*hal_rx_cb)(uint8_t) = rx_dummy;

void hal_serial_init(uint32_t speed)
{
	sei();

/* Codigo de arduino - Temporal*/
#if defined(__AVR_ATmega168__)
	UBRR0H = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;
	UBRR0L = ((F_CPU / 16 + speed / 2) / speed - 1);
        // enable rx and tx
        sbi(UCSR0B, RXEN0);
        sbi(UCSR0B, TXEN0);

        // enable interrupt on complete reception of a byte
        sbi(UCSR0B, RXCIE0);
#else
	UBRRH = ((F_CPU / 16 + speed / 2) / speed - 1) >> 8;
	UBRRL = ((F_CPU / 16 + speed / 2) / speed - 1);

	// enable rx and tx
	sbi(UCSRB, RXEN);
        sbi(UCSRB, TXEN);

        // enable interrupt on complete reception of a byte
        sbi(UCSRB, RXCIE);

#endif
}

void hal_serial_rx_cb( void (*cb)(uint8_t) )
{
	hal_rx_cb = cb;
}

static void serial_putc(uint8_t c)
{
#if defined(__AVR_ATmega168__)
	while (!(UCSR0A & (1 << UDRE0)));
        UDR0 = c;
#else
	while (!(UCSRA & (1 << UDRE)));
	UDR = c;
#endif
}

void hal_serial_write(uint8_t *buf, uint16_t size)
{
	uint16_t i;
	for(i = 0; i < size; i++)
		serial_putc(buf[i]);
}

static void rx_dummy(uint8_t c)
{
	return;
}

#if defined(__AVR_ATmega168__)
ISR(USART_RX_vect)
#else
ISR(USART_RXC_vect)
#endif
{
#if defined(__AVR_ATmega168__)
	unsigned char c = UDR0;
#else
	unsigned char c = UDR;
#endif
	hal_rx_cb(c);
}
