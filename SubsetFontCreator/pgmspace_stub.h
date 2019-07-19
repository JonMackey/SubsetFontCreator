#ifndef __PGMSPACE_H_
#define __PGMSPACE_H_ 1

#include <inttypes.h>

#define pgm_read_word_near(address_short) *(uint16_t*)(address_short)

#define memcpy_P(dest, src, len)	memcpy(dest, src, len)

#endif /* __PGMSPACE_H_ */
