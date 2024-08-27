/*
* main.c
*
*  Created on: Feb 20, 2024
*      Author: Abdelrahman (Ta'laab) Mohammed
*		Github: https://github.com/AbdelrahmanMohammed002
*/

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO_DRIVER/DIO_Interface.h"
#include "../MCAL/GIE_DRIVER/GIE_Interface.h"
#include "../MCAL/USART_DRIVER/USART_Interface.h"
#include "../MCAL/EEPROM_DRIVER/EEPROM_Interface.h"

#include "../HAL/KPD_DRIVER/KPD_Interface.h"
#include "../HAL/LCD_DRIVER/LCD_Interface.h"

#include <util/delay.h>

#define F_CPU 8000000UL
#define PASSWORD_SIZE 5

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


/**
* Function_vGetPassword: Gets a password from the user.
*
* @param A_ScreenString: The string to display on the screen.
* @param A_PointerToPassword: A pointer to the buffer where the password will be stored.
*/
void Function_vGetPassword(u8 *A_ScreenString, u8 *A_PointerToPassword) {
	// Initialize variables
	u8 *local_u8PressedKey = NOT_PRESSED;
	u8 local_u8Counter = 0;

	// Display the prompt on the LCD
	HLCD_vSendString(A_ScreenString);

	// Main loop to get the password
	while (*local_u8PressedKey != '=') {
		// Inner loop to get each character of the password
		while (local_u8Counter < 5) {
			// Get the pressed key from the keypad
			local_u8PressedKey = HKPD_u8GetPressedKey();

			// Move the cursor to the current position
			HLCD_vMoveTo(1, local_u8Counter);

			// Check if a valid key was pressed
			if (*local_u8PressedKey != NOT_PRESSED && *local_u8PressedKey != '=') {
				// Store the character in the password buffer
				*(A_PointerToPassword + local_u8Counter) = *local_u8PressedKey;

				// Display the character on the LCD
				HLCD_vSendData(*(A_PointerToPassword + local_u8Counter));

				// Delay for readability
				_delay_ms(1000);

				// Clear the character on the LCD
				HLCD_vMoveTo(1, local_u8Counter);
				HLCD_vSendData(0x2A);

				// Increment the counter
				local_u8Counter++;

				// Move the cursor to the next position
				HLCD_vMoveTo(1, local_u8Counter);

				// Clear the pressed key flag
				*local_u8PressedKey = NOT_PRESSED;
			}
		}

		// Check if the '=' key was pressed to exit the loop
		local_u8PressedKey = HKPD_u8GetPressedKey();
	}
	// Display Clear for next use
	HLCD_vDisplayClear();
}



void Function_vWaitingScreen(void) {
	u8 local_u8Waiting[] = "PLEASE WAIT!";
	HLCD_vSendString(local_u8Waiting);

	HLCD_vSendData(0xFF);
	HLCD_vMoveTo(0, 13);
	_delay_ms(500);

	HLCD_vSendData(0xFF);
	HLCD_vMoveTo(0, 14);
	_delay_ms(500);

	HLCD_vSendData(0xFF);
	HLCD_vMoveTo(0, 15);
	_delay_ms(500);

	HLCD_vSendData(0xFF);
	HLCD_vMoveTo(0, 16);
	_delay_ms(500);

}




void Function_vGetOption(u8 * A_u8Option){
	
	u8 * local_u8PressedKey = NOT_PRESSED;
	
	while(*local_u8PressedKey != '1' && *local_u8PressedKey != '2'){
		local_u8PressedKey= HKPD_u8GetPressedKey();
	}
	
	*A_u8Option = *local_u8PressedKey;
	*local_u8PressedKey = NOT_PRESSED;
}





