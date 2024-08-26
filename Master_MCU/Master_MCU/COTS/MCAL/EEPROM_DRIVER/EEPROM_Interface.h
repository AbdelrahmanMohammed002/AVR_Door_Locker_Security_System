/*
 * EEPROM_Interface.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef MCAL_EEPROM_DRIVER_EEPROM_INTERFACE_H_
#define MCAL_EEPROM_DRIVER_EEPROM_INTERFACE_H_

void MEEPROM_vWriteByte(u16 A_u16Address, u8 A_u8Date);

void MEEPROM_vWriteString(u16 A_u16Address, u8 *A_u8String);

u8 MEEPROM_u8ReadByte(u16 A_u16Address);

void MEEPROM_vInterruptEnable(void);

void MEEPROM_vInterruptDisable(void);

/**
 * @brief Set the callback function to be executed upon EEPROM interrupt.
 *
 * @param ptr_to_fun: Pointer to the callback function.
 */
void MEEPROM_vSetCallBack(void (*CallBack)(void));

#endif /* MCAL_EEPROM_DRIVER_EEPROM_INTERFACE_H_ */
