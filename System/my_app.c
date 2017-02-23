#include <ucmp.h>
#include <string.h>
#include <kepler.h>
#include <hal_timer.h>

void ucmp_user_function(struct ucmp_message *msg)
{
	return;
}

KINIT(init_subsystem)
{
	uint8_t ucmp_local_addr[] = {0, 0, 2};

	ucmp_init(ucmp_local_addr, ucmp_user_function);
}

