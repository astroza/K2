/* 2006/12/28 ElectroLinux
 * Felipe Astroza
 * Curicó
 * file: ucmp.c
 * UCMP <=> Microcontroller Multipoint Protocol
 */

#include <ucmp.h>
#include <hal_serial.h>
#include <hal_timer.h>

#ifndef DATA_SIZE
#define DATA_SIZE 64
#endif

/* Mascaras para sys_bits */
#define ACKNAK_MASK 0x2
#define ACK_WAIT_MASK 0x1

#define CLEAR_ACKNAK ucmp.sys_bits &= ~ACKNAK_MASK
#define READ_ACKNAK (ucmp.sys_bits & ACKNAK_MASK)
#define ACK_IS_PRESENT ucmp.sys_bits |= ACKNAK_MASK
#define NAK_IS_PRESENT ucmp.sys_bits &= ~ACKNAK_MASK
#define ACK_WAIT_IS_PRESENT (ucmp.sys_bits & ACK_WAIT_MASK)

/* Activar y desactivar ACK_WAIT */
#define ACTIVATE_ACK_WAIT ucmp.sys_bits |= ACK_WAIT_MASK
#define DEACTIVATE_ACK_WAIT ucmp.sys_bits &= ~ACK_WAIT_MASK

/* Tiempo estimado para la transferencia de un octeto a SERIAL_SPEED baudios */
#define TIMER_TICKS_PER_BYTE ((8*100000)/SERIAL_SPEED)
/* TIMER_TICKS_PER_BYTE: La frecuencia en este caso es SERIAL_SPEED,
 * SERIAL_SPEED expresa la cantidad de bits por segundo, nosotros necesitamos bytes,
 * (SERIAL_SPEED/8), esta seria la frecuencia de bytes por segundo. Ahora
 * por la expresion T=1/F tenemos que el periodo es de 1/(SERIAL_SPEED/8), esto
 * quiere decir que se va a demorar 1/(SERIAL_SPEED/8) segundos por cada byte.
 * 1/(SERIAL_SPEED/8) = 8/SERIAL_SPEED, HAL genera un tick cada 0.00001 segundos
 * osea que 100000 ticks es un segundo, de esta manera 8/SERIAL_SPEED*100000 es la
 * cantidad de ticks por byte transferido.
 *
 *      Recuerden, por cada segundo, 100000 ticks :-) 
 */

static uint16_t needed, received = 0, discard = 0;
static uint8_t cur_stage = 0;
static uint32_t rx_start, rx_timeout;
static uint8_t cmp_addr(struct private_address *, struct frame *, uint8_t);

/* Reglas:
 * 1: Cuando se comparan 2 direcciones (big-endian) siempre se debe comenzar por el byte menos
 * significativo.
 */

/* ucmp: Esta estructura anonima contiene datos de control. 
 */
static struct {
	/* Nuestra direccion */
	struct private_address sys_addr;
	/* bits 0-0: ack waiting, 1-1: ACK / NAK */
	uint8_t sys_bits;

	/* Funcion llamada luego de recibir un frame valido */
	func_t sys_user_routine;
} ucmp;

/* storage: Espacio para almacenar un frame entrante, y el origen esperada de un ACK
 */
struct __attribute__((packed)) {
	uint8_t st_stx;
	uint8_t st_hd[2];

	/* potenciales bytes: dst addr, src addr, flags, DATA, crc[8, 16, 32] */
	uint8_t st_heap[9 + DATA_SIZE + 4 + 1];

