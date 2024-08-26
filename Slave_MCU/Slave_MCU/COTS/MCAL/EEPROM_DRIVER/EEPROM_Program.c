/*
 * EEPROM_Program.c
 *
 *  Created on: Apr 25, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "EEPROM_Config.h"
#include "EEPROM_Private.h"
#include "EEPROM_Interface.h"

void (*EEPROM_Call_Back)(void) = NULL;


void MEEPROM_vWriteByte(u16 A_u16Address, u8 A_u8Date){
	//wait for last operation to be completed
	while(GET_BIT(EECR, EEWE)!=0);

	//set up address and data register
	EEAR = A_u16Address;
	EEDR = A_u8Date;

	SET_BIT(EECR, EEMWE);//set EEPROM Master Write Enable to One
	SET_BIT(EECR, EEWE);//Start Writing.
}

void MEEPROM_vWriteString(u16 A_u16Address, u8 *A_u8String){

	for(u8 local_u8Counter = 0; A_u8String[local_u8Counter]!= '\0'; local_u8Counter++){
		MEEPROM_vWriteByte(A_u16Address+local_u8Counter, A_u8String[local_u8Counter]);
	}

}

u8 MEEPROM_u8ReadByte(u16 A_u16Address){
	//wait for last operation to be completed
	while(GET_BIT(EECR, EEWE)!=0);

	EEAR = A_u16Address;
	SET_BIT(EECR, EERE);

	return EEDR;
}

void MEEPROM_vInterruptEnable(void){
	SET_BIT(EECR, EERIE);
}

void MEEPROM_vInterruptDisable(void){
	CLR_BIT(EECR, EERIE);
}

/**
 * @brief Set the callback function to be executed upon EEPROM interrupt.
 *
 * @param ptr_to_fun: Pointer to the callback function.
 */

void MEEPROM_vSetCallBack(void (*ptr_to_fun)(void)) {
	if (ptr_to_fun != NULL) {
		EEPROM_Call_Back = ptr_to_fun;
	}
}

void __vector_17(void) {
	if (EEPROM_Call_Back != NULL)
		EEPROM_Call_Back();
}
