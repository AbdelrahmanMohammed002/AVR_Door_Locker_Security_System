/*
 * USART_Program.c
 *
 *  Created on: Mar 2, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "USART_Interface.h"
#include "USART_Private.h"
#include "USART_Config.h"

void (*USART_RXC_Call_Back)(void) = NULL;
void (*USART_TXC_Call_Back)(void) = NULL;
void (*USART_UDRE_Call_Back)(void) = NULL;

void MUSART_vInit(u16 A_u16BaudRate) {

	//set the baud rate
	UBRRL = (u8) (A_u16BaudRate);
	UBRRH = (u8) (A_u16BaudRate >> 8);

	//configure the UCSR-C register
	u8 UCSRC_Value;

	//select register UCSR-C
	SET_BIT(UCSRC_Value, URSEL);

	//select USART MODE (a-sync, sync)
#if	USART_MODE	==	USART_MODE_ASYNC
	CLR_BIT(UCSRC_Value, UMSEL);
	CLR_BIT(UCSRC_Value, UCPOL); //clock polarity bit
#elif USART_MODE	==	USART_MODE_SYNC
	SET_BIT(UCSRC_Value, UMSEL);
	SET_BIT(UCSRC_Value, UCPOL);// clock polarity bit

#endif

#if	USART_PARITY_MODE	==	USART_PARITY_MODE_DISABLED
	CLR_BIT(UCSRC_Value, UPM0);
	CLR_BIT(UCSRC_Value, UPM1);
#elif USART_PARITY_MODE	==	USART_PARITY_MODE_EVEN
	CLR_BIT(UCSRC_Value, UPM0);
	SET_BIT(UCSRC_Value, UPM1);
#elif USART_PARITY_MODE	==	USART_PARITY_MODE_ODD
	SET_BIT(UCSRC_Value, UPM0);
	SET_BIT(UCSRC_Value, UPM1);
#endif

#if	USART_STOP_BIT	==	USART_STOP_BIT_1
	CLR_BIT(UCSRC_Value, USBS);
#elif USART_STOP_BIT	==	USART_STOP_BIT_2
	SET_BIT(UCSRC_Value, USBS);
#endif

#if	USART_CHARACTER_SIZE	==	USART_CHARACTER_SIZE_5BITS
	CLR_BIT(UCSRC_Value, UCSZ0);
	CLR_BIT(UCSRC_Value, UCSZ1);
	CLR_BIT(UCSRB, UCSZ2);
#elif USART_CHARACTER_SIZE	==	USART_CHARACTER_SIZE_6BITS
	SET_BIT(UCSRC_Value, UCSZ0);
	CLR_BIT(UCSRC_Value, UCSZ1);
	CLR_BIT(UCSRB, UCSZ2);
#elif USART_CHARACTER_SIZE	==	USART_CHARACTER_SIZE_7BITS
	CLR_BIT(UCSRC_Value, UCSZ0);
	SET_BIT(UCSRC_Value, UCSZ1);
	CLR_BIT(UCSRB, UCSZ2);
#elif USART_CHARACTER_SIZE	==	USART_CHARACTER_SIZE_8BITS
	SET_BIT(UCSRC_Value, UCSZ0);
	SET_BIT(UCSRC_Value, UCSZ1);
	CLR_BIT(UCSRB, UCSZ2);
#elif USART_CHARACTER_SIZE	==	USART_CHARACTER_SIZE_9BITS
	SET_BIT(UCSRC_Value, UCSZ0);
	SET_BIT(UCSRC_Value, UCSZ1);
	SET_BIT(UCSRB, UCSZ2);
#endif

	UCSRC = UCSRC_Value;		//configuration of frame format

	//enable transmitter and receiver
	SET_BIT(UCSRB, RXEN);
	SET_BIT(UCSRB, TXEN);

}

void MUSART_vTransmit(u8 A_u8Data) {
	/* Wait for empty transmit buffer */
	while ( GET_BIT(UCSRA, UDRE) == 0)
		;
