#include <sys/stat.h>
#include <stm32f4xx/stm32f4xx.h>

//FIXME: implement _read

int _close(int file) { return -1; }
int _isatty(int file) { return 1; }
int _lseek(int file, int ptr, int dir) { return 0; }
int _open(const char *name, int flags, int mode) { return -1; }
caddr_t _sbrk(int incr) { return (caddr_t)0; }
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
		USART_SendData(USART2, (uint16_t) (*ptr));
		/* Loop until the end of transmission */
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
		ptr++;
	}
	return len;
}
