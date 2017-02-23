/* 2009/08/20 ElectroLinux
 * Felipe Astroza
 * Curicó
 * file: msg.c
 * Kepler messages
 */
#include <task.h>
#include <stdlib.h>

uint8_t kpl_msg_send(int8_t dst, uint8_t data)
{
	kpl_port *port = kpl_task_get_port(dst);

	if(!port || port->busy)
		return 0;

	port->msg.src = kpl_task_get_current();
	port->msg.dst = dst;
	port->msg.data = data;
	port->busy = 1;

	return 1;
}

uint8_t kpl_msg_recv(kpl_msg **msg)
{
	kpl_port *port = kpl_task_get_port( kpl_task_get_current() );

	if(!port->busy)
		return 0;

	port->busy = 0;
	*msg = &port->msg;
	return 1;
}
