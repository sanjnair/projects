/*-----------------------------------------------------------------------------
Header file for choosing different implementation for circular buffer
\author - Sanjay Nair
-----------------------------------------------------------------------------*/

#ifndef _CBUF_H_
#define _CBUF_H_

#include "config.h"

#if USE_COMPRESSED_CIRC_BUF == 0
#include "circbuf.h"
#else
#include "circbuf_12bit.h"
#endif

#endif  /* _CBUF_H_ */


