/*-----------------------------------------------------------------------------
Acceleation Data manipulation functions
\author - Sanjay Nair
-----------------------------------------------------------------------------*/

#include "acceldata.h"
#include "pq.h"
#include "cbuf.h"

/* declare global variables for storage
alternatively, we can use system specific memory allocation calls using 
custom interfaces through function pointers (such as function pointers for 
platform specific memory allocation routines).
*/
struct CircBuf g_circ_buf;
struct PQ g_pq;

/*-----------------------------------------------------------------------------
Initialize the Accleartion data
\return          Status: STATUS_OK - success, otherwise STATUS_ERR
-----------------------------------------------------------------------------*/
int accel_init()
{
    pq_init(&g_pq);
    circ_buf_init(&g_circ_buf);

	return STATUS_OK;
}

/*-----------------------------------------------------------------------------
Returns the circular buffer
\return          CircBuf
-----------------------------------------------------------------------------*/
struct CircBuf*accel_get_circ_buf()
{
    return &g_circ_buf;
}

/*-----------------------------------------------------------------------------
Initialize the Accleartion data
\return          Status: STATUS_OK - success, otherwise STATUS_ERR
-----------------------------------------------------------------------------*/
struct PQ* accel_get_pq()
{
    return &g_pq;
}

/*-----------------------------------------------------------------------------
Add Data to PQ
\param data     16-bit data
-----------------------------------------------------------------------------*/
static void add_data_to_pq(uint16_t data)
{
    if (!pq_full(&g_pq))
    {
        pq_insert(&g_pq, data);
    }
    else if (data > pq_peek(&g_pq))
    {
        pq_get_min(&g_pq);
        pq_insert(&g_pq, data);
    }
}

/*-----------------------------------------------------------------------------
Process 3 bytes of data for 2 12-bit data
Expected data to be in Big-Endian format where lower memory address contain
MSB of the 12-bit data. 
\param buf			Buffer
\param size			must be 3.
-----------------------------------------------------------------------------*/
void accel_process_3_bytes(const uint8_t *buf, uint32_t size)
{
	uint16_t data;
	ASSERT(3 == size);
	
	/* Get first 12-bits */
	data = (buf[0] << 4) | (buf[1] >> 4);
	add_data_to_pq(data);
	circ_buf_put(&g_circ_buf, data);

	/* Get next 12-bits */
	data = ((buf[1] & 0x0F) << 8) | buf[2];
	add_data_to_pq(data);
	circ_buf_put(&g_circ_buf, data);
}

/*-----------------------------------------------------------------------------
Process 2 bytes of data for 1 12-bit data. Last nibble will be discarded
\param buf			Buffer
\param size			must be 2
-----------------------------------------------------------------------------*/
void accel_process_2_bytes(const uint8_t *buf, uint32_t size)
{
	uint16_t data;
	ASSERT(2 == size);

	/* Get first 12-bits */
	data = (buf[0] << 4) | (buf[1] >> 4);
	add_data_to_pq(data);
	circ_buf_put(&g_circ_buf, data);
}