	/* ¿Quien me envia un agradecimiento? st_ack_from */
	struct private_address st_ack_from;
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

/* EE_image(): Determina el tamaÃ±o de un detector de errores en un frame
 *
 *  f(x) =  2^(x+2)
 *  Dom(f) = A
 *  A = {x e |N / 0 < x < 4}
 */
uint8_t EE_image(uint8_t x)
{
	return x? 1 << (x + 2) : 0;
}

/* NNNN_image(): Retorna la cantidad de bytes en datos
 *  x = 31 no definido aún. 
 */
uint16_t NNNN_image(uint8_t x)
{
        if(x < 8)
                return x;

        return 1 << (x - 4);
}

/* rx_callback(): Espera 3 bytes, STX, HDB2 y HDB1, luego verifica la constante STX y el tamaño
 * del frame, si estos son correctos, entonces se prepara a esperar los bytes indicados por la cabezera
 * contenidos en la trama. Cuando es completado se lanza got_a_frame().
 */

static void rx_callback(uint8_t byte)
{
	uint8_t *buffer = (uint8_t *)&storage, dd;
	struct frame *frm = (struct frame *)&storage;
	uint16_t ahead;

	/* 1. cur_stage siempre es 0 en el inicio de una comunicacion
	 * 2. Cualquier recepcion o descarte esta asociado a un tiempo maximo para su realizacion
	 */

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

	buffer[received++ % (sizeof(storage) - sizeof(struct private_address))] = byte;

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
					rx_timeout = TIMER_TICKS_PER_BYTE * (3 + 9 + DATA_SIZE + 4 + 1);
				} else
				if(NNNN_image(NNNN(frm)) > DATA_SIZE) {
					discard = DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm) + NNNN_image(NNNN(frm)) + EE_image(EE(frm)) + 1;
					rx_timeout = TIMER_TICKS_PER_BYTE * discard;
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
				ahead = SADDR_SIZE(frm) + PP(frm) + NNNN_image(NNNN(frm)) + EE_image(EE(frm)) + 1;

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
				if(buffer[received-1] == EOT)
					got_a_frame();

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
	hal_serial_rx_cb(rx_callback);
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

	while(!(ret = READ_ACKNAK) && (hal_timer_ticks - start) < ACK_TIMEOUT);

	return !ret;
}

/* send_frame(): Envia un frame. Si requiere un agradecimiento, lo espera.
 */
uint8_t send_frame(struct frame *frm)
{
	uint8_t retry, status;
	uint16_t size = FRM_SIZE(frm);

	if(AA(frm) == RACK) {
		GET_DADDR(&storage.st_ack_from, frm);

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

	/* ¿Es un ack o un nak? */
	frm->hd[B2] |= w? ACK : NAK;

	/* Copio las direcciones del frame de entrada a "frm", pero intercambiadas */
	inverse_addresses(frm, (struct frame *)&storage);

	hal_serial_write((uint8_t *)frm, FRM_SIZE(frm));
}

static uint8_t call_edm(struct frame *frm)
{
	switch(GET_EDM(frm)) {
		case CRC8:

		case CRC16:

		case CRC32:

		default:
			/* TEST */
			return 1;
	}
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

/* got_a_frame(): Rutina llamada luego de recibir un frame.
 */
void got_a_frame()
{
	struct frame *frm = (struct frame *)&storage;
	struct private_address src_addr;
	uint8_t ret, *data_addr = storage.st_heap + DADDR_SIZE(frm) + SADDR_SIZE(frm) + PP(frm);

	/* ¿Es para mi? */
	if(!(DADDR_SIZE(frm) == 0)) {
		ret = AA(frm);
		if(ret > 0x1) { /* Relacionado con Metodos de Deteccion de Errores */
			/* ¿Estamos esperando ack?, 
			 * ¿Es de quien espero? y ¿Se trata de un agradecimiento? Si, activo el bit ACKNAK 
			 */
			if(ACK_WAIT_IS_PRESENT && ret == ACK && CMP_SADDR(&storage.st_ack_from, frm))
					ACK_IS_PRESENT;

			return;
		}

		/* ¿Estamos esperando ack? Si -> return
		 * Es imperativo recibir un "ack" antes del timeout 
		 */
		if(ACK_WAIT_IS_PRESENT)
			return;

		ret = call_edm(frm);
		if(AA(frm) == RACK)
			send_acknak(ret);
		if(ret) {
			GET_SADDR(&src_addr, frm);
			ucmp.sys_user_routine(data_addr, &src_addr, NNNN(frm) | 1 << 4);
		}

	} else 
	if(!ACK_WAIT_IS_PRESENT) { 
		/* No hay notificacion de FRAME recibido, solo chequeo de integridad */
		if(call_edm(frm)) {
			GET_SADDR(&src_addr, frm);
			ucmp.sys_user_routine(data_addr, &src_addr, NNNN(frm));
		}
	}
}

