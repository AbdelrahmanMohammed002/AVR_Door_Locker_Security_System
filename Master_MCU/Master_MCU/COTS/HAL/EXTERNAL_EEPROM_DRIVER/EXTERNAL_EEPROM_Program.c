/*
 * EEPROM_Program.c
 *
 *  Created on: Mar 18, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/TWI_DRIVER/TWI_Interface.h"
#include "EXTERNAL_EEPROM_Interface.h"
#include "EXTERNAL_EEPROM_Private.h"
#include "EXTERNAL_EERPOM_Config.h"

void HEEPROM_vInit(void) {
	MTWI_vInit();
}

u8 HEEPROM_u8WriteByte(u16 A_u16Address, u8 A_u8Data) {

	u8 local_u8Status;

	// Generate start condition
	MTWI_u8StartCondition(&local_u8Status);
	if (local_u8Status != SENT_START)
		return ERROR;

	// Send EEPROM address and write command
	u8 local_u8SlaveAddress = (u8) (0xA0 | ((A_u16Address & 0x0700) >> 7));
	MTWI_u8MasterTransmitterMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Send EEPROM memory address (16-bit)
	MTWI_u8MasterSendByteToSlave((u8) A_u16Address, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Send data to write
	MTWI_u8MasterSendByteToSlave(A_u8Data, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Generate stop condition
	MTWI_vStopCondition();

	return SUCCESS;

}

u8 HEEPROM_u8ReadByte(u16 A_u16Address, u8 *A_u8Data) {

	u8 local_u8Status;

	// Generate start condition
	MTWI_u8StartCondition(&local_u8Status);
	if (local_u8Status != SENT_START)
		return ERROR;

	// Send EEPROM address and write command
	u8 local_u8SlaveAddress = (u8) (0xA0 | ((A_u16Address & 0x0700) >> 7));
	MTWI_u8MasterTransmitterMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Send EEPROM memory address (16-bit)
	MTWI_u8MasterSendByteToSlave((u8) A_u16Address, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Generate repeated start condition
	MTWI_u8RepeatedStartCondition(&local_u8Status);
	if (local_u8Status != SENT_REPEATED_START)
		return ERROR;

	// Send EEPROM address and read command
	MTWI_u8MasterRecieverMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Read data from EEPROM
	MTWI_u8MasterReadByteFromSlave(&A_u8Data, &local_u8Status, SEND_NACK);
	if (local_u8Status != SENT_NACK)
		return ERROR;

	// Generate stop condition
	MTWI_vStopCondition();

	return SUCCESS;

}

u8 HEEPROM_u8WritePage(u16 A_u16PageAddress,  u8 *A_u8Data,
		u8 A_u8DataLength) {
	u8 local_u8Status;

	// Generate start condition
	MTWI_u8StartCondition(&local_u8Status);
	if (local_u8Status != SENT_START)
		return ERROR;

	// Send EEPROM address and write command
	u8 local_u8SlaveAddress = (u8) (0xA0 | ((A_u16PageAddress & 0x0700) >> 7));
	MTWI_u8MasterTransmitterMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Send EEPROM memory address (16-bit)
	MTWI_u8MasterSendByteToSlave((u8) A_u16PageAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Write data to EEPROM
	for (u8 i = 0; i < A_u8DataLength; i++) {
		MTWI_u8MasterSendByteToSlave(A_u8Data[i], &local_u8Status);
		if (local_u8Status != RECEIVED_ACK)
			return ERROR;
	}

	// Generate stop condition
	MTWI_vStopCondition();

	return SUCCESS;
}

u8 HEEPROM_u8ReadSequential(u16 A_u16StartAddress, u8 *A_u8Data,
		u8 A_u8DataLength) {
	u8 local_u8Status;

	// Generate start condition
	MTWI_u8StartCondition(&local_u8Status);
	if (local_u8Status != SENT_START)
		return ERROR;

	// Send EEPROM address and write command
	u8 local_u8SlaveAddress = (u8) (0xA0 | ((A_u16StartAddress & 0x0700) >> 7));
	MTWI_u8MasterTransmitterMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Send EEPROM memory address (16-bit)
	MTWI_u8MasterSendByteToSlave((u8) A_u16StartAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Generate repeated start condition
	MTWI_u8RepeatedStartCondition(&local_u8Status);
	if (local_u8Status != SENT_REPEATED_START)
		return ERROR;

	// Send EEPROM address and read command
	MTWI_u8MasterRecieverMode(local_u8SlaveAddress, &local_u8Status);
	if (local_u8Status != RECEIVED_ACK)
		return ERROR;

	// Read data from EEPROM
	for (u8 i = 0; i < A_u8DataLength - 1; i++) {
		MTWI_u8MasterReadByteFromSlave(&A_u8Data[i], &local_u8Status,
				SEND_ACK);
		if (local_u8Status != RECEIVED_ACK)
			return ERROR;
	}
	// Read last byte with NACK
	MTWI_u8MasterReadByteFromSlave(&A_u8Data[A_u8DataLength - 1],
			&local_u8Status, SEND_NACK);
	if (local_u8Status != SENT_NACK)
		return ERROR;

	// Generate stop condition
	MTWI_vStopCondition();

	return SUCCESS;
}
