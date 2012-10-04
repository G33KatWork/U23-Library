#include <platform/NewlibSyscalls.h>

//#include <stm32f4xx/stm32f4xx.h>

#include <errno.h>
#include <stdio.h>

#include <game/Debug.h>

//FIXME: implement _read
//FIXME: heap-symbol in linkerscript?

//File operations
int _file_fstat(int fd, struct stat *st);
int _file_write(int fd, void *buf, size_t len);
int _error_write(int fd, void *buf, size_t len);
int _file_read(int fd, void *buf, size_t len);
off_t _file_lseek(int fd, off_t offset, int whence);
int _file_close(int fd);
int _file_isatty(int fd);

//Newlibs syscalls
int _fstat(int fd, struct stat *st);
int _lseek(int fd, int ptr, int dir);
int _read(int fd, char *ptr, int len);
int _close(int fd);
int _open(const char *name, int flags, int mode);
int _isatty(int fd);
int _write(int fd, char *ptr, int len);

int _fstat(int fd, struct stat *st)
{
	if(fd == 0 || fd == 1 || fd == 2)
	{
		st->st_mode = S_IFCHR;
		return 0;
	}
	
	return _file_fstat(fd, st);
}

int _lseek(int fd, int ptr, int dir)
{
	if(fd == 0 || fd == 1 || fd == 2)
		return EBADF;
	
	return _file_lseek(fd, ptr, dir);
}

int _read(int fd, char *ptr, int len)
{
	//TODO: debug read?
	if(fd == 0 || fd == 1 || fd == 2)
		return EBADF;
	
	return _file_read(fd, ptr, len);
}

int _close(int fd)
{
	if(fd == 0 || fd == 1 || fd == 2)
		return EBADF;
	
	return _file_close(fd);
}

int _isatty_r(int fd)
{
	if(fd == 0 || fd == 1 || fd == 2)
		return EBADF;

	return _file_isatty(fd);
}

int _write(int fd, char *ptr, int len)
{
	if(fd == 1 || fd == 2)
	{
		int counter;

		counter = len;
		for (; counter > 0; counter--)
		{
			if (*ptr == 0) break;
			//USART_SendData(USART2, (uint16_t) (*ptr));
			/* Loop until the end of transmission */
			//while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
			DebugPrintChar(*ptr);

			ptr++;
		}
		return len;
	}

	if(fd == 0)
		return EBADF;

	return _file_write(fd, ptr, len);
}




//FIXME: use heap from linkerscript
#define HEAPSIZE 4096
unsigned char heap[HEAPSIZE];
caddr_t _sbrk(int incr);
void _exit(int code);

caddr_t _sbrk(int incr)
{
	printf("sbrk: %x\r\n", incr);
	static unsigned char *heap_end;
	unsigned char *prev_heap_end;
	
	if(heap_end == 0) heap_end = heap;
	prev_heap_end = heap_end;
	
	if(heap_end + incr - heap > HEAPSIZE ) {
		printf("ERR: out of heap memory\r\n");
		errno = ENOMEM;
		return (caddr_t)-1;
	}
	
	heap_end += incr;

	return (caddr_t) prev_heap_end;
}

void _exit(int code)
{
	fprintf(stderr, "Application exited with code: %d\r\n", code);
	while(1);
}




//Dummy file I/O functions
__attribute__((weak))
int _file_fstat(int fd, struct stat *st)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
int _file_write(int fd, void *buf, size_t len)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
int _error_write(int fd, void *buf, size_t len)
{
    return -1;
}

__attribute__((weak))
int _file_read(int fd, void *buf, size_t len)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
off_t _file_lseek(int fd, off_t offset, int whence)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
int _file_close(int fd)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
int _file_isatty(int fd)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}

__attribute__((weak))
int _open(const char *name, int flags, int mode)
{
    fprintf(stderr, "Error: Unimplemented syscall: %s\r\n", __func__);
    return EBADF;
}
