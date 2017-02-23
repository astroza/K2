#ifndef HAL_SERIAL

#include <hal_type.h>

void hal_serial_init(uint32_t);
void hal_serial_rx_cb(void (*)(uint8_t));
void hal_serial_write(uint8_t *, uint16_t);

#endif
