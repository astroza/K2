/* 2006/12/28 ElectroLinux
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

/* Constants */
#define STX 		0x02 	/* Start of text */
#define EOT		0x04	/* End of text */

/* bitmasks for HDB2 */
#define DD_MASK		0xc0	/* 1100 0000 */
#define SS_MASK		0x30    /* 0011 0000 */
#define PP_MASK 	0x0c    /* 0000 1100 */
#define AA_MASK 	0x03    /* 0000 0011 */

/* bitmasks for HDB1 */
#define C_MASK		0x80	/* 1000 0000 */
#define A_MASK		0x40    /* 0100 0000 */
#define E_MASK		0x20	/* 0010 0000 */
#define NNNNN_MASK	0x1f    /* 0001 1111 */

/* Definiciones para trabajar con la estructura frame y su cabezera.
 */

#define DADDR_SIZE(a) 	(((a)->hd[B2] & DD_MASK) >> 6)
#define SADDR_SIZE(a) 	(((a)->hd[B2] & SS_MASK) >> 4)
#define PP(a) 		(((a)->hd[B2] & PP_MASK) >> 2)
#define AA(a) 		(((a)->hd[B2] & AA_MASK))
#define SET_DD(a, b) 	((a)->hd[B2] |= (((b) & 0x3) << 6))
#define SET_SS(a, b) 	((a)->hd[B2] |= (((b) & 0x3) << 4))
#define SET_AA(a, b) 	((a)->hd[B2] |= ((b) & 0x3))

#define C(a) 		((a)->hd[B1] & C_MASK) /* Cuando un campo es de un solo bit, no nos importa su cantidad, puede ser 0 o mayor a 0, por eso no hay shift right */
#define A(a)		((a)->hd[B1] & A_MASK)
#define E(a) 		((a)->hd[B1] & E_MASK)
#define NNNNN(a) 	((a)->hd[B1] & NNNNN_MASK)
#define SET_C(a) 	((a)->hd[B1] |= C_MASK)
#define SET_A(a)	((a)->hd[B1] |= A_MASK)
#define SET_E(a) 	((a)->hd[B1] |= E_MASK)
#define SET_NNNNN(a, b)	((a)->hd[B1] |= (b & NNNNN_MASK))

#define ADDR_LEN	3
#define PADDR(a) 	uint8_t (a)[ADDR_LEN]
typedef uint8_t *addr_t;

struct frame {
	uint8_t stx;
	uint8_t hd[2];
	uint8_t ahead[];
} __attribute__ ((packed));

/* Calcula tamaño de un frame leyendo cabezera */
#define FRM_SIZE(a) (DADDR_SIZE((a)) + SADDR_SIZE((a)) + PP((a)) + NNNNN((a)) + E((a)) + 4)
/* Un no-pecador hack */
#define CLR_HEADER(a) *(uint16_t *)&((a)->hd[B2]) = 0

#ifndef SUPPORTED_NNNNN
#define SUPPORTED_NNNNN 31
#endif

/* Un buffer para una trama uCmp */
union ucmp_buffer
{
	struct frame as_frame;
	/* 3: [stx, hd2, hd1], 9: [da, sa, flags], data, 2: [crc8, eot] */
	uint8_t as_array[3 + 9 + SUPPORTED_NNNNN + 2];
};

/* struct private_address: Estructura utilizada para describir una direccion de red dentro del codigo
 */
struct private_address {

	PADDR(pa_addr);
	uint8_t pa_size;
};

#if ADDR_LEN == 3
	#define SET_PA(pa, a, b, c) (pa)->pa_addr[0] = (a); (pa)->pa_addr[1] = (b); (pa)->pa_addr[2] = (c)
#else 
	#if ADDR_LEN == 2
		#define SET_PA(pa, a, b) (pa)->pa_addr[0] = (a); (pa)->pa_addr[1] = (b)
	#else
		#if ADDR_LEN == 1
			#define SET_PA(pa, a) (pa)->pa_addr[0] = (a)
		#endif
	#endif
#endif

#define SET_PASZ(pa, sz) (pa)->pa_size = (sz)

#define B2 		0x0
#define B1 		0x1

#define ACK_TIMEOUT	50000	/* medio segundo en unidades de HAL ticks */
/* Para AA bits */
#define RACK 		0x1 	/* Requiere ACK */
#define ACK 		0x2 	/* Acknowledged */
#define NAK 		0x3 	/* Not Acknowledged */

#define GET_EDM(a) 	E((a)) 	/* Error detection method */

#define NOMETHOD	0x0
#define CRC8		0x1

#define CHECK_INTEGRITY	0x1

/* Numero de veces que reintenta enviar un frame */
#define RETRY_MAX	3

typedef void (*func_t)(uint8_t *, struct private_address *, uint8_t);

#define GET_SADDR(a, b) __GET_ADDR((a), (b), 1)
#define GET_DADDR(a, b) __GET_ADDR((a), (b), 0)

/* Compara una direccion (a) con la direccion de destino del frame (b)
*/
#define CMP_DADDR(a, b) (cmp_addr((a), (b), 0))

/* Compara una direccion (a) con la direccion de origen del frame (b)
*/
#define CMP_SADDR(a, b) (cmp_addr((a), (b), 1))

/* Si el tamaño de Destination Address es 0, se trata de una direccion 0x0, que equivale a BROADCAST
 */ 
#define TO_BROADCAST(a) (DADDR_SIZE((a)) == 0)

void ucmp_init(addr_t, func_t);
void SET_ADDR(struct frame *, struct private_address *, struct private_address *);
struct private_address *GET_NADDR();
uint8_t ucmp_send(struct frame *);
void inverse_addresses(struct frame *, struct frame *);
void ucmp_buffer_digest_data(union ucmp_buffer *, int8_t *, uint8_t, uint8_t);

void __GET_ADDR(struct private_address *, struct frame *, uint8_t);


#endif

