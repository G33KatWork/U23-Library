#ifndef _NEWLIB_SYSCALLS_H_
#define _NEWLIB_SYSCALLS_H_

#include <sys/stat.h>

int _close(int file);
int _isatty(int file);
int _lseek(int file, int ptr, int dir);
int _open(const char *name, int flags, int mode);
int _read(int file, char *ptr, int len) ;
int _fstat(int file, struct stat *st);
int _write(int file, char *ptr, int len);
caddr_t _sbrk(int incr);

#endif
