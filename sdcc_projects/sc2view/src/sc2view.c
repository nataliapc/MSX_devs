/**
    sc2view.c
    Purpose: MSX1 Screen 2 viewer.

    @author Natalia Pujol Cremades
    @version 1.0 10/02/2018
*/
#include "types.h"
#include "msxdos_const.h"
#include "msxdos.h"
#include "conio.h"
#include "vdp_tms9918.h"
#include "heap.h"


typedef struct
{
	byte magic;
	word ini;
	word end;
	word start;
} SC2HEAD;

SC2HEAD *head;
byte *data;

#define BUFFER_SIZE		2048

/**
	MSX-DOS entry point
*/
int main(char** argv, int argc) {
	byte h;
	byte errCode = 0;
	word size;
	argv, argc;

	cprintf("SC2VIEW v1.0\n\rCopyleft 2018 @ishwin74\n\r");

	if (argc == 0) {
		cprintf("Usage: sc2view <filename>\n\r");
		return 0;
	}

	if (getDOSType() == DOSTYPE_MSXDOS1) {
		cprintf("Error: MSXDOS 2.x needed!\n\r");
		return 1;
	}

	cprintf("Opening %s...\r\n", argv[0]);
	h = open(argv[0], O_RDONLY);
	if (getLastDOSError()) {
		cprintf("Error opening file!\r\n");
		return 2;
	}

	head = malloc(sizeof(SC2HEAD));
	read(h, head, sizeof(SC2HEAD));
	if (getLastDOSError()) {
		cprintf("Error reading file!\r\n");
		return 2;
	}

	size = head->end - head->ini;
	data = malloc(size);

	if (head->magic==0xFE && size<=16384) {

		screen(2);
		color(15, 1, 1);

		read(h, data, size);
		copyToVRAM((word)data, head->ini, size);

		getch();

		color(15, 4, 4);
		screen(0);
	} else {
		cprintf("Bad SCREEN 2 file format!\n\r");
		errCode = 4;
	}

	close(h);

	return errCode;
}
