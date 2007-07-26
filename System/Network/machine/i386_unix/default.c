/* 2006/12/28 ElectroLinux
 * Felipe H. Astroza A.
 * Curicó
 * file: machine/i386_unix/default.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <default.h>

/* Todas las functiones o datos estatico son propias de la implementacion para la arquitectura.
 * El resto son rutinas necesarias para que el sistema funcione :
 * NNNN_table()
 * calculate_size()
 * do_send_frame()
 * waiting_event()
 * ack_wait()
 */

typedef void (*event_cb)();

uint8 event_bitmap = 0;
static event_cb event_table[sizeof(event_bitmap)*8] = {&got_a_frame, [1 ... (sizeof(event_bitmap)*8)-1] = NULL};

int ftx_addr = 0x2;
int sockfd = -1;

static struct timespec ts;

/* Datos necesarios de simulador */
static const char unixdomain[] = "/tmp/ucmp_bus";

/* Las siguientes funciones calculan los octetos que ocuparan
 * los bits de la HEADER en el frame
 ********************************/                     
 /* f(0) = 0          /
 * f(1) = 1     f(x) | si x < 9 => x
 * ...               | si x >= 9 => 2^(x-5)
 * f(9) = 16          \
 * f(10) = 32
 * ...
 * f(14) = 512
 */
uint16 NNNN_table(uint8 x)
{
	if(x < 9)
		return x;

	return 1 << (x - 5);
}
/*********************************/

/* calculate_size(): La siguiente rutina calcula el tamaño de un frame, excluyendo 
 * la base (STX, HDB1 y HDB2) del frame.
 */
inline uint16 calculate_size(struct frame *frm)
{
	return (NNNN_table(NNNN(frm)) + EEE_table(EEE(frm)) + DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP_SIZE(frm));
}

/* do_send_frame(): Hace efectivo el envio de 4 + len octetos 
 */
void do_send_frame(void *s, uint16 len)
{
	/* 4: STX, HDB2, HDB1 y EOT */
	if(send(sockfd, (void *)s, 4 + len, 0) == -1) {
		perror("send");
		exit(1);
	}
}

#define futex(a, b, c, d, e, f) syscall(SYS_futex, (a), (b), (c), (d), (e), (f))

/*  Este waiting_event(): Trato de emular el comportamiento de un uControlador, donde modulos
 * aparte generan las interrupciones.
 *  Cada vez que recibe un FUTEX_WAKE del otro thread revisa event_bitmap.
 */
void waiting_event()
{
	int i;

	futex(&ftx_addr, FUTEX_WAIT, 0x2, ts.tv_nsec ?&ts : NULL, NULL, 0);

	for(i=0; i < sizeof(event_bitmap)*8 && event_table[i] != NULL; i++)
		if(event_bitmap & (1 << i)) {
			event_bitmap &= ~(1 << i);
			event_table[i]();
		}
}

/* ack_wait(): Ajusta el tiempo de espera de waiting_event() y la llama.
 */
void ack_wait()
{
        ts.tv_nsec = ACK_TIMEOUT * 1000000;
        waiting_event();
        ts.tv_nsec = 0;
}

/* Funcion exclusiva de i386_unix
 * connect_2_bus(): Conecta al bus (UNIX DOMAIN)
 */
void connect_2_bus() 
{
	struct sockaddr_un sa;

	sockfd = socket(PF_UNIX, SOCK_STREAM, 0);
	if(sockfd != -1) {
		strcpy(sa.sun_path, unixdomain);
		sa.sun_family = AF_UNIX;

		if(connect(sockfd, (struct sockaddr *)&sa, sizeof(sa.sun_family) + sizeof(unixdomain)) == -1) {
			perror("connect");
			close(sockfd);
			sockfd = -1;
		}
	}
}

/* Function exclusiva de i386_unix
 * set_event(): Asocia un evento con una llamada
 */

/* Explico: Cada vez que hay un evento se debe refrescar event_bitmap,
 * Si un bit representa un evento, la ubicacion de este bit sirve
 * como indice para un conjunto. Cada elemento de este conjunto o arreglo es
 * una direccion de una funcion a ser llamada.
 */
void set_event(unsigned int i, event_cb ptr)
{
	if(i < sizeof(event_bitmap)*8)
		event_table[i] = ptr;
}
