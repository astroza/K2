#ifndef MSG_H
#define MSG_H
#include <stdint.h>

typedef struct {
	int8_t src;
	int8_t dst;
	uint8_t data;
} kpl_msg;

uint8_t kpl_msg_send(int8_t dst, uint8_t data);
uint8_t kpl_msg_recv(kpl_msg **msg);
	
#endif
