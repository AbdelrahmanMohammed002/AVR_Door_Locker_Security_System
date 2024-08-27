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
#define PIN0	0
#define PIN1	1

typedef enum {
	MASTER_READY ,
	SLAVE_READY,
	IDLE,
	CREATE_PASSWORD,
	CHECK_PASSWORD,
	CHANGE_PASSWORD,
	SUCCEEDED,
	ERROR,
	OPEN_DOOR,
} SystemStatus;

int main(int argc, char **argv) {
	
	MDIO_vSetPinDirection(PORT_D, 0, INPUT);//RX
	MDIO_vSetPinDirection(PORT_D, 1, OUTPUT);//\TX

	MUSART_vInit(51);
	
	DCMotor local_DoorMotor;
	local_DoorMotor.port_name = PORT_A;
	local_DoorMotor.input_pin1 = PIN0;
	local_DoorMotor.input_pin2 = PIN1;
	local_DoorMotor.State = STOP;
	HDCMotor_vInit(local_DoorMotor);
	
	while (MUSART_u8Receive()!=MASTER_READY);
	MUSART_vTransmit(SLAVE_READY);
	
	u8 local_u8SlaveStatus , local_u8MasterStatus;
	
	local_u8SlaveStatus = MEEPROM_u8ReadByte(0x0010);
	MUSART_vTransmit(local_u8SlaveStatus);
	
	local_u8SlaveStatus = IDLE;
	local_u8MasterStatus = IDLE;
	
	while (1) {
		
		while(!MUSART_u8UnreadDate());
		local_u8MasterStatus = MUSART_u8Receive();
		
		if (local_u8MasterStatus == CREATE_PASSWORD)
		{

			
			u8 local_u8EnteredPassword[5]; u8 local_u8ReEnteredPassword[5];
			MUSART_vTransmit(local_u8SlaveStatus);
			MUSART_vRecieveString(local_u8EnteredPassword, PASSWORD_SIZE);
			MUSART_vRecieveString(local_u8ReEnteredPassword, PASSWORD_SIZE);
			
			
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
				MEEPROM_vWriteByte(0x0010, 0);
				MUSART_vTransmit(SUCCEEDED);
			}
			else
			{
				MUSART_vTransmit(ERROR);
			}
		}
		
		
		
		
		if (local_u8MasterStatus == CHANGE_PASSWORD || local_u8MasterStatus == OPEN_DOOR)
		{
			u8 local_u8SavedPassword[5], local_u8EnteredPassword[5], local_u8Flag=0 ; static u8 local_u8ErrorCounter ;
			MUSART_vRecieveString(local_u8EnteredPassword, PASSWORD_SIZE);
			for (u8 local_u8Counter =0; local_u8Counter<PASSWORD_SIZE; local_u8Counter++)
			{
				local_u8SavedPassword[local_u8Counter] = MEEPROM_u8ReadByte(0x0000 + local_u8Counter);
			}
			for (u8 local_u8Counter =0; local_u8Counter<PASSWORD_SIZE; local_u8Counter++)
			{
				if (local_u8EnteredPassword[local_u8Counter]!= local_u8SavedPassword[local_u8Counter])
				{
					local_u8Flag++;
					local_u8ErrorCounter++;
					break;
				}
			}
			if (local_u8Flag == 0 && local_u8ErrorCounter < 3  )
			{
				MUSART_vTransmit(SUCCEEDED);
				if (local_u8MasterStatus == OPEN_DOOR)
				{
					// Open Door CW for 15 seconds
					local_DoorMotor.State = CLOCKWISE;
					local_DoorMotor.Speed = 100; 
					HDCMotor_RotateDirection_Speed(local_DoorMotor);
					_delay_ms(15000);
					// Wait for 3 seconds
					local_DoorMotor.State = STOP;
					HDCMotor_RotateDirection_Speed(local_DoorMotor);
					_delay_ms(3000);
					// Close Door ACW for 15 seconds
					local_DoorMotor.State = ANTI_CLOCKWISE;
					HDCMotor_RotateDirection_Speed(local_DoorMotor);
					_delay_ms(15000);
					local_DoorMotor.State = STOP;
					HDCMotor_RotateDirection_Speed(local_DoorMotor);
				}else if (local_u8MasterStatus == CHANGE_PASSWORD)
				{
					MEEPROM_vWriteByte(0x0010, 0xFF);
				}
			} 
			else 
			{
								MUSART_vTransmit(ERROR);

			}
			
			
		}
		
		
		

		local_u8SlaveStatus = MEEPROM_u8ReadByte(0x0010);
		MUSART_vTransmit(local_u8SlaveStatus);
		
	}

}
