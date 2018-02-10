

#ifndef  __CONIO_H__
#define  __CONIO_H__


#include "msxdos_const.h"

// Cursor types for 'setcursortype':
#define NOCURSOR       0	//cursor off
#define NORMALCURSOR   1	//half cursor
#define SOLIDCURSOR    2	//block cursor


extern char  getch();
extern void  putch(char c);
extern void  cputs(char *s);
extern char  getchar();
extern void  putchar(char c);

extern char* cgets(char *str);
extern char  getche();
extern int   ungetch(int c);			//TODO
extern int   kbhit();

extern int   cprintf(const char *format, ...);
extern void  cscanf();					//TODO

extern void  clrscr();
extern void  clreol();
extern void  delline();
extern void  insline();
extern void  gotoxy(byte x, byte y);
extern int   wherex();
extern int   wherey();
extern void  setcursortype(int type);

extern void  window();					//TODO
extern int   gettext(int left, int top, int right, int bottom, void *target);	//TODO
extern int   puttext(int left, int top, int right, int bottom, void *source);	//TODO
extern int   movetext(int left, int top, int right, int bottom, int targetLeft, int targetTop);	//TODO

extern void  textcolor();				//TODO
extern void  textbackground();			//TODO

extern void  puthex8(uint8_t v);
extern void  puthex16(uint16_t v);
extern void  putdec8(uint8_t v);
extern void  putdec16(uint16_t v);


#endif  // __CONIO_H__
