/* 2006/12/28 ElectroLinux
 * Felipe Astroza
 * Curicó
 * file: ucmp.c
 * UCMP <=> Microcontroller Multipoint Protocol
 */

#include <ucmp.h>
#include <kepler.h>
#include <hal_serial.h>
#include <hal_timer.h>

#include <string.h>

/* Mascaras para sys_bits */
#define FRAME_IN_QUEUE_MASK 0x4
#define ACKNAK_MASK    0x2
#define ACK_WAIT_MASK  0x1

#define IS_FRAME_IN_QUEUE	(ucmp.sys_bits & FRAME_IN_QUEUE_MASK)
#define SET_FRAME_IN_QUEUE	ucmp.sys_bits |= FRAME_IN_QUEUE_MASK
#define CLEAR_FRAME_IN_QUEUE	ucmp.sys_bits &= ~FRAME_IN_QUEUE_MASK

#define CLEAR_ACKNAK 		ucmp.sys_bits &= ~ACKNAK_MASK
#define READ_ACKNAK 		(ucmp.sys_bits & ACKNAK_MASK)
#define SET_ACK			ucmp.sys_bits |= ACKNAK_MASK

#define ACK_WAIT_IS_PRESENT 	(ucmp.sys_bits & ACK_WAIT_MASK)
/* Activar y desactivar ACK_WAIT */
#define ACTIVATE_ACK_WAIT 	ucmp.sys_bits |= ACK_WAIT_MASK
#define DEACTIVATE_ACK_WAIT 	ucmp.sys_bits &= ~ACK_WAIT_MASK


/* Tiempo estimado para la transferencia de un octeto a SERIAL_SPEED baudios
 * Observacion: La cantidad de bits por byte transferido son 10. START BIT + 8 bits + STOP BIT 
 */
#define TIMER_TICKS_PER_BYTE ((10*100000)/SERIAL_SPEED + 1)

/* TIMER_TICKS_PER_BYTE: La frecuencia en este caso es SERIAL_SPEED,
 * SERIAL_SPEED expresa la cantidad de bits por segundo, nosotros necesitamos bytes,
 * (SERIAL_SPEED/9), esta seria la frecuencia de bytes por segundo. Ahora
 * por la expresion T=1/F tenemos que el periodo es de 1/(SERIAL_SPEED/10), esto
 * quiere decir que se va a demorar 1/(SERIAL_SPEED/10) segundos por cada byte.
 * 1/(SERIAL_SPEED/10) = 10/SERIAL_SPEED, HAL genera un tick cada 0.00001 segundos
 * osea que 100000 ticks es un segundo, de esta manera 10/SERIAL_SPEED*100000 es la
 * cantidad de ticks por byte transferido.
 *
 *      Recuerden, por cada segundo, 100000 ticks :-) 
 *  >>> Creo que esto es demaciado.... tal vez llegar a 1mS seria mas sensato, tal vez
 *      fui exagerado cuando hablamos la otra vez.(rab)
 *  >>> Proximo ciclo de desarrollo cambiamos eso (felipe)
 */

static uint16_t needed, received = 0, discard = 0;
static uint8_t 	cur_stage = 0;
static uint32_t rx_start, rx_timeout;
static uint8_t 	cmp_addr(struct private_address *, struct frame *, uint8_t);
static void got_a_frame();

/* Reglas:
 * 1: Cuando se comparan 2 direcciones (big-endian) siempre se debe comenzar por el byte menos
 * significativo.
 */

/* ucmp: Esta estructura anonima contiene datos de control. 
 */
static struct {
	/* Nuestra direccion */
	struct private_address sys_addr;
	/* bits 0-0: ack waiting, 1-1: ACK / NAK, bits 2-2: frame in queue */
	uint8_t sys_bits;

	/* Funcion llamada luego de recibir un frame valido */
	func_t sys_user_routine;
} ucmp;


/* storage: Espacio para almacenar un frame entrante, y el origen esperada de un ACK
 */
struct __attribute__((packed)) {

	union ucmp_buffer input;

	/* Â¿Quien me envia un agradecimiento? ack_from */
	struct private_address ack_from;
} storage;

/* GET_NADDR(): Consigue nuestra direccion en una estructura private_address */
struct private_address *GET_NADDR() 
{
	return &ucmp.sys_addr;
}

/* __GET_ADDR(): Fue traido la seccion independiente, ahora el codigo esta orientado a 8 bits
 * Las direcciones son Big-endian 
 */
void __GET_ADDR(struct private_address *pa, struct frame *frm, uint8_t sd) 
{
	uint8_t off = 0;
	int8_t i = ADDR_LEN - 1, j = DADDR_SIZE(frm);

	/* Necesito la direccion de origen? */
	if(sd) {
		off += j;
		j += (pa->pa_size = SADDR_SIZE(frm));
	} else
		pa->pa_size = j;

	j--; /* to index */

	while(i >= 0)
		pa->pa_addr[i--] = j >= off? frm->ahead[j--] : 0;
}

/* SET_ADDR(): Esta funcion ajusta la direccion de destino y origen de un frame.
 */
