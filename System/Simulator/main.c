/* 2006/12/28 ElectroLinux
 * Felipe Astroza
 * Curicó
 * file: Simulator/main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <mplx2.h>

#define UNIXDOMAIN "/tmp/ucmp_bus"
static struct mplx_handler mplxh;

#define BUFSIZE	529 /* Calculo que ese es el tamaño maximo de un frame */

static void disconnected(struct mplx_socket *cur)
{
	printf("Device(fd=%d) unplugged\n", cur->sockfd);
}

void print_addr(unsigned char *base, unsigned char size, unsigned char offset)
{
	int i, j = 2;
	unsigned char octs[] = {0, 0, 0};

	for(i = size - 1; i >= 0; i--, j--)
		octs[j] = base[offset+i];

	printf("%d.%d.%d", octs[0], octs[1], octs[2]);
}

void debug_frame(unsigned char *buf)
{
	unsigned char ret;

	fprintf(stdout, "[STX=0x%08x HDB2=0x%08x HDB1=0x%08x", buf[0], buf[1], buf[2]);
	fprintf(stdout, " FROM ");
	ret = (buf[1] & 0xc0) >> 6;
	print_addr(buf+3, (buf[1] & 0x30) >> 4, ret);
	fprintf(stdout, " TO ");
	print_addr(buf+3, ret, 0);

	ret = buf[1] & 0x03;
	switch(ret) {
		case 0x1:
			fprintf(stdout, " REQUIRE_ACK");
			break;
		case 0x2:
			fprintf(stdout, " ACK");
			break;
		case 0x3:
			fprintf(stdout, " NAK");
			break;
	}
	fprintf(stdout, "]\n");
	fflush(stdout);
}

static int reflect(struct mplx_list *list)
{
	unsigned char buf[BUFSIZE];
	int bytes;
	struct mplx_socket *socket;

	bytes = read(MPLX_CUR(list)->sockfd, (void *)buf, BUFSIZE);
	if(bytes <= 0)
		return 0;

	printf("[fd=%d] ", MPLX_CUR(list)->sockfd);
	debug_frame(buf);

	for(socket = list->head->next; socket != NULL; socket = socket->next) {
		if(socket == MPLX_CUR(list))
			continue;

		if(send(socket->sockfd, (void *)buf, bytes, 0) == -1) 
			perror("send");
		
	}

	return 1;
}

 /* Ocuparé un unix domain, los nodos se conectaran a el y este actuara como un bus multicast */
static int sim_accept(struct mplx_list *list)
{
	int clifd;
	struct sockaddr *sa;
	struct mplx_socket *cur = MPLX_CUR(list), *mplx;
	int sa_size;

	sa_size = cur->sa_size;

	sa = calloc(1, sa_size);
	clifd = accept(cur->sockfd, sa, (socklen_t *)&sa_size);
	if ( clifd == -1 )
		return -1;

	mplx = mplx_add_socket(list, clifd, (struct sockaddr *)sa, sa_size);

	mplx_set(mplx, MPLX_RECV_CALLBACK, (void *)&reflect);
	mplx_set(mplx, MPLX_DELETE_SOCK_CALLBACK, (void *)&disconnected);

        return 1;
}
	
static void init_bus()
{
	struct mplx_socket *socket;

	mplx_init(&mplxh, MPLX_USE_POLL, 5000);

	socket = mplx_listen_unix(&mplxh.list, UNIXDOMAIN);
	if(socket == NULL) {
		fprintf(stderr, "No puedo crear un unix domain\n");
		exit(EXIT_FAILURE);
	}

	mplx_set(socket, MPLX_RECV_CALLBACK, &sim_accept);
}

int main()
{
	printf("Simulador de protocolo rev2 para ElectroLinux\ninit BUS..");
	fflush(stdout);

	init_bus();

	printf(" OK.\n");
	fflush(stdout);

	mplx_loop(&mplxh);
}
