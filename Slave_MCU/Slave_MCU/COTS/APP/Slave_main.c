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
#include "../MCAL/EEPROM_DRIVER/EEPROM_Interface.h"
#include "../MCAL/TIMER1_DRIVER/TIMER1_Interface.h"

#include "../HAL/DCMotor_DRIVER/DCMotor_Interface.h"
#include "../HAL/BUZZER_DRIVER/Buzzer_Interface.h"

#define F_CPU 8000000UL
#include <util/delay.h>

#define PASSWORD_SIZE 6  // Modified to accommodate null-terminated string
#define PASSWORD_ADDRESS 0x00 // EEPROM address to store the password
#define FIRST_USE_INDICATOR_ADDRESS 0x10 // EEPROM address to store first-use indicator
#define PIN0 0
#define PIN1 1

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
 * Function_vReceivePassword: Receives a password from the Master MCU.
 *
 * @param A_u8PasswordBuffer: A pointer to the buffer where the received password will be stored.
 */
void Function_vReceivePassword(u8 *A_u8PasswordBuffer) {
    // Receive each character of the password
    MUSART_vRecieveString(A_u8PasswordBuffer, PASSWORD_SIZE);
    *(A_u8PasswordBuffer + PASSWORD_SIZE - 1) = '\0';  // Ensure the string is null-terminated
}

/**
 * Function_u8ComparePasswords: Compares two passwords.
 *
 * @param A_u8Password1: First password buffer.
 * @param A_u8Password2: Second password buffer.
 * @return u8: Returns 1 if passwords match, 0 otherwise.
 */
u8 Function_u8ComparePasswords(u8 *A_u8Password1, u8 *A_u8Password2) {
    for (u8 i = 0; i < PASSWORD_SIZE - 1; i++) {  // Adjusted loop to consider the null-terminator
        if (*(A_u8Password1 + i) != *(A_u8Password2 + i)) {
            return 0;  // Passwords do not match
        }
    }
    return 1;  // Passwords match
}

/**
 * Function_vStorePasswordInEEPROM: Stores the password in the EEPROM.
 *
 * @param A_u8Password: The password to be stored.
 */
void Function_vStorePasswordInEEPROM(u8 *A_u8Password) {
    for (u8 i = 0; i < PASSWORD_SIZE; i++) {
        MEEPROM_vWriteByte(PASSWORD_ADDRESS + i, A_u8Password[i]);  // Write each character to EEPROM
        _delay_ms(10);  // Small delay to ensure EEPROM write completion
    }
    MEEPROM_vWriteByte(FIRST_USE_INDICATOR_ADDRESS, 0x00); // Clear the first-use indicator
    _delay_ms(10);
}

/**
 * Function_vReadPasswordFromEEPROM: Reads the stored password from EEPROM.
 *
 * @param A_u8PasswordBuffer: A pointer to the buffer where the read password will be stored.
 */
void Function_vReadPasswordFromEEPROM(u8 *A_u8PasswordBuffer) {
    for (u8 i = 0; i < PASSWORD_SIZE; i++) {
        A_u8PasswordBuffer[i] = MEEPROM_u8ReadByte(PASSWORD_ADDRESS + i);  // Read each character from EEPROM
        _delay_ms(10);  // Small delay to ensure EEPROM read completion
    }
    A_u8PasswordBuffer[PASSWORD_SIZE - 1] = '\0';  // Ensure the string is null-terminated
}


/**
 * TIMER1_CTC_INT_ISR: Timer1 Interrupt service routine for CTC Mode.
 */
void TIMER1_CTC_INT_ISR() {
	if (global_u8TicksCounter == 15) {
		global_u8SecondsCounter++;
		global_u8TicksCounter = 0;
		} else {
		global_u8TicksCounter++;
	}
}


/**
 * Function_vSystemFreeze: Freeze the System for a minute if password is incorrect for 3 times.
 */
void Function_vSystemFreeze(){
	// Initialize Timer1 and set callback for channel A CTC interrupt
	MTIMER1_vInit();
	MTIMER1_vChannelASetCTCCallBack(&TIMER1_CTC_INT_ISR);
	MTIMER1_vChannelASetCTCRegister(62500);
	MTIMER1_vChannelAEnableCTCINT();
	
	// Enable global interrupts
	MGIE_vEnableInterrupts();
	
	// Initialize & Enable Buzzer
	HBuzzer_vInit();
	HBuzzer_vOn();
	
	// Wait for a minute
	while(global_u8SecondsCounter<60);
	
	// Turn off buzzer
	HBuzzer_vOFF();
	
	// Reset counters and disable Timer1.
	global_u8FailureCounter= 0;
	global_u8SecondsCounter =0;
	MTIMER1_vDisInit();
}

/**
 * Function_vOpenDoor: Handles the process of opening the door.
 */