void SET_ADDR(struct frame *frm, struct private_address *pa0, struct private_address *pa1) 
{
	int8_t i, j = ADDR_LEN - 1;
	uint8_t off = 0;

	i = pa0->pa_size - 1;
	while(i >= 0)
		frm->ahead[i--] = pa0->pa_addr[j--];

	off = pa0->pa_size;

	j = ADDR_LEN - 1;
	i = pa1->pa_size + off - 1;
	while(i >= off)
		frm->ahead[i--] = pa1->pa_addr[j--];

	SET_DD(frm, pa0->pa_size);
	SET_SS(frm, pa1->pa_size);
}

/* paddr_size(): Consigue el tamaño de una direccion.
 */
uint8_t paddr_size(addr_t addr) 
{
	uint8_t i = 0;

	while(i < ADDR_LEN && addr[i] == 0)
		i++;

	return ADDR_LEN - i;
}

/* ucmp_rx_callback(): 
 */
static void ucmp_rx_callback(uint8_t byte) 
{
	uint8_t dd;
	uint16_t ahead;
	struct frame *frm = &storage.input.as_frame;

	/* 1. cur_stage siempre es 0 en el inicio de una comunicacion
	 * 2. Cualquier recepcion o descarte esta asociado a un tiempo maximo para su realizacion
	 */

	if(IS_FRAME_IN_QUEUE)
		return;

	if(cur_stage > 0) {
		if( (hal_timer_ticks - rx_start) >= rx_timeout ) {
			received = 0;
			cur_stage = 0;
			discard = 0;
		}

		if(discard > 0) {
			if(--discard == 0) {
				received = 0;
				cur_stage = 0;
			} else
				return;
		}
	}

	storage.input.as_array[received++ % sizeof(storage.input.as_array)] = byte;

	switch(cur_stage) {
		case 0:
			rx_timeout = TIMER_TICKS_PER_BYTE * 2;
			cur_stage++;
			rx_start = hal_timer_ticks;
			break;
		case 1:
			if(received == 3) {
				if(frm->stx != STX) {
					discard = 0xffff; /* Lo que interesa ahora es el rx_timeout */
					/* No podemos confiar en este frame, así que descartaremos
					 * por una cantidad de tiempo equivalente a la transferencia de un frame
					 * con su maximo tamaño.
					 */
					rx_timeout = TIMER_TICKS_PER_BYTE * sizeof(storage.input.as_array);

#if SUPPORTED_NNNNN < 31 /* No es necesario este codigo si SUPPORTED_NNNNN es 31, porque es el maximo formado por 5 bits */
				} else
				if(NNNNN(frm) > SUPPORTED_NNNNN) {
					discard = DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm) + NNNNN(frm) + E(frm) + 1;
					rx_timeout = TIMER_TICKS_PER_BYTE * discard;
#endif

				} else {
					dd = DADDR_SIZE(frm);
					rx_timeout = TIMER_TICKS_PER_BYTE * dd;
					needed = 3 + dd;
					cur_stage++;
				}
				rx_start = hal_timer_ticks;
			}
			break;
		case 2:
			if(received == needed) {
				ahead = SADDR_SIZE(frm) + PP(frm) + NNNNN(frm) + E(frm) + 1;

				if((DADDR_SIZE(frm) == 0) || CMP_DADDR(&ucmp.sys_addr, frm)) {
					rx_timeout = TIMER_TICKS_PER_BYTE * ahead;
					needed += ahead;
					cur_stage++;
				} else {
					discard = ahead;
					rx_timeout = TIMER_TICKS_PER_BYTE * discard;
				}

				rx_start = hal_timer_ticks;
			}
			break;
		case 3:
			if(received == needed) {
				if(storage.input.as_array[received-1] == EOT)
					SET_FRAME_IN_QUEUE;

				cur_stage = 0;
				received = 0;
			}
			break;
	}
}

/* ucmp_init(): Iniciadora del subsistema.
 */
void ucmp_init(addr_t addr, func_t user_callback) 
{
	int i;

	for(i=0; i < ADDR_LEN; i++)
		ucmp.sys_addr.pa_addr[i] = addr[i];

	ucmp.sys_addr.pa_size = paddr_size(ucmp.sys_addr.pa_addr);

	hal_serial_init(SERIAL_SPEED);
	hal_serial_rx_cb(ucmp_rx_callback);
	hal_timer_init();

	/* Funcion llamada luego de recibir un frame */
	ucmp.sys_user_routine = user_callback;
}

/* ack_wait(): Espera ACK_TIMEOUT ticks por un ACK.
 */
static uint8_t ack_wait() 
{
	uint32_t start;
	uint8_t ret;

	start = hal_timer_ticks;

	do {
		if(IS_FRAME_IN_QUEUE)
			got_a_frame();
	} while(!(ret = READ_ACKNAK) && (hal_timer_ticks - start) < ACK_TIMEOUT);

	return ret ? OK : ERROR;
}

/* ucmp_send(): Envia un frame. Si requiere un agradecimiento, lo espera.
 */
