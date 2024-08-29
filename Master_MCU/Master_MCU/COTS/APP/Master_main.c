/*
* main.c
*
*  Created on: Feb 20, 2024
*      Author: Abdelrahman (Ta'laab) Mohammed
*      Github: https://github.com/AbdelrahmanMohammed002
*/

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO_DRIVER/DIO_Interface.h"
#include "../MCAL/GIE_DRIVER/GIE_Interface.h"
#include "../MCAL/USART_DRIVER/USART_Interface.h"
#include "../MCAL/TIMER1_DRIVER/TIMER1_Interface.h"
#include "../MCAL/EEPROM_DRIVER/EEPROM_Interface.h"

#include "../HAL/KPD_DRIVER/KPD_Interface.h"
#include "../HAL/LCD_DRIVER/LCD_Interface.h"

#include <util/delay.h>

#define F_CPU 8000000UL
#define PASSWORD_SIZE 6 // Modified to accommodate null-terminated string
u8 global_u8SecondsCounter = 0, global_u8TicksCounter = 0, global_u8FailureCounter =0;


typedef enum {
	MASTER_READY,
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
	u8 *local_u8PressedKey = NOT_PRESSED;
	u8 local_u8Counter = 0;

	HLCD_vSendString(A_ScreenString); // Display the prompt on the LCD

	// Main loop to get the password
	while (*local_u8PressedKey != '=') {
		// Inner loop to get each character of the password
		while (local_u8Counter < PASSWORD_SIZE - 1) { // Ensure space for null terminator
			local_u8PressedKey = HKPD_u8GetPressedKey(); // Get the pressed key from the keypad

			HLCD_vMoveTo(1, local_u8Counter); // Move the cursor to the current position

			// Check if a valid key was pressed
			if (*local_u8PressedKey != NOT_PRESSED && *local_u8PressedKey != '=') {
				*(A_PointerToPassword + local_u8Counter) = *local_u8PressedKey; // Store the character in the password buffer

				HLCD_vSendData(*(A_PointerToPassword + local_u8Counter)); // Display the character on the LCD

				_delay_ms(1000); // Delay for readability

				HLCD_vMoveTo(1, local_u8Counter); // Clear the character on the LCD
				HLCD_vSendData(0x2A);

				local_u8Counter++; // Increment the counter
				HLCD_vMoveTo(1, local_u8Counter); // Move the cursor to the next position
				*local_u8PressedKey = NOT_PRESSED; // Clear the pressed key flag
			}
		}
		local_u8PressedKey = HKPD_u8GetPressedKey(); // Check if the '=' key was pressed to exit the loop
	}
	*(A_PointerToPassword + local_u8Counter) = '\0'; // Null-terminate the password string
	HLCD_vDisplayClear(); // Clear the display for next use
}

/**
* Function_vWaitingScreen: Displays a "Please Wait!" message with a loading effect.
*/
void Function_vWaitingScreen(void) {
	u8 local_u8Waiting[] = "PLEASE WAIT!";
	HLCD_vSendString(local_u8Waiting);

	for (u8 i = 13; i <= 16; i++) {
		HLCD_vMoveTo(0, i);
		HLCD_vSendData(0xFF);
		_delay_ms(500);
	}
}

/**
* Function_vGetOption: Retrieves the user's option ('1' or '2') via the keypad.
*
* @param A_u8Option: A pointer to the variable where the selected option will be stored.
*/
void Function_vGetOption(u8 *A_u8Option) {
	u8 *local_u8PressedKey = NOT_PRESSED;

	// Loop until a valid option ('1' or '2') is selected
	while (*local_u8PressedKey != '1' && *local_u8PressedKey != '2') {
		local_u8PressedKey = HKPD_u8GetPressedKey();
	}

	*A_u8Option = *local_u8PressedKey; // Store the selected option
	*local_u8PressedKey = NOT_PRESSED; // Clear the pressed key flag
}


void Function_vCreatePassword(u8 (*A_u8LCDScreens)[16]){
	u8 local_u8EnteredPassword[PASSWORD_SIZE], local_u8ReEnteredPassword[PASSWORD_SIZE];

	// Get the new password from the user
	Function_vGetPassword(A_u8LCDScreens[0], local_u8EnteredPassword);
	Function_vGetPassword(A_u8LCDScreens[1], local_u8ReEnteredPassword);

	// Transmit the password creation request to the Slave MCU
	MUSART_vTransmitString(local_u8EnteredPassword, PASSWORD_SIZE);
	MUSART_vTransmitString(local_u8ReEnteredPassword, PASSWORD_SIZE);

}

void TIMER1_CTC_INT_ISR() {
	if (global_u8TicksCounter == 15) {
		global_u8SecondsCounter++;
		global_u8TicksCounter = 0;
		} else {
		global_u8TicksCounter++;
	}
}

void Function_vSystemFreeze(u8 (*A_u8LCDScreens)[16]){
	// Initialize Timer1 and set callback for channel A CTC interrupt
	MTIMER1_vInit();
	MTIMER1_vChannelASetCTCCallBack(&TIMER1_CTC_INT_ISR);
	MTIMER1_vChannelASetCTCRegister(62500);
	MTIMER1_vChannelAEnableCTCINT();
	
	// Enable global interrupts
	MGIE_vEnableInterrupts();
	
	HLCD_vSendString(A_u8LCDScreens[12]);
	while(global_u8SecondsCounter<60);
	HLCD_vDisplayClear();
	global_u8FailureCounter= 0;
	global_u8SecondsCounter =0;
	MTIMER1_vDisInit();
}

