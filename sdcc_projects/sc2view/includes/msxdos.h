

#ifndef  __MSXDOS_H__
#define  __MSXDOS_H__


#include "msxdos_const.h"


extern byte  getDOSType();
extern dword getDOSVersion();

extern byte  getLastDOSError();

extern byte  open(char *filename, byte mode);
extern byte  creat(char *filename, byte mode, byte attr);
extern byte  close(byte h);
extern byte  dup(byte h);
extern word  read(byte h, void *buffer, word len);
extern word  write(byte h, void *data, word len);
extern dword lseek(byte h, dword offset, byte whence);
extern void  exit(int code);


//Return zero if a match is found, nonzero if none found.
extern word findfirst(const char *pathname, FCB *, int attrib);
//Return zero if there was a match, else nonzero.
extern word findnext(FCB *);


#endif  // __MSXDOS_H__
