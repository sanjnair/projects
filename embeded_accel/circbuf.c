/*-----------------------------------------------------------------------------
Circular Buffer Implementation
\author - Sanjay Nair
-----------------------------------------------------------------------------*/
#include "circbuf.h"

#if USE_COMPRESSED_CIRC_BUF == 0

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
    uint16_t val;
    ASSERT(_this->count > 0);

	val = _this->buf[_this->tail];
    _this->tail = mod_inc(_this->tail, MAX_CIRC_BUF_ITEMS);
    --_this->count;

    return val;
}

/*-----------------------------------------------------------------------------
Puts an item from the circular buffer.
Increments the count by 1
\param _this    Pointer to Circular Buffer
\param data     Data to put
-----------------------------------------------------------------------------*/
void circ_buf_put(CircBuf* _this, uint16_t data)
{
    if (_this->count == MAX_CIRC_BUF_ITEMS)
    {
        /* remove the item from tail */
        circ_buf_get(_this);
    }

    _this->buf[_this->head] = data;
    _this->head = mod_inc(_this->head, MAX_CIRC_BUF_ITEMS);
    ++_this->count;
}

#endif /* USE_COMPRESSED_CIRC_BUF */
