/*-----------------------------------------------------------------------------
Priority Queue (Minheap) implemenation
Can be extended to max_heap by taking a compartor function for comparing
elements.

\author - Sanjay Nair
-----------------------------------------------------------------------------*/
#include "pq.h"

static void swap(uint16_t* pa, uint16_t *pb)
{
    uint16_t t;
    t = *pa;
    *pa = *pb;
    *pb = t;
}

/*-----------------------------------------------------------------------------
Move up the node to restore the heap condition if needed.
\param a			Array of data
\pararm k			Position of the insertion

Note: Compiler can convert k/2 operation to shft operation (>>1) for 
performance reasons. This code is preserved for readability
-----------------------------------------------------------------------------*/
static void pq_fix_up(uint16_t a[], uint8_t k)
{
	while ((k > 1) && a[k / 2] > a[k])
	{
		uint8_t k2 = k / 2;
        swap(&a[k], &a[k2]);
		k = k2;
	}
}

/*-----------------------------------------------------------------------------
Move down the node to restore the heap condition if needed.
\param a			Array of data
\pararm k			Position of the insertion
\param size			Size of the heap

Note: Compiler can convert k*2 operation to shft operation (<<1) for 
performance reasons. This code is preserved for readability
-----------------------------------------------------------------------------*/
static void pq_fix_down(uint16_t a[], uint8_t k, uint8_t size)
{
	uint32_t j;
	
	ASSERT(a);
	while (2 * k <= size)
	{
        j = 2 * k;

        if ((j < size) && (a[j] > a[j + 1]))
			++j;

		if (!(a[k] > a[j]))
			break;

        swap(&a[k], &a[j]);
		k = j;
	}
}

/*-----------------------------------------------------------------------------
Initializes the Priority Queue
\param _this		Pointer to Priority Queue
-----------------------------------------------------------------------------*/
void pq_init(struct PQ* _this)
{
    _this->index = 0;
}

/*-----------------------------------------------------------------------------
Returns the size of Priority Queue
\param _this		Pointer to Priority Queue
\return         Size
-----------------------------------------------------------------------------*/
uint8_t pq_size(struct PQ* _this)
{
    return _this->index;
}

/*-----------------------------------------------------------------------------
Checks if Priority Queue is empty
\param _this		Pointer to Priority Queue
\return             1 if PQ is empty
-----------------------------------------------------------------------------*/
int pq_empty(struct PQ* _this)
{
	return (_this->index == 0);
}

/*-----------------------------------------------------------------------------
Checks if Priority Queue is full
\param _this		Pointer to Priority Queue
\return             1 if PQ is full
-----------------------------------------------------------------------------*/
int pq_full(struct PQ* _this)
{
    return (_this->index == MAX_SORTED_ITEMS);
}

/*-----------------------------------------------------------------------------
Inserts item in priority queue. Caller needs to verify if PQ has enough space
\param _this		Pointer to Priority Queue
\param val		Value to insert
-----------------------------------------------------------------------------*/
void pq_insert(struct PQ* _this, uint16_t val)
{
    ASSERT(_this->index < MAX_SORTED_ITEMS);
    _this->buf[++_this->index] = val;
    pq_fix_up(_this->buf, _this->index);
}

/*-----------------------------------------------------------------------------
Peeks the top value 
\param _this		Pointer to Priority Queue
\param val		Value to insert
-----------------------------------------------------------------------------*/
uint16_t pq_peek(struct PQ* _this)
{
    ASSERT(_this->index > 0);

    return _this->buf[1];
}

/*-----------------------------------------------------------------------------
Gets the minimum value from the priority Queue. 
This operation will remove the min element from the priority queue
\param _this		Pointer to Priority Queue
-----------------------------------------------------------------------------*/
uint16_t pq_get_min(struct PQ* _this)
{
    ASSERT(_this->index > 0);

    swap(&_this->buf[1], &_this->buf[_this->index]);
    pq_fix_down(_this->buf, 1, _this->index - 1);
    return _this->buf[_this->index--];
}

