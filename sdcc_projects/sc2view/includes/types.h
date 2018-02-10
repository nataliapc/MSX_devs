/**
    types.h
    Purpose: Define basic data types

    @version 1.1 10/06/2017 
*/

#ifndef  __TYPES_MSX_H__
#define  __TYPES_MSX_H__


typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed int int16_t;
typedef unsigned int uint16_t;
typedef signed long int32_t;
typedef unsigned long uint32_t;
typedef float float32;

#ifndef byte
typedef uint8_t  byte;
#endif
#ifndef word
typedef uint16_t word;
#endif
#ifndef dword
typedef uint32_t dword;
#endif
#ifndef bool
typedef uint8_t bool;
#define true  ((bool) 1)
#define false ((bool) 0)
#endif

union word_byte {
	uint16_t w;
	struct {
		uint8_t lsb;
		uint8_t msb;
	} b;
};

#ifndef  NULL
#define  NULL  ((void *) 0)
#endif


#endif  // __TYPES_MSX_H__