uint8_t ucmp_send(struct frame *frm) 
{
	uint8_t retry, status;
	uint16_t size = FRM_SIZE(frm);

	if(AA(frm) == RACK) {
		GET_DADDR(&storage.ack_from, frm);

		/* Limpia ACKNAK */
		CLEAR_ACKNAK;

		ACTIVATE_ACK_WAIT;

		/* En el caso de fallar reintenta RETRY_MAX veces */
		retry = 0;
		do {
			hal_serial_write((uint8_t *)frm, size);
		} while( (status = ack_wait()) == ERROR && retry++ < RETRY_MAX);

		DEACTIVATE_ACK_WAIT;
		return status;
	}

	hal_serial_write((uint8_t *)frm, size);
	return OK;
}

/* inverse_addresses(): Copia direcciones entre 2 frames pero de forma inversa.
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
void inverse_addresses(struct frame *frm0, struct frame *frm1) 
{
	struct private_address pa0, pa1;

	GET_DADDR(&pa0, frm1);
	GET_SADDR(&pa1, frm1);
	SET_ADDR(frm0, &pa1, &pa0);
}

/* __send_acknak(): Envia ack/nak según corresponda.
 */
static void send_acknak(uint8_t w) 
{
	uint8_t foo[10];
	struct frame *frm = (struct frame *)foo;

	/* Me aseguro de que la cabezera este limpia */
	CLR_HEADER(frm);
	frm->stx = STX;

	/* Â¿Es un ack o un nak? */
	frm->hd[B2] |= w == OK? ACK : NAK;

	/* Copio las direcciones de la frame de entrada a "frm", pero intercambiadas */
	inverse_addresses(frm, &storage.input.as_frame);

	hal_serial_write(foo, FRM_SIZE(frm));
}

/* Por implementar */
uint8_t crc8(struct frame *frm)
{
	return 0;
}

static uint8_t check_integrity(struct frame *frm) 
{
	/* CRC8 code */
	return OK;
}

static uint8_t cmp_addr(struct private_address *pa, struct frame *frm, uint8_t s) 
{
	uint8_t size = DADDR_SIZE(frm), off = 0;
	int8_t i, j;

	if(s) {
		off += size;
		size = SADDR_SIZE(frm);
	}

	if(size > ADDR_LEN || size != pa->pa_size) /* La direccion no debe sobrepasar lo soportado por la arquitectura */
		return 0;

	i = size + off - 1;
	j = ADDR_LEN - 1;

	while(i >= off)
		if(pa->pa_addr[j--] != frm->ahead[i--])
			return 0;

	return 1;
}

/*----------------------------------------------------------------------------+
 * got_a_frame(): Rutina llamada luego de recibir un frame.                   |
 *----------------------------------------------------------------------------+
 */
static void got_a_frame() 
{
	struct frame *frm = &storage.input.as_frame;
	struct private_address src_addr;
	uint8_t ret, *data_addr = storage.input.as_frame.ahead + DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);

	/* Â¿Es para mi? */
	if(!(DADDR_SIZE(frm) == 0)) {
		ret = AA(frm);
		if(ret > 0x1) { /* Relacionado con Metodos de Deteccion de Errores */
			/* Â¿Estamos esperando ack?, 
			 * Â¿Es de quien espero? y ?Se trata de un agradecimiento? Si, activo el bit ACKNAK 
			 */
			if(ACK_WAIT_IS_PRESENT && ret == ACK && CMP_SADDR(&storage.ack_from, frm))
					SET_ACK;

			return;
		}

		/* Â¿Estamos esperando ack? Si -> return
		 * Es imperativo recibir un "ack" antes del timeout 
		 */
		if(ACK_WAIT_IS_PRESENT)
			return;

		ret = check_integrity(frm);
		if(AA(frm) == RACK)
			send_acknak(ret);

		if(ret == OK) {
			GET_SADDR(&src_addr, frm);
			ucmp.sys_user_routine(data_addr, &src_addr, NNNNN(frm) | (1 << 5));
		}

	} else 
	if(!ACK_WAIT_IS_PRESENT) { 
		/* No hay notificacion de FRAME recibido, solo chequeo de integridad */
		if(check_integrity(frm) == OK) {
			GET_SADDR(&src_addr, frm);
			ucmp.sys_user_routine(data_addr, &src_addr, NNNNN(frm));
		}
	}
}

void ucmp_buffer_digest_data(union ucmp_buffer *buf, int8_t *data, uint8_t size, uint8_t attr)
{
	struct frame *frm = &buf->as_frame;
	uint8_t offset;

	size = size & 0x1f; /* 5 bits para el tamanno */
	offset = DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);

	memcpy(frm->ahead + offset, data, (uint16_t)size);
	offset += size;
	if(attr) { /* CHECK_INTEGRITY */
		frm->ahead[offset] = crc8(frm);
		SET_E(frm);
		offset++;
	}

	SET_NNNNN(frm, size);
	frm->ahead[offset] = EOT;
}

/* Tarea para kepler
 */
TASK(ucmp_task0)
{
	if(IS_FRAME_IN_QUEUE) {
		got_a_frame();
		CLEAR_FRAME_IN_QUEUE;
	}
}

