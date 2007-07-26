/* Felipe Astroza
 * i386.h
 */

#ifndef __i386_H_
#define __i386_H_

#include <ucmp.h>

#define FRAMEMAXSIZE 527 /* Maximo tamaño de un frame en octetos */

#define CLR_HEADER(a) *(short *)&((a)->hd[B2]) = 0

uint16 calculate_size(struct frame *frm);
void do_send_frame(void *s, uint16 len);
void ack_wait();

#endif
