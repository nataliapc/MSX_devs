

#ifndef  __HEAP_MSXDOS_H__
#define  __HEAP_MSXDOS_H__

#ifdef __HEAP_H__
#error You cannot use both MSXDOS and non-DOS heap functions
#endif

#include "types.h"


extern uint8_t *heap_top;

extern void heap_init(byte *address);
extern void *malloc(word size);


#endif  // __HEAP_MSXDOS_H__