void Function_vOpenDoor(u8 (*A_u8LCDScreens)[16]){
	
	// Initialize Timer1 and set callback for channel A CTC interrupt
	MTIMER1_vInit();
	MTIMER1_vChannelASetCTCCallBack(&TIMER1_CTC_INT_ISR);
	MTIMER1_vChannelASetCTCRegister(62500);
	MTIMER1_vChannelAEnableCTCINT();
		
	// Enable global interrupts
	MGIE_vEnableInterrupts();
	
	// Open the door and display status messages
	HLCD_vSendString(A_u8LCDScreens[7]);
		while(global_u8SecondsCounter<15);

	HLCD_vDisplayClear();
	HLCD_vSendString(A_u8LCDScreens[8]);
		while(global_u8SecondsCounter<19);

	HLCD_vDisplayClear();
	HLCD_vSendString(A_u8LCDScreens[9]);
		while(global_u8SecondsCounter<34);

	HLCD_vDisplayClear();
	global_u8SecondsCounter =0;
	MTIMER1_vDisInit();
}



int main(void) {
	// Initialize MCU peripherals
	MDIO_vSetPinDirection(PORT_D, 0, INPUT);  // RX
	MDIO_vSetPinDirection(PORT_D, 1, OUTPUT); // TX

	HLCD_vInit();
	HKPD_vInit();
	MUSART_vInit(51);

	// Initial handshake with Slave MCU
	MUSART_vTransmit(MASTER_READY);
	while (MUSART_u8Receive() != SLAVE_READY);

	u8 local_u8MasterStatus, local_u8SlaveStatus;

	// LCD screen messages
	u8 local_u8LCDScreens[13][16] = {
		{"Enter Password:"},
		{"Re-Enter Pass:"},
		{"Slave is Busy"},
		{"Create Success!"},
		{"Create Error!"},
		{"1 : Open Door"},
		{"2 : Change Pass"},
		{"Door Opening!"},
		{"Door is Open!"},
		{"Door Closing!"}, 
		{"Wrong Password!"},
		{"Try Again"},
		{"System is Locked"}
	};

	while (1) {
		// Wait for incoming data from Slave MCU
		while (!MUSART_u8UnreadDate());
		local_u8SlaveStatus = MUSART_u8Receive();

		// Handle the CREATE_PASSWORD operation
		if (local_u8SlaveStatus == 0xFF) {
			
			MUSART_vTransmit(CREATE_PASSWORD);
			
			// Create Passwords
			Function_vCreatePassword(local_u8LCDScreens);
			
			// Wait for the Slave MCU to process the request
			while (!MUSART_u8UnreadDate());
			local_u8SlaveStatus = MUSART_u8Receive();

			// Display success or error message
			Function_vWaitingScreen();
			HLCD_vMoveTo(1, 0);
			HLCD_vSendString(local_u8SlaveStatus == SUCCEEDED ? local_u8LCDScreens[3] : local_u8LCDScreens[4]);
			_delay_ms(5000);
			HLCD_vDisplayClear();
		}

		// Handle the existing password scenario
		else if (local_u8SlaveStatus != 0xFF) {
			HLCD_vSendString(local_u8LCDScreens[5]);
			HLCD_vMoveTo(1, 0);
			HLCD_vSendString(local_u8LCDScreens[6]);

			u8 local_u8Option[1];
			Function_vGetOption(local_u8Option); // Get the user's option

			HLCD_vDisplayClear();
			u8 local_u8EnterPassword[PASSWORD_SIZE];
			Function_vGetPassword(local_u8LCDScreens[0], local_u8EnterPassword); // Get the password

			// Handle OPEN_DOOR or CHANGE_PASSWORD
			MUSART_vTransmit(*local_u8Option == '1' ? OPEN_DOOR : CHANGE_PASSWORD);
			MUSART_vTransmitString(local_u8EnterPassword, PASSWORD_SIZE);

			// Wait for the Slave MCU's response
			while (!MUSART_u8UnreadDate());
			local_u8SlaveStatus = MUSART_u8Receive();

			if (local_u8SlaveStatus == ERROR && global_u8FailureCounter<2) {
				global_u8FailureCounter++;
				HLCD_vSendString(local_u8LCDScreens[10]);
				HLCD_vMoveTo(1,0);
				HLCD_vSendString(local_u8LCDScreens[11]);
				_delay_ms(5000);
				HLCD_vDisplayClear();
			} else if (*local_u8Option == '1' && local_u8SlaveStatus == SUCCEEDED) {
				Function_vOpenDoor(local_u8LCDScreens);
			} else if (*local_u8Option == '2' && local_u8SlaveStatus == SUCCEEDED) {
				// Create Passwords
				Function_vCreatePassword(local_u8LCDScreens);
							
				// Wait for the Slave MCU to process the request
				while (!MUSART_u8UnreadDate());
				local_u8SlaveStatus = MUSART_u8Receive();

				// Display success or error message
				Function_vWaitingScreen();
				HLCD_vMoveTo(1, 0);
				HLCD_vSendString(local_u8SlaveStatus == SUCCEEDED ? local_u8LCDScreens[3] : local_u8LCDScreens[4]);
				_delay_ms(5000);
				HLCD_vDisplayClear();
			}else{
				Function_vSystemFreeze(local_u8LCDScreens);
			}
		}
	}

	return 0;
}