#if USART_CHARACTER_SIZE == USART_CHARACTER_SIZE_9BITS
	CLR_BIT(UCSRB, TXB8);
		if(A_u8Data &0x0100){
			SET_BIT(UCSRA,TXB8);
		}
#endif
	/* Put data into buffer, sends the data */
	UDR_T = A_u8Data;
}

#if USART_CHARACTER_SIZE != USART_CHARACTER_SIZE_9BITS

// Function to receive 8-bit data
u8 MUSART_u8Receive(void) {
    /* Wait for data to be received */
    while (!GET_BIT(UCSRA, RXC))
        ;

    // If error, return -1
    if (GET_BIT(UCSRA, FE) || GET_BIT(UCSRA, DOR) || GET_BIT(UCSRA, PE))
        return -1;

    // Get received data from UDR_R and return
    return UDR_R;
}
#elif USART_CHARACTER_SIZE == USART_CHARACTER_SIZE_9BITS

// Function to receive 9-bit data
u16 MUSART_u16Receive_9bit(void) {
    /* Wait for data to be received */
    while (!GET_BIT(UCSRA, RXC))
        ;

    // If error, return -1
    if (GET_BIT(UCSRA, FE) || GET_BIT(UCSRA, DOR) || GET_BIT(UCSRA, PE))
        return -1;

    // Get RXB8 bit from UCSRB
    u8 local_u8resultHigh = GET_BIT(UCSRB, RXB8);

    // Get received data from UDR_R and return 16-bit result
    return ((local_u8resultHigh << 8) | UDR_R);
}
#endif

u8 MUSART_u8UnreadDate(void){
	return GET_BIT(UCSRA, RXC);
}

void MUSART_vTransmitString(u8 *A_u8StringArray, u8 A_u8StringLength) {
	for (u8 local_u8Counter = 0; local_u8Counter < A_u8StringLength; local_u8Counter++) {
		MUSART_vTransmit(*(A_u8StringArray+local_u8Counter));
	}
}

void MUSART_vRecieveString(u8 *A_u8StringArray, u8 A_u8StringLength) {
	for (u8 local_u8Counter = 0; local_u8Counter < A_u8StringLength; local_u8Counter++) {
		A_u8StringArray[local_u8Counter] = MUSART_u8Receive();
	}
}

void MUSART_RXC_INTEnable(void) {
	SET_BIT(UCSRB, RXCIE);

}
void MUSART_RXC_INTDisable(void) {
	CLR_BIT(UCSRB, RXCIE);

}
void MUSART_TXC_INTEnable(void) {
	SET_BIT(UCSRB, TXCIE);

}
void MUSART_TXC_INTDisable(void) {
	CLR_BIT(UCSRB, RXEN);

}
void MUSART_UDRE_INTEnable(void) {
	SET_BIT(UCSRB, UDRIE);

}
void MUSART_UDRE_INTDisable(void) {
	CLR_BIT(UCSRB, UDRIE);

}

void MUSART_vSetRXCCallBack(void (*CallBack)(void)) {
	if (CallBack != NULL) {
		USART_RXC_Call_Back = CallBack;
	}
}
void MUSART_vSetTXCCallBack(void (*CallBack)(void)) {
	if (CallBack != NULL) {
		USART_TXC_Call_Back = CallBack;
	}
}
void MUSART_vSetUDRECallBack(void (*CallBack)(void)) {
	if (CallBack != NULL) {
		USART_UDRE_Call_Back = CallBack;
	}
}

void __vector_13(void) {
	if (USART_RXC_Call_Back != NULL)
		USART_RXC_Call_Back();
}

void __vector_14(void) {
	if (USART_UDRE_Call_Back != NULL)
		USART_UDRE_Call_Back();
}

void __vector_15(void) {
	if (USART_TXC_Call_Back != NULL)
		USART_TXC_Call_Back();
}