void Function_vOpenDoor(DCMotor * Door_Motor) {
	
	// Initialize Timer1 and set callback for channel A CTC interrupt
	MTIMER1_vInit();
	MTIMER1_vChannelASetCTCCallBack(&TIMER1_CTC_INT_ISR);
	MTIMER1_vChannelASetCTCRegister(62500);
	MTIMER1_vChannelAEnableCTCINT();
	
	// Enable global interrupts
	MGIE_vEnableInterrupts();
	
    // Door open delay simulation
    Door_Motor->State = CLOCKWISE;
    HDCMotor_RotateDirection_Speed(Door_Motor);
	while(global_u8SecondsCounter<15);
	
    // Door is open 
    Door_Motor->State = STOP;
    HDCMotor_RotateDirection_Speed(Door_Motor);
	while(global_u8SecondsCounter<19);
    
    // Door close delay simulation
    Door_Motor->State = ANTI_CLOCKWISE;
    HDCMotor_RotateDirection_Speed(Door_Motor);
	while(global_u8SecondsCounter<34);
    Door_Motor->State = STOP;
    HDCMotor_RotateDirection_Speed(Door_Motor);
	global_u8SecondsCounter =0 ;
	MTIMER1_vDisInit();
}

/**
 * Function_vCheckFirstUse: Checks if the system is being used for the first time.
 *
 * @return u8: Returns 1 if it is the first use, 0 otherwise.
 */
u8 Function_vCheckFirstUse(void) {
    return (MEEPROM_u8ReadByte(FIRST_USE_INDICATOR_ADDRESS) == 0xFF);
}


int main(void) {
    // Initialize MCU peripherals
    MDIO_vSetPinDirection(PORT_D, 0, INPUT);  // RX
    MDIO_vSetPinDirection(PORT_D, 1, OUTPUT); // TX

    MUSART_vInit(51);  // Initialize USART communication

    // Initial handshake with Master MCU
    while (MUSART_u8Receive() != MASTER_READY);
    MUSART_vTransmit(SLAVE_READY);

    u8 local_u8SystemStatus;
    u8 local_u8StoredPassword[PASSWORD_SIZE], local_u8EnteredPassword[PASSWORD_SIZE], local_u8ReEnteredPassword[PASSWORD_SIZE];

    // Initialize the door motor
    DCMotor Door_Motor = {
	    .input_pin1 = PIN0,
	    .input_pin2 = PIN1,
	    .port_name = PORT_A,
	    .State = STOP
    };
	
	HDCMotor_vInit(&Door_Motor);
    
    // Check if the system is being used for the first time
    u8 local_u8IsFirstUse = Function_vCheckFirstUse();

    // Send the status to the Master MCU
    MUSART_vTransmit(local_u8IsFirstUse ? 0xFF : 0x00);

    while (1) {

        // Wait for Master MCU command
        while (!MUSART_u8UnreadDate());
        local_u8SystemStatus = MUSART_u8Receive();
        
        switch (local_u8SystemStatus) {
            case CREATE_PASSWORD:
                Function_vReceivePassword(local_u8EnteredPassword);
                Function_vReceivePassword(local_u8ReEnteredPassword);

                // Compare the entered passwords
                if (Function_u8ComparePasswords(local_u8EnteredPassword, local_u8ReEnteredPassword)) {
                    // Store the password in EEPROM
                    Function_vStorePasswordInEEPROM(local_u8EnteredPassword);
                    MUSART_vTransmit(SUCCEEDED);  // Notify success
                } else {
                    MUSART_vTransmit(ERROR);  // Notify error in password creation
                }
                break;

            case OPEN_DOOR:
                Function_vReceivePassword(local_u8EnteredPassword);

                // Verify the entered password
                Function_vReadPasswordFromEEPROM(local_u8StoredPassword);
                if (Function_u8ComparePasswords(local_u8StoredPassword, local_u8EnteredPassword)) {
                    MUSART_vTransmit(SUCCEEDED);  // Notify success
                    Function_vOpenDoor(&Door_Motor);  // Open the door
                } else {
                    MUSART_vTransmit(ERROR);  // Notify error
					global_u8FailureCounter ++;
                }
                break;

            case CHANGE_PASSWORD:
                Function_vReceivePassword(local_u8EnteredPassword);

                // Verify the current password
                Function_vReadPasswordFromEEPROM(local_u8StoredPassword);
                if (Function_u8ComparePasswords(local_u8StoredPassword, local_u8EnteredPassword)) {
                    MUSART_vTransmit(SUCCEEDED);  // Notify success
                    Function_vReceivePassword(local_u8EnteredPassword);
                    Function_vReceivePassword(local_u8ReEnteredPassword);

                    // Compare the new passwords
                    if (Function_u8ComparePasswords(local_u8EnteredPassword, local_u8ReEnteredPassword)) {
                        // Store the new password in EEPROM
                        Function_vStorePasswordInEEPROM(local_u8EnteredPassword);
                        MUSART_vTransmit(SUCCEEDED);  // Notify success
                    } else {
                        MUSART_vTransmit(ERROR);  // Notify error in password change
                    }
                } else {
                    MUSART_vTransmit(ERROR);  // Notify error
					global_u8FailureCounter ++;
                }
                break;

            default:
                // Handle unexpected commands
                MUSART_vTransmit(ERROR);
                break;
        }
		
		
		// Handle incorrect password trail for three times
		if (global_u8FailureCounter==3)
		{
			Function_vSystemFreeze();
		}
		
        // After each operation, update the status to indicate whether the system is first-use or not
        local_u8IsFirstUse = Function_vCheckFirstUse();
        MUSART_vTransmit(local_u8IsFirstUse ? 0xFF : 0x00);
    }

    return 0;
}
