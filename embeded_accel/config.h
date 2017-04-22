/*-----------------------------------------------------------------------------
System configurations
-----------------------------------------------------------------------------*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <assert.h>

#define STATUS_OK	0
#define STATUS_ERR	1

#define MAX_SORTED_ITEMS        32
#define MAX_CIRC_BUF_ITEMS      32

#define USE_COMPRESSED_CIRC_BUF 0

/* Assert Macro - Can be replaced with system specific macros */
#define ASSERT(x) assert(x)

#endif  /* _CONFIG_H_ */



