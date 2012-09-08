#include <platform/NewlibSyscalls.h>

//#include <stm32f4xx/stm32f4xx.h>

#include <errno.h>
#include <stdio.h>

#include <game/Debug.h>

//FIXME: implement _read
//FIXME: heap-symbol in linkerscript?

#define HEAPSIZE 4096
unsigned char heap[HEAPSIZE];

int _close(int file) { return -1; }
int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _open(const char *name, int flags, int mode) { return -1; }
int _read(int file, char *ptr, int len) { return 0; }

int _fstat(int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _write(int file, char *ptr, int len)
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
