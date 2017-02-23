#ifndef PORT_H
#define PORT_H

#include "msg.h"

typedef struct {
	uint8_t busy;
	kpl_msg msg;
} kpl_port;

#endif
