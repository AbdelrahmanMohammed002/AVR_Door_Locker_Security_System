/*
 * EEPROM_Interface.h
 *
 *  Created on: Mar 18, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef HAL_EXTERNAL_EEPROM_DRIVER_EXTERNAL_EEPROM_INTERFACE_H_
#define HAL_EXTERNAL_EEPROM_DRIVER_EXTERNAL_EEPROM_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

/**
 * Function: HEEPROM_vInit
 * ------------------------
 *   @brief Initializes the EEPROM driver.
 *   		Must be called before any EEPROM access functions.
 */
void HEEPROM_vInit(void);

/**
 * Function: HEEPROM_u8WriteByte
 * ------------------------------
 *   @brief Writes a single byte of data to the specified EEPROM address.
 *
 *   @param A_u16Address: The address in the EEPROM memory to write the data to.
 *   @param A_u8Data: The data byte to be written.
 *
 *   @return Status of the operation (SUCCESS or ERROR).
 */
u8 HEEPROM_u8WriteByte(u16 A_u16Address, u8 A_u8Data);

/**
 * Function: HEEPROM_u8ReadByte
 * -----------------------------
 *   @brief Reads a single byte of data from the specified EEPROM address.
 *
 *   @param A_u16Address: The address in the EEPROM memory to read the data from.
 *   @param A_u8Data: Pointer to store the read data.
 *
 *   @return Status of the operation (SUCCESS or ERROR).
 */
u8 HEEPROM_u8ReadByte(u16 A_u16Address, u8 *A_u8Data);

/**
 * Function: HEEPROM_u8WritePage
 * ------------------------------
 *   @brief Writes a page of data to the EEPROM starting from the specified address.
 *
 *   @param A_u16PageAddress: The starting address of the page to write data to.
 *   @param A_u8Data: Pointer to the data buffer to be written.
 *   @param A_u8Length: The number of bytes to write (must not exceed the page size).
 *
 *   @return Status of the operation (SUCCESS or ERROR).
 */
u8 HEEPROM_u8WritePage(u16 A_u16PageAddress, u8 *A_u8Data, u8 A_u8Length);

/**
 * Function: HEEPROM_u8ReadSequential
 * -----------------------------------
 *   @brief Reads sequential data from the EEPROM starting from the specified address.
 *
 *   @param A_u16StartAddress: The starting address in the EEPROM memory to start reading from.
 *   @param A_u8Data: Pointer to the buffer to store the read data.
 *   @param A_u8Length: The number of bytes to read.
 *
 *   @return Status of the operation (SUCCESS or ERROR).
 */
u8 HEEPROM_u8ReadSequential(u16 A_u16StartAddress, u8 *A_u8Data, u8 A_u8Length);

#endif /* HAL_EXTERNAL_EEPROM_DRIVER_EXTERNAL_EEPROM_INTERFACE_H_ */
