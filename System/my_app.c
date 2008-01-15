#include <ucmp.h>
#include <string.h>
#include <kepler.h>
#include <hal_timer.h>

#define FINGERPRINT 	1
#define TICKS		2

void ucmp_user_function(struct ucmp_message *msg)
{
	uint8_t *buf;

	buf = ucmp_get_buffer(&msg->src, GET_LOCAL_ADDRESS(), 4);
	if(!buf)
		return;

	if(msg->size) {
		switch(msg->content[0]) {
			case FINGERPRINT:
				buf[0] = 0xff;
				buf[1] = 0xff;
				buf[2] = 0xff;
				buf[3] = 0xff;
				ucmp_send(RACK);
				break;
			case TICKS:
				memcpy((void *)buf, (void *)&hal_timer_ticks, 4);
				ucmp_send(RACK);
				break;
		}
	}
}

INIT(init_subsystem)
{
	uint8_t ucmp_local_addr[] = {0, 0, 2};

	ucmp_init(ucmp_local_addr, ucmp_user_function);
}
