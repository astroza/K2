/* 2006/12/28 ElectroLinux
 * Felipe Astroza
 * CuricÛ
 * file: ucmp.c
 * UCMP <=> Microcontroller Multipoint Protocol
 */

#include <ucmp.h>
#include <kepler.h>
#include <hal_serial.h>
#include <hal_timer.h>
#include <string.h>

/* Tiempo estimado para la transferencia de un octeto a SERIAL_SPEED baudios
 * Observacion: La cantidad de bits por byte transferido son 10. START BIT + 8 bits + STOP BIT 
 */
#define TIMER_TICKS_PER_BYTE ((10*100000)/SERIAL_SPEED + 1)

/* TIMER_TICKS_PER_BYTE: La frecuencia en este caso es SERIAL_SPEED,
 * SERIAL_SPEED expresa la cantidad de bits por segundo, nosotros necesitamos bytes,
 * (SERIAL_SPEED/10), esta seria la frecuencia de bytes por segundo. Ahora
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

static uint32_t rx_start, rx_timeout, tx_start;
static uint8_t 	cur_stage = 0, discard = 0, received = 0, needed, tx_retry;
static uint8_t 	cmp_addr(struct private_address *, struct frame *, uint8_t);
static void frame_to_user();

/* Reglas:
 * 1: Cuando se comparan 2 direcciones (big-endian) siempre se debe comenzar por el byte menos
 * significativo.
 */

/* ucmp: Esta estructura anonima contiene datos de control. 
 */
static struct {
	/* Nuestra direccion */
	struct private_address local_addr;

	uint8_t ack_waiting:1;
	/* status 0: Esta en pleno envio de una trama (con RACK), status 1: Listo para enviar una trama */
	uint8_t status:1;
	volatile uint8_t frame_in_queue:1;
	uint8_t reserved:5;

	/* Funcion llamada luego de recibir un frame valido */
	func_t user_routine;
} ucmp;


/* storage: Espacio para almacenar un frame entrante, y el origen esperada de un ACK
 */
struct __attribute__((packed)) {

	union ucmp_buffer input;
	union ucmp_buffer output;

	/* øQuien me envia un agradecimiento? ack_from */
	struct private_address ack_from;
} storage;

/* GET_LOCAL_ADDRESS(): Retorna nuestra direccion.
 */
struct private_address *GET_LOCAL_ADDRESS() 
{
	return &ucmp.local_addr;
}

/* __GET_ADDR(): Extrae la direccion de destino u origen de una trama a private_address.  
 */
void __GET_ADDR(struct private_address *pa, struct frame *frm, uint8_t sd) 
{
	uint8_t off = 0;
	int8_t i = ADDR_SIZE - 1, j = DADDR_SIZE(frm);

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
	int8_t i, j = ADDR_SIZE - 1;
	uint8_t off = 0;

	i = pa0->pa_size - 1;
	while(i >= 0)
		frm->ahead[i--] = pa0->pa_addr[j--];

	off = pa0->pa_size;

	j = ADDR_SIZE - 1;
	i = pa1->pa_size + off - 1;
	while(i >= off)
		frm->ahead[i--] = pa1->pa_addr[j--];

	SET_DD(frm, pa0->pa_size);
	SET_SS(frm, pa1->pa_size);
}

/* paddr_size(): Consigue el tama√±o de una direccion.
 */
