/*
 * Buzzer_Program.c
 *
 *  Created on: Apr 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO_DRIVER/DIO_Interface.h"

#include "Buzzer_Interface.h"

void HBuzzer_vInit(void){

	MDIO_vSetPinDirection(BUZZER_PORT, BUZZER_PIN, OUTPUT);
	MDIO_vSetPinValue(BUZZER_PORT, BUZZER_PIN, LOW);

}

void HBuzzer_vOn(void){

	MDIO_vSetPinValue(BUZZER_PORT, BUZZER_PIN, HIGH);

}

void HBuzzer_vOFF(void) {

	MDIO_vSetPinValue(BUZZER_PORT, BUZZER_PIN, LOW);

}
