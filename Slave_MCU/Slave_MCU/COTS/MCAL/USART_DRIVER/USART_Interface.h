/*
 * USART_Interface.h
 *
 *  Created on: Mar 2, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef MCAL_USART_DRIVER_USART_INTERFACE_H_
#define MCAL_USART_DRIVER_USART_INTERFACE_H_

void MUSART_vInit(u16 A_u16BaudRate);

void MUSART_vTransmit(u8 A_u8Data);

u8 MUSART_u8Receive(void);
u16 MUSART_u16Receive_9bit(void);

u8 MUSART_u8UnreadDate(void);

void MUSART_RXC_INTEnable(void);
void MUSART_RXC_INTDisable(void);
void MUSART_TXC_INTEnable(void);
void MUSART_TXC_INTDisable(void);
void MUSART_UDRE_INTEnable(void);
void MUSART_UDRE_INTDisable(void);

void MUSART_vSetRXCCallBack(void (*CallBack)(void));
void MUSART_vSetTXCCallBack(void (*CallBack)(void));
void MUSART_vSetUDRECallBack(void (*CallBack)(void));

#endif /* MCAL_USART_DRIVER_USART_INTERFACE_H_ */
