/*-----------------------------------------------------------------------------
Circular Buffer implemenation
\author - Sanjay Nair
-----------------------------------------------------------------------------*/

#ifndef _CIRCBUF_H_
#define _CIRCBUF_H_

#include "config.h"

#if USE_COMPRESSED_CIRC_BUF == 0
/*-----------------------------------------------------------------------------
Circular Buffer implementation.

Note: 
This implementation creates a buffer with a fixed size defined by the 
MAX_CIRC_BUF_ITEMS. Please note that we can  generalize the implementation
by creating buf as pointer (size_t *buf) that can be passed from caller functions.

For this particular excercise, this code addresses the requirements.
-----------------------------------------------------------------------------*/
typedef struct CircBuf
{
    uint16_t buf[MAX_CIRC_BUF_ITEMS];
	uint8_t head;
	uint8_t tail;
    uint8_t count;
} CircBuf;


void circ_buf_init(CircBuf* _this);
int circ_buf_size(CircBuf* _this);
int circ_buf_empty(CircBuf* _this);
int circ_buf_full(CircBuf* _this);
uint16_t circ_buf_get(CircBuf* _this);
void circ_buf_put(CircBuf* _this, uint16_t data);

#endif /* USE_COMPRESSED_CIRC_BUF */

#endif  /* _CIRCBUF_H_ */


