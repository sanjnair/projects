/*-----------------------------------------------------------------------------
Accelerometer data functions
\author - Sanjay Nair
-----------------------------------------------------------------------------*/
#ifndef _ACCELDATA_H_
#define _ACCELDATA_H_

#include "config.h"

int accel_init();
struct CircBuf* accel_get_circ_buf();
struct PQ* accel_get_pq();
void accel_process_3_bytes(const uint8_t *buf, uint32_t size);
void accel_process_2_bytes(const uint8_t *buf, uint32_t size);

#endif  /* _ACCELDATA_H_ */
