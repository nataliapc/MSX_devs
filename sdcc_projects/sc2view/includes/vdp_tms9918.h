/* =============================================================================
   VDP_TMS9918.h
   v1.0 (14 February 2014)
   Description:
     Header for VDP TMS9918 MSX Lib.
============================================================================= */
#ifndef  __VDP_TMS9918_H__
#define  __VDP_TMS9918_H__

#include "msx_const.h"
#include "types.h"


#define GR1_BANK0  0x0000
#define GR1_BANK1  0x0800
#define GR1_BANK2  0x1000


/* =============================================================================
 screen
 Function : Sets the display mode of the screen. 
 Input    : (char) - number of screen mode
============================================================================= */
extern void screen(char);


/* =============================================================================
 color
 Function : Specifies the foreground color, background and ink colors. 
 Input    : (char) - ink color
            (char) - background color
            (char) - border color     
============================================================================= */
extern void color(char, char, char);


/* =============================================================================
 vpoke
 Description : Writes a byte to the video RAM. 
 Input  : (uint) - VRAM address
          (byte) - value 
============================================================================= */
extern void vpoke(word, byte);


/* =============================================================================
 vpeek
 Function : Reads data from the video RAM. 
 Input    : (uint) - VRAM address
 Output   : (byte) - value
============================================================================= */ 
extern byte vpeek(word);


/* =============================================================================
 fillVRAM                               
 Function : Fill a large area of the VRAM of the same byte.
            Llena un area de la RAM de video de un mismo byte.
 Input    : (uint) - address of VRAM
            (uint) - blocklength
            (byte) - Value to fill.
============================================================================= */
extern void fillVRAM(word, word, byte);


/* =============================================================================
 copy2VRAM
 Function : Block transfer from memory to VRAM 
 Input    : (uint) address of RAM
            (uint) address of VRAM
            (uint) blocklength
============================================================================= */
extern void copyToVRAM(word, word, word);


/* =============================================================================
 copyFromVRAM
 Function : Block transfer from VRAM to memory
 Input    : (uint) address of VRAM                     
            (uint) address of RAM
            (uint) blocklength            
============================================================================= */
extern void copyFromVRAM(word, word, word);


/* =============================================================================
 setVDP
 Function : writes a value in VDP reg
 Input    : (byte) VDP reg                     
            (byte) value            
============================================================================= */
extern void setVDP(byte, byte);


#endif