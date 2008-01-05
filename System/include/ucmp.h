/* 2006 - 2007 ElectroLinux
 * Felipe H. Astroza A.
 * Curicó
 * file: ucmp.h
 */

#ifndef __UCMP_H_
#define __UCMP_H_

#include <hal_type.h>

/* Interface */
#define OK 		0x0
#define ERROR		0x1

#define READY		0x1
#define IN_PROGRESS	0x0

/* Constants */
#define STX 		0x02 	/* Start of text */
#define EOT		0x04	/* End of text */

/* Macros para trabajar con la estructura frame y su cabecera.
 */

#define DADDR_SIZE(a) 	(a)->hd2.dd
#define SADDR_SIZE(a) 	(a)->hd2.ss
#define PP(a) 		(a)->hd2.pp
#define AA(a) 		(a)->hd2.aa
#define SET_DD(a, b) 	( DADDR_SIZE(a) = (b) )
#define SET_SS(a, b) 	( SADDR_SIZE(a) = (b) )
#define SET_PP(a, b)	( PP(a) = (b) )
#define SET_AA(a, b) 	( AA(a) = (b) )

#define C(a) 		(a)->hd1.c
#define A(a)		(a)->hd1.a
#define E(a) 		(a)->hd1.e
#define NNNNN(a) 	(a)->hd1.nnnnn
#define SET_C(a) 	( C(a) = 1 )
#define SET_A(a)	( A(a) = 1 )
#define SET_E(a) 	( E(a) = 1 )
#define SET_NNNNN(a, b)	( NNNNN(a) = (b) )

struct frame {
	uint8_t stx;

	struct {
		uint8_t aa:2;
		uint8_t pp:2;
		uint8_t ss:2;
		uint8_t dd:2;
	} hd2;

	struct {
		uint8_t nnnnn:5;
		uint8_t e:1;
		uint8_t a:1;
		uint8_t c:1;
	} hd1;

	uint8_t ahead[];
} __attribute__ ((packed));

/* Esta estructura me permite acceder a HD2 y HD1 como un entero de 16 bits */
struct __frame {
	uint8_t stx;
	uint16_t hd;
} __attribute__ ((packed));

/* Calcula tama�o de un frame */
#define FRM_SIZE(a) (DADDR_SIZE((a)) + SADDR_SIZE((a)) + PP((a)) + NNNNN((a)) + E((a)) + 4)

#define CLR_HEADER(a) ((struct __frame *)(a))->hd = 0

#ifndef SUPPORTED_NNNNN
#define SUPPORTED_NNNNN 31
#endif

/* Un buffer para una trama uCmp */
union ucmp_buffer {
	struct frame as_frame;
	/* 3: [stx, hd2, hd1], 9: [da, sa, flags], data, 2: [crc8, eot] */
	uint8_t as_bytes[3 + 9 + SUPPORTED_NNNNN + 2];
};

#define ADDR_SIZE 	3

/* struct private_address: Descriptora de una direccion uCmp */
struct private_address {
	uint8_t pa_addr[ADDR_SIZE];
	uint8_t pa_size;
};

/* La rutina de usuario accede al contenido y parametros de un mensaje mediante
 * esta estructura
 */
struct ucmp_message {
	/* Contenido de mensaje */
	uint8_t *content;

	/* Tamaño del mensaje */
	uint8_t size:5;

	/* 0 para broadcast, 1 para nodo */
	uint8_t dst:1;
#define TO_BROADCAST 0
#define TO_NODE 1

	uint8_t reserved:2;

	/* Direccion de origen */
	struct private_address src;
};

typedef void (*func_t)(struct ucmp_message *);

#define SET_PA(pa, a, b, c) (pa)->pa_addr[0] = (a); (pa)->pa_addr[1] = (b); (pa)->pa_addr[2] = (c)
#define SET_PASZ(pa, sz) (pa)->pa_size = (sz)

#define ACK_TIMEOUT	50000	/* medio segundo en unidades de HAL ticks */
/* Para AA bits */
#define RACK 		0x1 	/* Requiere ACK */
#define ACK 		0x2 	/* Acknowledged */
#define NAK 		0x3 	/* Not Acknowledged */
#define NOMETHOD	0x0
#define CRC8		0x1
#define CHECK_INTEGRITY	0x1

/* Numero de veces que reintenta enviar un frame */
#define RETRY_MAX	3

#define GET_SADDR(a, b) __GET_ADDR((a), (b), 1)
#define GET_DADDR(a, b) __GET_ADDR((a), (b), 0)

/* Compara una direccion (a) con la direccion de destino del frame (b)
*/
#define CMP_DADDR(a, b) (cmp_addr((a), (b), 0))

/* Compara una direccion (a) con la direccion de origen del frame (b)
*/
#define CMP_SADDR(a, b) (cmp_addr((a), (b), 1))

void ucmp_init(uint8_t *, func_t);
void SET_ADDR(struct frame *, struct private_address *, struct private_address *);
struct private_address *GET_LOCAL_ADDRESS();
uint8_t ucmp_send();
struct frame *ucmp_buffer_get();
void inverse_addresses(struct frame *, struct frame *);
void ucmp_buffer_digest_data(uint8_t *, uint8_t, uint8_t);

void __GET_ADDR(struct private_address *, struct frame *, uint8_t);

#endif
