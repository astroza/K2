/* 2007/05/25 ElectroLinux
 * Felipe H. Astroza A.
 * Curicó
 * file: Application/sim_test.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>

#include <default.h>

static struct frame *sys_frame;

/* Bits */
#define NEW_FRAME 0x1
#define SENSOR1 0x2 

extern int ftx_addr;
extern int sockfd;
extern uint8 event_bitmap;

static int l=1;
static void internal_event_poll(void *);

static void send_notice()
{
	uint8 buf[15];
	struct private_address pa, *n_pa;
	struct frame *frm = (struct frame *)buf;

	CLR_HEADER(frm);
	pa.pa_addr[0]=0;
	pa.pa_addr[1]=0;
	pa.pa_addr[2]=2;
	pa.pa_size = 1;
	n_pa = GET_NADDR();
	SET_ADDR(frm, &pa, n_pa);
	SET_AA(frm, RACK);
	SET_EEE(frm, CRC8);
	SET_NNNN(frm, 0x4); /* 4 Bytes */
	memcpy(buf + 3 + n_pa->pa_size + pa.pa_size, "ORIX\x0", 5); /* Mensaje y pseudo CRC8 valor 0 */
	frm->stx = STX;

	fprintf(stdout, "send_notice(): Frame %s\n", send_frame(frm)? "perdido" : "recibido");
}

static void userfunction(struct frame *frm)
{
	struct private_address pa;

	GET_SADDR(&pa, frm);
	printf("frame from: %d.%d.%d\n", pa.pa_addr[0], pa.pa_addr[1], pa.pa_addr[2]);
}
 
int main(int c, char **v)
{
	PADDR(be_addr); /* Big endian */
	pthread_attr_t attr;
	pthread_t tid;
	char *e;
	unsigned int le_addr; /* Little endian */
	if(c < 2)
		return 0;

	le_addr = strtoul(v[1], &e, 16);
	if(*e != 0) {
		fprintf(stdout, "Por favor ingrese direccion valida\n");
		return 0;
	}

	sys_frame = calloc(1, FRAMEMAXSIZE);
	if(sys_frame == NULL) {
		perror("calloc");
		exit(1);
	}

	connect_2_bus();
	if(sockfd == -1) {
		fprintf(stderr, "No puedo conectarme al BUS de simulador\n");
		exit(1);
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&tid, &attr, (void *)internal_event_poll, NULL) == -1) {
		fprintf(stderr, "ERROR CRITICO: No puedo crear THREAD\n");
		exit(1);
	}

	be_addr[0] = ((unsigned char *)&le_addr)[2];
	be_addr[1] = ((unsigned char *)&le_addr)[1];
	be_addr[2] = ((unsigned char *)&le_addr)[0];
	system_initialize(be_addr, sys_frame, userfunction);

	/* Si 2^x=2 entonces x=1 */
	set_event(1, &send_notice);

	fprintf(stdout, "Address: %d.%d.%d\n", be_addr[0], be_addr[1], be_addr[2]);
	fprintf(stdout, "Demo:\n* Con 2 bytes de entrada en STDIN, envia un frame que requiere ACK a 0.0.2\n");

	while(l)
		waiting_event();

	free(sys_frame);
	return 0;
}

#define futex(a, b, c, d, e, f) syscall(SYS_futex, (a), (b), (c), (d), (e), (f))

static void internal_event_poll(void *unused)
{
	fd_set rds;
	char b[2];

	while(l) {
		FD_ZERO(&rds);
		FD_SET(0, &rds);
		FD_SET(sockfd, &rds);

		/* No tengo timeout */
		select(sockfd+1, &rds, NULL, NULL, NULL);

		if(FD_ISSET(0, &rds)) {
			read(0, b, 2);
			fprintf(stdout, "b=\"%c\"\n", b[0]);
			/* SENSOR 1 !!!! un ladron en casa!! */
			event_bitmap |= SENSOR1;
		}

		if(FD_ISSET(sockfd, &rds)) {
			if(recv(sockfd, (void *)sys_frame, FRAMEMAXSIZE, 0) < 1) {
				l = 0;
				fprintf(stderr, "Error en BUS de simulador\n");
			}
			event_bitmap |= NEW_FRAME;
		}

		if(event_bitmap)
			futex(&ftx_addr, FUTEX_WAKE, 0x2, NULL, NULL, 0);
	}

	pthread_exit(0);
}
