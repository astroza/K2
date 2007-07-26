/* 2006/12/28 ElectroLinux
 * Felipe H. Astroza A.
 * Curicó
 * file: ucmp.h
 */

#ifndef __UCMP_H_
#define __UCMP_H_

#include <types.h>

/* Interface */
#define OK 	0x0
#define ERROR	0x1

/* Constants */
#define STX 	0x02 	/* Start of text */
#define EOT	0x04	/* End of text */

/* bitmasks for HDB2 */
#define DD_MASK	0xc0
#define SS_MASK	0x30
#define PP_MASK 0x0c
#define AA_MASK 0x03

/* bitmasks for HDB1 */
#define C_MASK		0x80
#define EEE_MASK	0x70
#define NNNN_MASK	0x0f

/* Definiciones para trabajar con la estructura frame y su cabezera.
 */

#define DADDR_SIZE(a) (((a)->hd[B2] & DD_MASK) >> 6)
#define SADDR_SIZE(a) (((a)->hd[B2] & SS_MASK) >> 4)
#define PP_SIZE(a) (((a)->hd[B2] & PP_MASK) >> 2)
#define AA(a) (((a)->hd[B2] & AA_MASK))
#define SET_DD(a, b) ((a)->hd[B2] |= (((b) & 0x3) << 6))
#define SET_SS(a, b) ((a)->hd[B2] |= (((b) & 0x3) << 4))
#define SET_AA(a, b) ((a)->hd[B2] |= ((b) & 0x3))

#define C(a) ((a)->hd[B1] & C_MASK) >> 7)
#define EEE(a) (((a)->hd[B1] & EEE_MASK) >> 4)
#define NNNN(a) ((a)->hd[B1] & NNNN_MASK)
#define SET_C(a) ((a)->hd[B1] |= C_MASK)
#define SET_EEE(a, b) ((a)->hd[B1] |= (((b) & 0x7) << 4))
#define SET_NNNN(a, b) ((a)->hd[B1] |= (b & 0xf))

#define PADDR(a) uint8 (a)[ADDR_LEN]
typedef uint8 *addr_t;

struct frame {
	uint8 stx;
	uint8 hd[2];
	uint8 ahead[];
} __attribute__ ((packed));

/* struct private_address: Estructura utilizada para describir una direccion de red dentro del codigo
 */
struct private_address
{
	PADDR(pa_addr);
	uint8 pa_size;
};

#define B2 0x0
#define B1 0x1

#define ACK_TIMEOUT	100	/* milisegundos */
/* Para AA bits */
#define RACK 0x1 /* Requiere ACK */
#define ACK 0x2 /* Acknowledged */
#define NAK 0x3 /* Not Acknowledged */

#define GET_EDM(a) EEE((a)) /* Error detection method */

#define NOMETHOD	0x0
#define RETRY		0x1
#define CHKSM		0x2
#define CRC8		0x3
#define CRC16		0x4
#define	CRC32		0x5
/* 0x6 y 0x7 Aun no definido */


/* Numero de veces que reintenta enviar un frame */
#define RETRY_MAX	3

typedef void (*func_t)(struct frame *);

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

void system_initialize(uint8 *, struct frame *, func_t);
uint8 EEE_table(uint8);
void SET_ADDR(struct frame *, struct private_address *, struct private_address *);
struct private_address *GET_NADDR();
uint8 send_frame(struct frame *);
void got_a_frame();

#endif
