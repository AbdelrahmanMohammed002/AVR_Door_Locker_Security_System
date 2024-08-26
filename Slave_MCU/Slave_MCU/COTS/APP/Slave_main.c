/*
 * main.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/GIE_DRIVER/GIE_Interface.h"
#include "../MCAL/TIMER1_DRIVER/TIMER1_Interface.h"
#include "../MCAL/DIO_DRIVER/DIO_Interface.h"
#include "../MCAL/USART_DRIVER/USART_Interface.h"
#include "../MCAL/EEPROM_DRIVER/EEPROM_Interface.h"

#include "../HAL/BUZZER_DRIVER/Buzzer_Interface.h"
#include "../HAL/DCMotor_DRIVER/DCMotor_Interface.h"
#include "../HAL/LCD_DRIVER/LCD_Interface.h"

#include <util/delay.h>
#include "stdlib.h"

#define F_CPU 8000000UL
#define PASSWORD_SIZE 5

u8 global_u8Availability = 0;

u8 Match_notMatch(u8 *A_u8EnteredPass, u8 *A_u8ReEnteredPass) {
	u8 local_u8MatchCounter = 0;
	for (u8 local_u8Counter = 0; local_u8Counter < 5; local_u8Counter++) {
		if (*A_u8EnteredPass + local_u8Counter
				== *A_u8ReEnteredPass + local_u8Counter) {
			local_u8MatchCounter++;
		}
	}
	if (local_u8MatchCounter == 5) {
		return 1;
	} else {
		return 0;
	}
}

void UART_Recieve_String(u8 *A_u8StringArray) {
	for (u8 local_u8Counter = 0; local_u8Counter < PASSWORD_SIZE; local_u8Counter++) {
			A_u8StringArray[local_u8Counter] = MUSART_u8Receive();	
	}
}

u8 Function_u8HandlePolling(){
	if (MUSART_u8UnreadDate())
	{
		u8 local_u8Request = MUSART_u8Receive();
		
		if(local_u8Request==0xAA){
			MUSART_vTransmit(0xBB);
			global_u8Availability =1;
		}
	}
}

typedef enum {
	MASTER_READY ,
	SLAVE_READY, 
	IDLE,
	CREATE_PASSWORD,
	CHECK_PASSWORD,
	SUCCEEDED,
	ERROR, 
	OPEN_DOOR,
} SystemStatus;

int main(int argc, char **argv) {
	
	MDIO_vSetPinDirection(PORT_D, 0, INPUT);//RX
	MDIO_vSetPinDirection(PORT_D, 1, OUTPUT);//\TX

	MUSART_vInit(51);
	HLCD_vInit();
	
	while (MUSART_u8Receive()!=MASTER_READY);
	MUSART_vTransmit(SLAVE_READY);
	
	u8 local_u8SlaveStatus , local_u8MasterStatus;
	
	local_u8SlaveStatus = MEEPROM_u8ReadByte(0x0010);
	MUSART_vTransmit(local_u8SlaveStatus);
	
	local_u8SlaveStatus = IDLE;
	local_u8MasterStatus = IDLE;
	u8 Dummy_String[16];
	
	while (1) {
		
		local_u8MasterStatus = MUSART_u8Receive();
		
		if (local_u8MasterStatus == CREATE_PASSWORD)
		{
			{
				itoa(local_u8MasterStatus, Dummy_String, 10);
				HLCD_vSendData(Dummy_String);
				_delay_ms(1000);
				HLCD_vDisplayClear();
			}

			
			u8 local_u8EnteredPassword[5]; u8 local_u8ReEnteredPassword[5];
			MUSART_vTransmit(local_u8SlaveStatus);
			
			UART_Recieve_String(local_u8EnteredPassword);
			UART_Recieve_String(local_u8ReEnteredPassword);
			
			{
				HLCD_vSendString(local_u8EnteredPassword);
				HLCD_vMoveTo(1, 0);
				HLCD_vSendString(local_u8ReEnteredPassword);
			}
			
			u8 local_u8Flag =0;
			
			for (u8 local_u8Counter =0; local_u8Counter <PASSWORD_SIZE; local_u8Counter++)
			{
				if (local_u8EnteredPassword[local_u8Counter]!=local_u8ReEnteredPassword[local_u8Counter])
				{
					local_u8Flag = 1;
					break;
				}
			}
			
			if (local_u8Flag == 0)
			{
				for (u8 local_u8Counter =0 ; local_u8Counter < PASSWORD_SIZE ; local_u8Counter++)
				{
					MEEPROM_vWriteByte(0x0000+local_u8Counter, local_u8EnteredPassword[local_u8Counter]);
				}
				MUSART_vTransmit(SUCCEEDED);
				MEEPROM_vWriteByte(0x0010, 0);
			} 
			else
			{
				MUSART_vTransmit(ERROR);
			}
			
			local_u8SlaveStatus = MEEPROM_u8ReadByte(0x0010);
			MUSART_vTransmit(local_u8SlaveStatus);
		}
		
		
	
		


		
	}

}
