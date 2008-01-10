#include <ucmp.h>
#include <string.h>
#include <kepler.h>
#include <hal_timer.h>

#define FINGERPRINT 	1
#define TICKS		2

void ucmp_user_function(struct ucmp_message *msg)
{
	struct frame *frm;
	uint32_t fp;

	frm = ucmp_buffer_get();

	frm->stx = STX;
	CLR_HEADER(frm);
	SET_ADDR(frm, &msg->src, GET_LOCAL_ADDRESS());

	if(msg->size) {
		switch(msg->content[0]) {
			case FINGERPRINT:
				fp = 0xbeef;
				ucmp_buffer_digest_data((uint8_t *)&fp, 4, 0);
				ucmp_send();
				break;
			case TICKS:
				ucmp_buffer_digest_data((uint8_t *)&hal_timer_ticks, 4, 0);
				ucmp_send();
				break;
		}
	}
}

INIT(init_subsystem)
{
	uint8_t ucmp_local_addr[] = {0, 0, 2};

	ucmp_init(ucmp_local_addr, ucmp_user_function);
}