uint8_t paddr_size(uint8_t *addr) 
{
	uint8_t i = 0;

	while(i < ADDR_SIZE && addr[i] == 0)
		i++;

	return ADDR_SIZE - i;
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

	if(ucmp.frame_in_queue)
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

	storage.input.as_bytes[received++ % sizeof(storage.input.as_bytes)] = byte;

	switch(cur_stage) {
		case 0:
			rx_timeout = TIMER_TICKS_PER_BYTE * 2;
			cur_stage++;
			rx_start = hal_timer_ticks;
			break;
		case 1:
			if(received == 3) {
				if(frm->stx != STX) {
					discard = 0xff; /* Lo que interesa ahora es el rx_timeout */
					/* No podemos confiar en este frame, as√≠ que descartaremos
					 * por una cantidad de tiempo equivalente a la transferencia de un frame
					 * con su maximo tama√±o.
					 */
					rx_timeout = TIMER_TICKS_PER_BYTE * sizeof(storage.input.as_bytes);

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

				if((DADDR_SIZE(frm) == 0) || CMP_DADDR(&ucmp.local_addr, frm)) {
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
				if(storage.input.as_bytes[received-1] == EOT)
					ucmp.frame_in_queue = 1;

				cur_stage = 0;
				received = 0;
			}
			break;
	}
}

/* ucmp_init(): Iniciadora del subsistema.
 */
void ucmp_init(uint8_t *addr, func_t user_callback) 
{
	int i;

	for(i=0; i < ADDR_SIZE; i++)
		ucmp.local_addr.pa_addr[i] = addr[i];

	ucmp.local_addr.pa_size = paddr_size(ucmp.local_addr.pa_addr);

	hal_serial_init(SERIAL_SPEED);
	hal_serial_rx_cb(ucmp_rx_callback);
	hal_timer_init();

	/* Funcion llamada luego de recibir un frame */
	ucmp.user_routine = user_callback;
}

/* ucmp_send(): Envia la trama en el buffer y activa la espera de ACK si lo requiere.
 * Nota: No se verifica si se esta esperando un agradecimiento, con ucmp_buffer_get() es suficiente.
 */
uint8_t ucmp_send() 
{
	struct frame *frm = &storage.output.as_frame;

	hal_serial_write(storage.output.as_bytes, FRM_SIZE(frm));

	if(AA(frm) == RACK) {
		GET_DADDR(&storage.ack_from, frm);

		/* Activa la espera de agradecimiento desde storage.ack_from */
		ucmp.ack_waiting = 1;

		/* Bloqueamos el uso de uCmp hasta que este proceso se complete */
		ucmp.status = IN_PROGRES;
		tx_retry = RETRY_MAX;
		tx_start = hal_timer_ticks; /* Captura de hal_timer_ticks al momento del envio */
	} else
		ucmp.status = READY;

	return ucmp.status;
}

/* ucmp_get_buffer(): Si uCmp esta listo para enviar una trama, retorna la direccion del buffer para salida
 */
struct frame *ucmp_buffer_get()
{
	if(ucmp.status == IN_PROGRES)
		return NULL;

	return &storage.output.as_frame;
}

/* inverse_addresses(): Copia direcciones entre 2 frames pero de forma inversa.
 * Esta rutina es una optimizacion para peticion/respuesta.
 * inversa:
 * frm0 -> destination = frm1 -> source
 * frm0 -> source = frm1 -> destination
 *
 * Para los que conocieron la implementacion anterior:
 * En el pasado previlegiaba la eficacia,
 * Logrando esta tarea con una PEQUE√ëA optimizacion que 
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

/* __send_acknak(): Envia ack/nak seg˙n corresponda.
 */
static void send_acknak(uint8_t w) 
{
	struct frame *frm = &storage.output.as_frame;

	/* Me aseguro de que la cabezera este limpia */
	CLR_HEADER(frm);
	frm->stx = STX;

	/* øEs un ack o un nak? */
	SET_AA(frm, (w == OK? ACK : NAK));

	/* Copio las direcciones de la frame de entrada a "frm", pero intercambiadas */
	inverse_addresses(frm, &storage.input.as_frame);

	frm->ahead[DADDR_SIZE(frm) + SADDR_SIZE(frm)] = EOT;
	hal_serial_write(storage.output.as_bytes, FRM_SIZE(frm));
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

	if(size != pa->pa_size)
		return 0;

	i = size + off - 1;
	j = ADDR_SIZE - 1;

	while(i >= off)
		if(pa->pa_addr[j--] != frm->ahead[i--])
			return 0;

	return 1;
}

/* frame_to_user(): Queda verificar la integridad, si es necesario, de la trama para llamar la rutina de usuario.
 */
static void frame_to_user() 
{
	struct frame *frm = &storage.input.as_frame;
	struct ucmp_message msg;
	uint8_t ret;

	msg.content = storage.input.as_frame.ahead + DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);

	/* øEs para mi? */
	if(DADDR_SIZE(frm) != 0) {

		ret = check_integrity(frm);
		if(AA(frm) == RACK)
			send_acknak(ret);

		if(ret == OK) {
			GET_SADDR(&msg.src, frm);
			msg.size = NNNNN(frm);
			msg.dst = TO_NODE;
			ucmp.user_routine(&msg);
		}

	} else {
		/* No hay notificacion de FRAME recibido, solo chequeo de integridad */
		if(check_integrity(frm) == OK) {
			GET_SADDR(&msg.src, frm);
			msg.size = NNNNN(frm);
			msg.dst = TO_BROADCAST;
			ucmp.user_routine(&msg);
		}
	}
}

void ucmp_buffer_digest_data(uint8_t *data, uint8_t size, uint8_t attr)
{
	struct frame *frm = &storage.output.as_frame;
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

/* Tarea para Kepler
 */
TASK(ucmp_task0)
{
	uint8_t aa, acknak = 0;

	if(ucmp.frame_in_queue) {

		if(ucmp.ack_waiting) {
			aa = AA(&storage.input.as_frame);
			if(aa > 1 && CMP_SADDR(&storage.ack_from, &storage.input.as_frame))
				acknak = 3 - aa;

			if(acknak == 0) {
				if( (hal_timer_ticks - tx_start) >= ACK_TIMEOUT ) {
					if(tx_retry) {
						hal_serial_write(storage.output.as_bytes, FRM_SIZE(&storage.output.as_frame));
						tx_retry--;
						tx_start = hal_timer_ticks;
					} else
						goto ready;
				}
			} else {
				ready:
				ucmp.ack_waiting = 0;
				ucmp.status = READY;
			}
		} else
			frame_to_user();

		ucmp.frame_in_queue = 0;
	}
}
