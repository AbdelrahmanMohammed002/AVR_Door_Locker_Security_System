/*
 * USART_Private.h
 *
 *  Created on: Mar 2, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef MCAL_USART_DRIVER_USART_PRIVATE_H_
#define MCAL_USART_DRIVER_USART_PRIVATE_H_

#define UDR_T	*((volatile u8 *)0x2C)
#define UDR_R 	*((volatile u8 *)0x2C)
#define UCSRA	*((volatile u8 *)0x2B)
#define UCSRB	*((volatile u8 *)0x2A)
#define UCSRC	*((volatile u8 *)0x40)
#define UBRRL	*((volatile u8 *)0x29)
#define UBRRH	*((volatile u8 *)0x40)

//UCSRA register bits
#define RXC		7
#define TXC		6
#define UDRE	5
#define FE		4
#define DOR		3
#define PE		2
#define U2X		1
#define MPCM	0

//UCSRB register bits
#define RXCIE	7
#define TXCIE	6
#define UDRIE	5
#define RXEN	4
#define TXEN	3
#define UCSZ2	2
#define RXB8	1
#define TXB8	0

//UCSRC register bits
#define URSEL	7
#define UMSEL	6
#define UPM1	5
#define UPM0	4
#define USBS	3
#define UCSZ1	2
#define UCSZ0	1
#define UCPOL	0

#define USART_PARITY_MODE_DISABLED	0
#define USART_PARITY_MODE_EVEN		2
#define USART_PARITY_MODE_ODD		3

#define USART_CHARACTER_SIZE_5BITS	0
#define USART_CHARACTER_SIZE_6BITS	1
#define USART_CHARACTER_SIZE_7BITS	2
#define USART_CHARACTER_SIZE_8BITS	3
#define USART_CHARACTER_SIZE_9BITS	7

#define USART_MODE_ASYNC	0
#define USART_MODE_SYNC		1

#define USART_STOP_BIT_1	0
#define USART_STOP_BIT_2	1



void __vector_13(void) __attribute__((signal));
void __vector_14(void) __attribute__((signal));
void __vector_15(void) __attribute__((signal));

#endif /* MCAL_USART_DRIVER_USART_PRIVATE_H_ */
