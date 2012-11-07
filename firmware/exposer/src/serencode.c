#include <System.h>

const uint8_t ESCAPE = 0xBA;
const uint8_t ESCAPE_CHAR = 0x40;
const uint8_t SHORT_FRAME_PREFIX = 0x50;
const uint8_t LONG_FRAME_PREFIX = 0x60;

void se_start_frame(uint8_t n)
{
    USART_SendChar(ESCAPE);
    if(n < 0x10) {
        USART_SendChar(SHORT_FRAME_PREFIX | n);
    } else {
        USART_SendChar(LONG_FRAME_PREFIX);
        USART_SendChar(n);
    }
}

void se_putchr(uint8_t c)
{
    if(c == ESCAPE) {
        USART_SendChar(ESCAPE);
        USART_SendChar(ESCAPE_CHAR);
    } else {
        USART_SendChar(c);
    }
}

void se_puti16(uint16_t x)
{
    se_putchr(x & 0x00FF);
    se_putchr((x & 0xFF00) >> 8);
}

void se_puti32(uint32_t x)
{
    se_putchr((x & 0x000000FF));
    se_putchr((x & 0x0000FF00) >>  8);
    se_putchr((x & 0x00FF0000) >> 16);
    se_putchr((x & 0xFF000000) >> 24);
}

void se_putdata(const char* c, int n)
{
    int i;
    for(i=0; i<n; ++i)
        se_putchr(*c++);
}
