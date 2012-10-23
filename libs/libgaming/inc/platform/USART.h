#ifndef _USART_H_
#define _USART_H_

/*! @header USART.h
    @discussion USART stands for "universal synchronous/asynchronous receiver/transmitter"
    @discussion This is used to debug your code via a serial console. Do not use this directly, use printf() instead.
    @discussion On your Computer do the following to communicate with your STM32
    <tt>
    @textblock
# start screen. the device may vary. ask or google for help.
$ screen /dev/ttyUSB0 115200

# to end the screen session:
^A 
K
Y
    @/textblock
    </tt>
 */

#include <stdint.h>

/*! @function MyUSART_Init
	Initializes USART on PA2/PA3
	@discussion This is used to debug your code via a serial console
 */
void MyUSART_Init(void);

/*! @function USART_SendString
	Sends a C-String (e.g. a array of chars) via USART to the computer
	@param s pointer to a string
 */
void USART_SendString(char* s);

/*! @function USART_SendChar
	sends a single char (e.g. a array of chars) via USART to the computer
	@param c a char
 */
void USART_SendChar(char c);

/*! @function USART_ReceiveChar
	receives a single char (e.g. a array of chars) via USART to the computer
	@param c pointer to the receiving buffer
 */
uint8_t USART_ReceiveChar(char* c);

#endif