int main(void) {
	
	MDIO_vSetPinDirection(PORT_D, 0, INPUT);//RX
	MDIO_vSetPinDirection(PORT_D, 1, OUTPUT);//\TX

	HLCD_vInit();
	HKPD_vInit();
	MUSART_vInit(51);
	
	MUSART_vTransmit(MASTER_READY);
	while (MUSART_u8Receive()!=SLAVE_READY);
	
	u8 local_u8MasterStatus,  local_u8SlaveStatus;
	
	
	u8 local_u8LCDScreens[10][16] ={
		{"Enter Password:"},
		{"Re-Enter Pass:"},
		{"Slave is Busy"},
		{"Create Success!"},
		{"Create Error!"},
		{"1 : Open Door"},
		{"2 : Change Pass"},
		{"Door Opening!"},
		{"Door is Open!"},
		{"Door Closing!"}
	};
	

	while (1) {
		
		while(!MUSART_u8UnreadDate());
		local_u8SlaveStatus = MUSART_u8Receive();
		
		if (local_u8SlaveStatus == 0xFF)//create new password
		{
			u8 local_u8EnteredPassword[5], local_u8ReEnteredPassword[5];
			Function_vGetPassword(local_u8LCDScreens[0], local_u8EnteredPassword);
			Function_vGetPassword(local_u8LCDScreens[1], local_u8ReEnteredPassword);
			MUSART_vTransmit(CREATE_PASSWORD);
			MUSART_vTransmitString(local_u8EnteredPassword, PASSWORD_SIZE);
			MUSART_vTransmitString(local_u8ReEnteredPassword, PASSWORD_SIZE);
			while(!MUSART_u8UnreadDate());
			local_u8SlaveStatus = MUSART_u8Receive();
			if (local_u8SlaveStatus == SUCCEEDED)
			{
				Function_vWaitingScreen();
				HLCD_vMoveTo(1, 0);
				HLCD_vSendString(local_u8LCDScreens[3]);
				_delay_ms(5000);
				HLCD_vDisplayClear();
				
			}
			else if(local_u8SlaveStatus == ERROR)
			{
				Function_vWaitingScreen();
				HLCD_vMoveTo(1, 0);
				HLCD_vSendString(local_u8LCDScreens[4]);
				_delay_ms(5000);
				HLCD_vDisplayClear();

			}
			
			
			
		}
		else if (local_u8SlaveStatus == 0x00)// system is already used
		{
			HLCD_vSendString(local_u8LCDScreens[5]);
			HLCD_vMoveTo(1, 0);
			HLCD_vSendString(local_u8LCDScreens[6]);
			
			u8 local_u8Option[1];
			Function_vGetOption(local_u8Option);
			
			HLCD_vDisplayClear();
			u8 local_u8EnterPassword[5];
			Function_vGetPassword(local_u8LCDScreens[0], local_u8EnterPassword);
			
			if (*local_u8Option == '1')
			{
				MUSART_vTransmit(OPEN_DOOR);
			}else if(*local_u8Option == '2')
			{
				MUSART_vTransmit(CHANGE_PASSWORD);
			}
			MUSART_vTransmitString(local_u8EnterPassword, PASSWORD_SIZE);
			while(!MUSART_u8UnreadDate());
			local_u8SlaveStatus = MUSART_u8Receive();
			if (local_u8SlaveStatus == ERROR)
			{
				HLCD_vSendString(local_u8LCDScreens[2]);
				
			}
			
			
			if (*local_u8Option == '1' && local_u8SlaveStatus == SUCCEEDED)
			{
				// Open Door CW for 15 seconds
				HLCD_vSendString(local_u8LCDScreens[7]);
				_delay_ms(15000);
				HLCD_vDisplayClear();
				// Wait for 3 seconds
				HLCD_vSendString(local_u8LCDScreens[8]);
				_delay_ms(3000);
				HLCD_vDisplayClear();
				// Close Door ACW for 15 seconds
				HLCD_vSendString(local_u8LCDScreens[9]);
				_delay_ms(15000);
				HLCD_vDisplayClear();

			}
			if(*local_u8Option == '2' )
			{
				
				
			}
			
			
			
		}

	}

	return 0;
}
