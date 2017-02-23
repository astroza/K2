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
#define TIMER_TICKS_PER_BYTE ((10*100000)/SERIAL_SPEED)

/* TIMER_TICKS_PER_BYTE: La frecuencia en este caso es SERIAL_SPEED,
 * SERIAL_SPEED expresa la cantidad de bits por segundo, nosotros necesitamos bytes,
 * (SERIAL_SPEED/10), esta seria la frecuencia de bytes por segundo. Ahora
 * por la expresion T=1/F tenemos que el periodo es de 1/(SERIAL_SPEED/10), esto
 * quiere decir que se va a demorar 1/(SERIAL_SPEED/10) segundos por cada byte.
 * 1/(SERIAL_SPEED/10) = 10/SERIAL_SPEED, HAL genera un tick cada 0.00001 segundos
 * osea que 100000 ticks es un segundo, de esta manera 10/SERIAL_SPEED*100000 es la
 * cantidad de ticks por byte transferido.
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
	volatile uint8_t frame_in_queue:1;

	/* Funcion llamada luego de recibir un frame valido */
	func_t user_routine;

	/* øQuien me envia un agradecimiento? ack_from */
	struct private_address ack_from;
} ucmp;

static union ucmp_buffer input;

static union ucmp_buffer output;
static uint8_t output_offset;

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
void SET_ADDR(struct frame *frm, struct private_address *dst, struct private_address *src)
{
	int8_t i, j = ADDR_SIZE - 1;
	uint8_t off = 0;

	i = dst->pa_size - 1;
	while(i >= 0)
		frm->ahead[i--] = dst->pa_addr[j--];

	off = dst->pa_size;

	j = ADDR_SIZE - 1;
	i = src->pa_size + off - 1;
	while(i >= off)
		frm->ahead[i--] = src->pa_addr[j--];

	SET_DD(frm, dst->pa_size);
	SET_SS(frm, src->pa_size);
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
	struct frame *frm = &input.as_frame;

	/* 1. cur_stage siempre es 0 en el inicio de una comunicacion.
	 * 2. Cualquier recepcion o descarte esta asociado a un tiempo maximo.
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

	input.as_bytes[received++ % sizeof(input.as_bytes)] = byte;

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
					rx_timeout = TIMER_TICKS_PER_BYTE * sizeof(input.as_bytes);
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
				if(input.as_bytes[received-1] == EOT)
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

	ucmp.frame_in_queue = 0;
	ucmp.ack_waiting = 0;

	/* Funcion llamada luego de recibir un frame */
	ucmp.user_routine = user_callback;
}

/* ucmp_send(flags): Completa y envia la trama "output".
 */
uint8_t ucmp_send(uint8_t flags)
{
	struct frame *frm = &output.as_frame;
	uint8_t status;

	if(ucmp.ack_waiting)
		return BUSY;

	if(flags & RACK)
		SET_AA(frm, RACK);

	if(flags & CHKINT) {
		SET_E(frm);
		frm->ahead[output_offset++] = crc8(frm);
	}

	frm->ahead[output_offset++] = EOT;

	hal_serial_write(output.as_bytes, 3 + output_offset);

	if(AA(frm) == RACK) {
		GET_DADDR(&ucmp.ack_from, frm);

		/* Activa la espera de agradecimiento desde ucmp.ack_from */
		ucmp.ack_waiting = 1;
		tx_retry = RETRY_MAX;

		status = IN_PROGRESS;
		tx_start = hal_timer_ticks; /* Captura de hal_timer_ticks al momento del envio */
	} else
		status = READY;

	return status;
}

/* inverse_addresses(): Copia direcciones entre 2 frames pero de forma inversa.
 * inversa:
 * frm0 -> destination = frm1 -> source
 * frm0 -> source = frm1 -> destination
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
	struct frame *frm = &output.as_frame;

	CLR_HEADER(frm);
	frm->stx = STX;

	/* øEs un ack o un nak? */
	SET_AA(frm, (w == OK? ACK : NAK));

	/* Copio las direcciones de la frame de entrada a "frm", pero intercambiadas */
	inverse_addresses(frm, &input.as_frame);

	output_offset = DADDR_SIZE(frm) + SADDR_SIZE(frm);
	frm->ahead[output_offset++] = EOT;
	hal_serial_write(output.as_bytes, 3 + output_offset);
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

/* frame_to_user(): Verifica trama "input" y llama a la funcion de usuario.
 */
static void frame_to_user() 
{
	struct frame *frm = &input.as_frame;
	struct ucmp_message msg;
	uint8_t ret;

	msg.content = input.as_frame.ahead + DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);

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

/* ucmp_get_buffer(dst, src, size): Prepara trama "output" y retorna direccion del buffer con "size" bytes.
 */
uint8_t *ucmp_get_buffer(struct private_address *dst, struct private_address *src, uint8_t size)
{
	uint8_t *buf;

	if(ucmp.ack_waiting)
                return NULL;

	if(size & ~0x1f) /* if(size > 31) */
		return NULL;

	struct frame *frm = &output.as_frame;

	frm->stx = STX;
	CLR_HEADER(frm);
	SET_ADDR(frm, dst, src);

	output_offset = DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);
	buf = frm->ahead + output_offset;
	SET_NNNNN(frm, size);
	output_offset += size;

	return buf;
}

/* Tarea de UCMP
 */
TASK(ucmp_task0)
{
	uint8_t aa, acknak = NAK;

	if(ucmp.ack_waiting) {
		if(ucmp.frame_in_queue) {
			aa = AA(&input.as_frame);
			if(aa > 1 && CMP_SADDR(&ucmp.ack_from, &input.as_frame))
				acknak = aa;

			ucmp.frame_in_queue = 0;
		}

		if(acknak != ACK) {
			if( (hal_timer_ticks - tx_start) >= ACK_TIMEOUT ) {
				if(tx_retry) {
					hal_serial_write(output.as_bytes, 3 + output_offset);
					tx_retry--;
					tx_start = hal_timer_ticks;
				} else
					ucmp.ack_waiting = 0;
			}
		} else
			ucmp.ack_waiting = 0;
	} else
	if(ucmp.frame_in_queue) {
		frame_to_user();
		ucmp.frame_in_queue = 0;
	}
}
