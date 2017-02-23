#include <task.h>
#include <time.h>
#include <stdio.h>
	
time_t t1;
int8_t dst;

TASK(task0)
{
	TASK_PROGRAM {
		kpl_msg *msg;

		t1 = time(NULL);

		WAIT_UNTIL(kpl_msg_recv(&msg));
		printf("Llego mensaje desde %hhu data=%hhu\n", msg->src, msg->data);
		WAIT_UNTIL( (time(NULL) - t1) >= 5 );
	}
}

TASK(task1)
{
	TASK_PROGRAM {
		WAIT_UNTIL(kpl_msg_send(dst, 88));
		puts("Enviado");
	}
}

INIT(init0)
{
	dst = kpl_task_create(task0);
	kpl_task_create(task1);
}

INIT(init1)
{
	puts("Test");
}
