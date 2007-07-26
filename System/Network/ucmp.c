/* 2006/12/28 ElectroLinux
 * Felipe Astroza
 * CuricÃ³
 * file: ucmp.c
 * UCMP <=> Microcontroller Multipoint Protocol
 */

#include <default.h>

/* Mascaras para sys_bits */
#define STATUS_MASK 0x3
#define ACKNAK_MASK 0x4
#define ACK_WAIT_MASK 0x1

/* Para verificar estados */
#define STATUS (ucmp.sys_bits & STATUS_MASK)

/* El bit ACKNAK tiene una doble vida. Puede indicar si el mensaje de notificacion es ack o nak
 * o puede indicar que llego un ACK esperado.
 */
#define ACKNAK (ucmp.sys_bits & ACKNAK_MASK)
#define ACK_IS_PRESENT ucmp.sys_bits |= ACKNAK_MASK
#define NAK_IS_PRESENT ucmp.sys_bits &= ~ACKNAK_MASK
#define ACK_WAIT_IS_PRESENT (ucmp.sys_bits & ACK_WAIT_MASK)

/* Activar y desactivar ACK_WAIT */
#define ACTIVATE_ACK_WAIT ucmp.sys_bits |= ACK_WAIT_MASK
#define DEACTIVATE_ACK_WAIT ucmp.sys_bits &= ~ACK_WAIT_MASK

/* Reglas:
 * 1: Cuando se comparan 2 direcciones (big-endian) siempre se debe comenzar por el byte menos
 * significativo.
 */

/* Estructura principal */
static struct
{
	/* Direccion de nodo */
	struct private_address sys_addr;

	/* bits 0-1: Status, 2-2: ack/nak, 3-7: unused */
	uint8 sys_bits;

	/* Usado para almacenar frame entrante */
	struct frame *sys_frame;

	/* Espero un ack de: */
	struct private_address sys_ackfrom;

	func_t sys_frameto[2];
#define to_broadcast sys_frameto[1]
#define to_node	sys_frameto[0]
} ucmp;

/* __GET_ADDR(): Fue traido la seccion independiente, ahora el codigo esta orientado a 8 bits
 * Las direcciones son Big-endian 
 */
void __GET_ADDR(struct private_address *pa, struct frame *frm, uint8 sd)
{
	uint8 off = 0;
	int8 i = ADDR_LEN - 1, j = DADDR_SIZE(frm);

	/* Necesito la direccion de origen? */
	if(sd) {
		off += j;
		j += (pa->pa_size = SADDR_SIZE(frm));
	} else
		pa->pa_size = j;

	j--; /* to index */

	while(i >= 0)
		pa->pa_addr[(uint8)i--] = j >= off? frm->ahead[(uint8)j--] : 0;
}

/* SET_ADDR(): Esta funcion ajusta la direccion de destino y origen de un frame.
 */
void SET_ADDR(struct frame *frm, struct private_address *pa0, struct private_address *pa1)
{
	int8 i, j = ADDR_LEN - 1;
	uint8 off = 0;

	i = pa0->pa_size - 1;
	while(i >= 0)
		frm->ahead[(uint8)i--] = pa0->pa_addr[(uint8)j--];

	off = pa0->pa_size;

	j = ADDR_LEN - 1;
	i = pa1->pa_size + off - 1;
	while(i >= off)
		frm->ahead[(uint8)i--] = pa1->pa_addr[(uint8)j--];

	SET_DD(frm, pa0->pa_size);
	SET_SS(frm, pa1->pa_size);
}

/* GET_NADDR(): Consigue nuestra direccion en una estructura private_address */
struct private_address *GET_NADDR()
{
	return &ucmp.sys_addr;
}

/* paddr_size(): Consigue el tamaño de una direccion.
 */
uint8 paddr_size(addr_t addr)
{
	uint8 i = 0;

	while(i < ADDR_LEN && addr[i] == 0)
		i++;

	return ADDR_LEN - i;
}

/*       /
 * f(x) | si x < 2 => 0
 *      | si x < 4 => 1
 *      | si x > 3 => 2^(x-3)
 *       \
 */
/* EEE_table(): Determina el tamaÃ±o de un detector de errores en un frame
 */
uint8 EEE_table(uint8 x)
{
	if(x < 2)
		return 0;
	else if(x < 4)
		return 1;

	return 1 << (x - 3);
}

/* system_initialize(): Iniciadora de sistema.
 */
void system_initialize(addr_t addr, struct frame *frm, func_t frm_to_node, func_t frm_to_broadcast)
{
	ucmp.sys_addr.pa_addr[0] = addr[0];
	ucmp.sys_addr.pa_addr[1] = addr[1];
	ucmp.sys_addr.pa_addr[2] = addr[2];
	ucmp.sys_addr.pa_size = paddr_size(ucmp.sys_addr.pa_addr);

	/* Segmento de memoria de FRAMEMAXSIZE bytes, utilizado para almacenar el frame entrante. */
	ucmp.sys_frame = frm;

	/* Funcion llamada luego de recibir un frame */
	ucmp.to_node = frm_to_node;
	ucmp.to_broadcast = frm_to_broadcast;
}

/* send_frame(): Envia frame y _SI_ lo necesita, espera un agradecimiento 
 */
