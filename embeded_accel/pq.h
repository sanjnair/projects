/*-----------------------------------------------------------------------------
Priority Queue implemenation
\author - Sanjay Nair
-----------------------------------------------------------------------------*/

#ifndef _PQ_H_
#define _PQ_H_

#include "config.h"

/*-----------------------------------------------------------------------------
Generic Priroity Queue implementation.

Note:
This implementation creates a buffer with a fixed size defined by the
MAX_SORTED_ITEMS. Please note that we can  generalize the implementation
by creating buf as pointer (size_t *buf) that can be passed from caller functions.

For this particular excercise, this code addresses the requirements.
-----------------------------------------------------------------------------*/
typedef struct PQ
{
    uint16_t buf[MAX_SORTED_ITEMS + 1];
	uint8_t index;
} PQ;


void pq_init(struct PQ* _this);
uint8_t pq_size(struct PQ* _this);
int pq_empty(struct PQ* _this);
int pq_full(struct PQ* _this);
void pq_insert(struct PQ* _this, uint16_t val);
uint16_t pq_peek(struct PQ* _this);
uint16_t pq_get_min(struct PQ* _this);

#endif  /* _PQ_H_ */


