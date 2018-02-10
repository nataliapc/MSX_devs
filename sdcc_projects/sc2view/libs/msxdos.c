#include "msxdos.h"


// DOS Type
byte dosType = DOSTYPE_UNRESOLVED;
// Format: 0x00HHMMRR (HH:Main version MM:Minor version RR:Release version)
word dosVersion;
// Store the last error
byte last_error;

// Local variables
byte a;
union word_byte hl, bc, de, ix, iy;


//============================================================================
//============================================================================
//  Pointers to functions
//============================================================================
//============================================================================
/*
byte  (*open)(char *filename, byte mode);
byte  (*creat)(char *filename, byte mode, byte attr);
byte  (*close)(byte h);
byte  (*dup)(byte h);
word  (*read)(byte h, void *buffer, word len);
word  (*write)(byte h, void *data, word len);
dword (*lseek)(byte h, dword offset, byte whence);
void  (*exit)(byte code);
word  (*findfirst)(char *pathname, FCB *, int attrib);
word  (*findnext)(FCB *);
*/
//============================================================================
//============================================================================
//  Support functions
//============================================================================
//============================================================================

void _dosVer()
{
	__asm
		push	bc
		ld		c, #MSXDOS2_DOSVER	;#0x6F
		call 	5

		ld		(_a), a
		ld		(_bc), bc
		pop bc
	__endasm;
	if (bc.b.msb < 2) {
		dosType = DOSTYPE_MSXDOS1;
		dosVersion = 0x0100;
	}
	else
	{
		dosType = DOSTYPE_MSXDOS2;
		dosVersion = bc.w;
	}
}


byte getDOSType()
{
	if (dosType == DOSTYPE_UNRESOLVED) {
		dosType = DOSTYPE_UNKNOWN;
		dosVersion = 0;
		_dosVer();
	}
	return dosType;
}


dword getDOSVersion()
{
	if (dosType == DOSTYPE_UNRESOLVED) {
		dosType = DOSTYPE_UNKNOWN;
		dosVersion = 0;
		_dosVer();
	}
	return dosVersion;
}

//============================================================================
//============================================================================
//  MSX-DOS File methods
//============================================================================
//============================================================================

byte getLastDOSError()
{
	return last_error;
}

/**
 *
 * byte  open(char *filename, byte mode)
 *
 */
byte  open(char *filename, byte mode) __naked
{
	filename, mode;
	__asm
		push 	ix
		ld 		ix,#0
		add 	ix,sp

		ld 		e, 4(ix)			; path
		ld 		d, 5(ix)
		ld 		a, 6(ix)			; mode flags
		ld 		c, #MSXDOS2_OPEN	;0x43
		call 	5

		ld 		(_last_error), a 	; check error
		add 	a, #0
		jp 		z, $openok
		ld 		l, #-1
		jp 		$openend
	$openok:			;open_ok
		ld 		l, b
	$openend:			;open_end
		pop 	ix
		ret
	__endasm;
}

/**
 *
 * byte creat(char *filename, byte mode, byte attr)
 *
 */
byte  creat(char *filename, byte mode, byte attr) __naked
{
	filename, mode, attr;
	__asm
		push 	ix
		ld 		ix, #0
		add 	ix, sp

		ld 		e, 4(ix)			; path
		ld 		d, 5(ix)
		ld 		a, 6(ix)			; flags
		ld 		b, 7(ix)			; attrib
		ld 		c,#MSXDOS2_CREATE	;0x44
		call 	5
		; check error
		ld 		(_last_error), a
		add 	a, #0
		jp 		z, $creat_ok
		ld 		l, #-1
		jp 		$creat_end
	$creat_ok:
		ld 		l, b
	$creat_end:

		pop 	ix
		ret
	__endasm;
}

/**
 *
 * byte close(byte h)
 *
 */
byte  close(byte h) __naked
{
	h;
	__asm
		push 	ix
		ld 		ix, #0
		add 	ix, sp

		ld 		b, 4(ix)			;handle
		ld 		c,#MSXDOS2_CLOSE	;0x45
		call 	5

		ld 		(_last_error), a	;return
		ld 		l, a

		pop 	ix
		ret
	__endasm;
}

/**
 *
 * byte dup(byte h)
 *
 */
byte  dup(byte h) __naked
{
	h;
	__asm
		push 	ix
		ld 		ix, #0
		add 	ix, sp

		ld 		b, 4(ix)			;handle
		ld 		c, #MSXDOS2_DUP		;0x47
		call 	5

		ld 		(_last_error), a 	;return
		add 	a, #0
		jp 		z, $dup_ok
		ld 		l, #-1
		jp 		$dup_end
	$dup_ok:
		ld 		l, b
	$dup_end:

		pop 	ix
		ret	
	__endasm;
}


/**
 *
 * word read(byte h, void *buffer, word len)
 *
 */
word  read(byte h, void *buffer, word len) __naked
{
	h, buffer, len;
	__asm
		push 	ix
		ld 		ix, #0
		add 	ix, sp

		ld 		b, 4(ix)				;handle
		ld 		e, 5(ix)				;buffer
		ld 		d, 6(ix)
		ld 		l, 7(ix)				;bytes
		ld 		h, 8(ix)
		ld 		c, #MSXDOS2_READ		;0x48
		call 	5
		
		ld 		(_last_error), a 		;return
		add 	a, #0
		jp 		z, $read_end
		ld 		h, #-1
		ld 		l, #-1
	$read_end:
		pop 	ix
		ret
	__endasm;
}


/**
 *
 * word write(byte h, void *data, word len)
 *
 */
word  write(byte h, void *data, word len)
{
	h, data, len;
	//TODO
	return ERR_ABORT;
}


/**
 *
 * dword lseek(byte h, dword offset, byte whence)
 *
 */
dword lseek(byte h, dword offset, byte whence)
{
	h, offset, whence;
	//TODO
	return ERR_ABORT;
}


/**
 *
 * void exit(byte code)
 *
 */
void exit(int code) __naked
{
	code;
	__asm
		push 	ix
		ld 		ix, #0
		add 	ix, sp

		ld 		b, 4(ix)
		ld 		c, #MSXDOS2_TERM	;0x62
		call 	5

		pop 	ix
		ret
	__endasm;
}
