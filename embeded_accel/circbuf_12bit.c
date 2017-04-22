/*-----------------------------------------------------------------------------
Circular Buffer Implementation
\author - Sanjay Nair
-----------------------------------------------------------------------------*/
#include "circbuf_12bit.h"

#if USE_COMPRESSED_CIRC_BUF

/*-----------------------------------------------------------------------------
Increments the value by 1. If value is >= modulus, then wraps around
\param val              Value to be incremented
\param modulus          Modulus value. Must be power of 2
\return                 Incremented value based on the modulus            

Note:
Can optimize if we make sure modulus is power of 2
ASSERT(0 == (modulus & (modulus - 1)));
uint8_t ret = (val + 1) & (modulus - 1);
-----------------------------------------------------------------------------*/
static uint8_t mod_inc(uint8_t val, uint8_t modulus)
{
	uint8_t ret;
	
	ASSERT(val < UINT8_MAX);
	ASSERT(modulus > 0);
	
	ret = val + 1;
    if (ret >= modulus)
		ret = 0;
    
	return ret;
}

/*-----------------------------------------------------------------------------
Initializes the Circular buffer
\param _this    Pointer to Circular Buffer     
-----------------------------------------------------------------------------*/
void circ_buf_init(CircBuf* _this)
{
    _this->count = 0;
    _this->head = 0;
    _this->tail = 0;
}

/*-----------------------------------------------------------------------------
Gets the circular buffer size
\param _this    Pointer to Circular Buffer
\return         Size of the circular buffer
-----------------------------------------------------------------------------*/
int circ_buf_size(CircBuf* _this)
{
    return _this->count;
}

/*-----------------------------------------------------------------------------
Checks if circular buffer is empty
\param _this    Pointer to Circular Buffer
\return         1 if circular buffer is empty. 
-----------------------------------------------------------------------------*/
int circ_buf_empty(CircBuf* _this)
{
    return (0 == _this->count);
}

/*-----------------------------------------------------------------------------
Checks if circular buffer is full
\param _this    Pointer to Circular Buffer
\return         1 if circular buffer is empty.
-----------------------------------------------------------------------------*/
int circ_buf_full(CircBuf* _this)
{
    return (MAX_CIRC_BUF_ITEMS == _this->count);
}

/*-----------------------------------------------------------------------------
Gets an item from the circular buffer. 
Decrements the count by 1
\param _this    Pointer to Circular Buffer
\return         1 if circular buffer is empty.
-----------------------------------------------------------------------------*/
uint16_t circ_buf_get(CircBuf* _this)
{
    uint16_t data;
    
    ASSERT(_this->count > 0);

    /* If tail is at begining of byte boundary, handle differently */
    if ((_this->tail % 3) == 0)
    {
        ASSERT(_this->tail < (MAX_CIRC_BUF_BYTES - 1));

        /* MSB data*/
        data = _this->buf[_this->tail];
        data <<= 4;
        ++_this->tail;

        data |= _this->buf[_this->tail] >> 4;
    }
    else
    {
        ASSERT(_this->tail < MAX_CIRC_BUF_BYTES);

        data = _this->buf[_this->tail] & 0xF;
        data <<= 8;
        ++_this->tail;

        data |= _this->buf[_this->tail];
        _this->tail = mod_inc(_this->tail, MAX_CIRC_BUF_BYTES);
    }
    
    --_this->count;

    return data;
}

/*-----------------------------------------------------------------------------
Puts an item from the circular buffer.
Increments the count by 1
\param _this    Pointer to Circular Buffer
\param data     Data to put
-----------------------------------------------------------------------------*/
void circ_buf_put(CircBuf* _this, uint16_t data)
{
    uint8_t data_msb;
    uint8_t data_lsb;
    uint8_t mask;

    if (_this->count == MAX_CIRC_BUF_ITEMS)
    {
        /* remove the item from tail */
        circ_buf_get(_this);
    }

    data_msb = (data >> 4) & 0xff;
    data_lsb = (data & 0xf);

    /* If head is at byte boundary, we can start at the begining of the byte boundary */
    if ((_this->head % 3) == 0)
    {
        ASSERT(_this->head < (MAX_CIRC_BUF_BYTES - 1));

        _this->buf[_this->head] = data_msb;
        ++_this->head;

        /* clear the top nibble*/
        mask = (1 << 4) - 1;
        _this->buf[_this->head] &= mask;

        data_lsb <<= 4;
        _this->buf[_this->head] |= data_lsb;
    }
    else
    {
        ASSERT(_this->head < MAX_CIRC_BUF_BYTES);

        /* clear the bottom nibble*/
        mask = (~0) << 4;
        _this->buf[_this->head] &= mask;
        
        /* set msb of data */
        _this->buf[_this->head] |= (data_msb >> 4);
        ++_this->head;

        /* set the lsb data */
        data_lsb = (data_msb << 4) | data_lsb;
        _this->buf[_this->head] = data_lsb;

        /* go to a new byte*/
        _this->head = mod_inc(_this->head, MAX_CIRC_BUF_BYTES);
    }
    
    ++_this->count;
}

#endif /* USE_COMPRESSED_CIRC_BUF */