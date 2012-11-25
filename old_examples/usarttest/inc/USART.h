#ifndef _USART_H_
#define _USART_H_

#include <stdint.h>

void MyUSART_Init(void);
void USART_SendString(char* s);
void USART_SendChar(char c);
uint8_t USART_ReceiveChar(char* c);

#endif
