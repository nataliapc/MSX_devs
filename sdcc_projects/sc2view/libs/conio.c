
#include "conio.h"
#include "msx_const.h"
#include <stdarg.h>


static void _printf(const char *, va_list);



/**
    Prompts the user to press a character and that character is not printed on screen.
 */
char getchar() __naked {
}
char getch() __naked
{
	__asm
		ld		c, #MSXDOS_INNOE	;0x08 CONSOLE INPUT WITHOUT ECHO
		call	5
		ret
	__endasm;
}

/**
    Prints characters on the screen.
    Gives the character c to the current text window. It is a text-mode function that displays output directly to the console.
 */
void putchar(char c) __naked {
	c;
}
void putch(char c) __naked
{
	c;
	__asm
		ld      hl, #2
		add     hl, sp
    
		ld		e, (hl)
		ld		c, #MSXDOS_CONOUT	;0x02 CONSOLE OUTPUT
		call	5
		ret
	__endasm;
}

/**

 */
void cputs(char *s)
{
	while (*s != 0) {
		putch(*s);
		s++;
	}
}

/**
    Reads a string from the console.
    Reads a string of characters from the console and stores the string
    (and the string length) in the location pointed to by str.
 */
char *cgets(char *str) __naked
{
	str;
	__asm
		push	ix			; accedemos a la pila
		ld		ix, #0
		add		ix, sp
		ld		e, 4(ix)	; de <- *str
		ld		d, 5(ix)

		push	de			; guardamos *str

		ld		c, #MSXDOS_BUFIN	;0x0a BUFFERED LINE INPUT
		call	5

		pop		hl			; rescatamos *str
		inc		hl
		inc		hl
		push 	hl			; guardamos *str+2

	$cgets1:						; buscamos <CR> y lo sustituimos por '\0'
		ld		a, (hl)
		cp		#0x0d
		jr		z, $cgets2
		inc		hl
		jr		$cgets1
	$cgets2:
		ld		(hl), #0

		pop 	hl			; rescatamos *str+2 para devolver el puntero
		pop		ix			; restauramos ix
		ret
	__endasm;
}

/**
    Reads a character from the keyboard and display it on the screen.
    Reads a single character from the keyboard, and there are direct or via a BIOS call made ​​in the current text window.
 */
char getche()
{
	char c = getch();
	putch(c);
	return c;
}

/**
	Used to find if a key has been pressed or not.
	If no key is pressed return 0x00 else 0xff
 */
int kbhit(void) __naked
{
	__asm
		ld		c, #MSXDOS_CONST	;0x0b CONSOLE STATUS
		call	5
		ret
	__endasm;
}


/**
    Clears the screen and move the cursor to upper left corner of the screen.
 */
void clrscr()
{
	cputs(VT_CLS);
}

/**
    Clears the line from cursor position and don't move it.
 */
void clreol()
{
	cputs(VT_CLREOL);
}

/**
    Clear the cursor line and move all below lines one line up.
 */
void delline()
{
	cputs(VT_DELLINE);
}

/**
    Clear the cursor line and move all below lines one line up.
 */
void insline()
{
	cputs(VT_INSLINE);
}

/**
    Places cursor at a desired location on screen in range 1...80 and 1...24.
    The arguments can't be 0.
 */
void gotoxy(byte x, byte y)
{
	x,y;
	cputs(VT_MOVECURSOR);
	putch(y+31);
	putch(x+31);
}

/**
	Reads the cursor X position.
 */
int wherex()
{
	return ADDR_POINTER(ADDR_CSRX);
}

/**
	Reads the cursor Y position.
 */
int wherey()
{
	return ADDR_POINTER(ADDR_CSRY);
}

/**
	Selecct the cursor visualization: NOCURSOR, NORMALCURSOR, SOLIDCURSOR.
 */
void setcursortype(int type)
{
	type;
	if (type == NOCURSOR) {
		cputs(VT_CURSOROFF);
	} else 
	if (type == NORMALCURSOR) {
		cputs(VT_SELCURSOR);
	} else 
	if (type == SOLIDCURSOR) {
		cputs(VT_BLKCURSOR);
	}
}

/**
	Simple cprintf implementation.
	Supports %c %s %u %d %x %b
 */
int cprintf(const char *format, ...)
{
	va_list va;
	va_start(va, format);

	_printf(format, va);

	/* return printed chars */
	return 0;
}

static void _printn(unsigned u, unsigned base, char issigned)
{
	const char *_hex = "0123456789ABCDEF";
	if (issigned && ((int)u < 0)) {
		putch('-');
		u = (unsigned)-((int)u);
	}
	if (u >= base)
		_printn(u/base, base, 0);
	putch(_hex[u%base]);
}

static void _printf(const char *format, va_list va)
{
	while (*format) {
		if (*format == '%') {
			switch (*++format) {
				case 'c': {
					char c = (char)va_arg(va, int);
					putch(c);
					break;
				}
				case 'u': {
					unsigned u = va_arg(va, unsigned);
					_printn(u, 10, 0);
					break;
				}
				case 'd': {
					unsigned u = va_arg(va, unsigned);
					_printn(u, 10, 1);
					break;
				}
				case 'x': {
					unsigned u = va_arg(va, unsigned);
					_printn(u, 16, 0);
					break;
				}
				case 'b': {
					unsigned u = va_arg(va, unsigned);
					_printn(u, 2, 0);
					break;
				}
				case 's': {
					char *s = va_arg(va, char *);
					while (*s) {
						putch(*s);
						s++;
					}
				}
			}
		} else {
			putch(*format);
		}
	format++;
	}
}



void puthex(int8_t nibbles, uint16_t v) {
	int8_t i = nibbles - 1;
	while (i >= 0) {
		uint16_t aux = (v >> (i << 2)) & 0x000F;
		uint8_t n = aux & 0x000F;
		if (n > 9)
			putch('A' + (n - 10));
		else
			putch('0' + n);
		i--;
	}
}

void puthex8(uint8_t v) {
	puthex(2, (uint16_t) v);
}


void puthex16(uint16_t v) {
	puthex(4, v);
}

void putdec(int16_t digits, uint16_t v) {
	while (digits > 0) {
		uint16_t aux = v / digits;
		uint8_t n = aux % 10;
		putch('0' + n);
		digits /= 10;
	}
}

void putdec8(uint8_t v) {
	putdec(100, (uint16_t) v);
}


void putdec16(uint16_t v) {
	putdec(10000, v);
}