uint8 send_frame(struct frame *frm)
{
	uint8 retry;
	frmlen size = calculate_size(frm);

	if(AA(frm) == RACK) {
		GET_DADDR(&ucmp.sys_ackfrom, frm);
		ucmp.sys_bits &= ~ACKNAK_MASK;
		/* En el caso de fallar reintenta RETRY_MAX-1 veces */
		ACTIVATE_ACK_WAIT;
		for(retry = 0; retry < RETRY_MAX; retry++) {
			do_send_frame(frm, size);
			ack_wait();
			if(ACKNAK) { /* 0x1 es ACK, 0x0 es NAK */
				DEACTIVATE_ACK_WAIT;
				return OK;
			}
		}
		DEACTIVATE_ACK_WAIT;
		return ERROR;
	}

	do_send_frame(frm, size);
	/* En este caso no me preocupa si llegÃ³ no  */
	return OK;
}

/* inverse_addr(): Copia direcciones entre 2 frames pero de forma inversa.
 * Esta rutina es una optimizacion para peticion/respuesta.
 * inversa:
 * frm0 -> destination = frm1 -> source
 * frm0 -> source = frm1 -> destination
 *
 * Para los que conocieron la implementacion anterior:
 * En el pasado previlegiaba la eficacia,
 * Logrando esta tarea con una PEQUEÃ‘A optimizacion que 
 * no se compara con la disminucion de TEXT al reutilizar
 * funciones ya definidas.
 */
void inverse_addr(struct frame *frm0, struct frame *frm1)
{
	struct private_address pa0, pa1;

	GET_DADDR(&pa0, frm1);
	GET_SADDR(&pa1, frm1);
	SET_ADDR(frm0, &pa1, &pa0);
}

/* __send_acknak(): Primero revisa si necesita notificar. Luego envia ack/nak segun corresponda.
 */
static void __send_acknak()
{
	uint8 frm[10];

	if(AA(ucmp.sys_frame) == RACK) {
		/* Me aseguro de que la cabezera este limpia */
		CLR_HEADER((struct frame *)frm);
		/* Â¿Es un ack o un nak?
		 * Consulta en bit ACKNAK de sys_bits
		 */
		((struct frame *)frm)->hd[B2] |=  ACKNAK? ACK : NAK;

		/* Finalmente envia el frame.
		 * Copio las direcciones desde frame de entrada a frame de salida, inversamente
		 */
		inverse_addr((struct frame *)frm, ucmp.sys_frame);
		frm[0] = STX; /* Equivalente a (struct frame *)frm->stx = STX */
		do_send_frame((struct frame *)frm, calculate_size((struct frame *)frm));
	}
}

/* Recuerden usar -O */
static inline void call_edm(struct frame *frm)
{
	switch(GET_EDM(frm)) {
		case RETRY:

		case CHKSM:

		case CRC8:

		case CRC16:

		case CRC32:

		default:
			/* TEST */
			ACK_IS_PRESENT;
			break;
	}
}

static uint8 cmp_addr(struct private_address *pa, struct frame *frm, uint8 s)
{
	uint8 size = DADDR_SIZE(frm), off = 0;
	int8 i, j;

	if(s) {
		off += size;
		size = SADDR_SIZE(frm);
	}

	if(size > ADDR_LEN || size != pa->pa_size) /* La direccion no debe sobrepasar lo soportado por la arquitectura */
		return 0;

	i = size + off - 1;
	j = ADDR_LEN - 1;

	while(i >= off)
		if(pa->pa_addr[(uint8)j--] != frm->ahead[(uint8)i--])
			return 0;

	return 1;
}


/* got_a_frame(): Rutina llamada luego de recibir un frame.
 * Es una pieza fundamental en esta implementaciÃ³n.
 */
void got_a_frame()
{
	uint8 ret;

	/* ¿Es para mi? */
	if(CMP_DADDR(&ucmp.sys_addr, ucmp.sys_frame)) {
		ret = AA(ucmp.sys_frame);
		if(ret > 0x1) { /* Relacionado con Metodos de Deteccion de Errores */
			/* ¿Estamos esperando ack?, 
			 * ¿Es de quien espero? y ¿Se trata de un agradecimiento? Si, activo el bit ACKNAK 
			 */
			if(ACK_WAIT_IS_PRESENT && ret == ACK && CMP_SADDR(&ucmp.sys_ackfrom, ucmp.sys_frame))
					ACK_IS_PRESENT;

			return;
		}

		/* ¿Estamos esperando ack? Si -> return
		 * Es imperativo recibir un "ack" antes del timeout 
		 */
		if(ACK_WAIT_IS_PRESENT)
			return;

		call_edm(ucmp.sys_frame);
		/* Envia ack/nak o nada segun corresponda */
		__send_acknak();
		ucmp.to_node(ucmp.sys_frame);
	} else 
	if(TO_BROADCAST(ucmp.sys_frame) && !ACK_WAIT_IS_PRESENT) {
		call_edm(ucmp.sys_frame); /* Trato de verificar su integridad */
		ucmp.to_broadcast(ucmp.sys_frame);
	}
	/* No me gusta leer correspondencia de otros :> */
}
