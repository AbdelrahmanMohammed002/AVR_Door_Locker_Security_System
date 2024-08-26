/*
 * I2C_Program.c
 *
 *  Created on: Mar 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "I2C_Config.h"
#include "I2C_Interface.h"
#include "I2C_Private.h"
#include <math.h>

#define F_CPU 8000000UL

// Function pointer to store interrupt callback
static void (*ptr_to_I2CINT)(void) = NULL;

// Function to initialize I2C master mode
void MI2C_vMasterInit(u32 A_u8BitRate) {

	// Set prescaler based on configuration
#if TWI_PRESCALER_SELECTOR == TWI_PRESCALER_1
	CLR_BIT(TWSR, TWPS0);
	CLR_BIT(TWSR, TWPS1);
#elif TWI_PRESCALER_SELECTOR == TWI_PRESCALER_4
	SET_BIT(TWSR, TWPS0);
	CLR_BIT(TWSR, TWPS1);
#elif TWI_PRESCALER_SELECTOR == TWI_PRESCALER_16
	CLR_BIT(TWSR, TWPS0);
	SET_BIT(TWSR, TWPS1);
#elif TWI_PRESCALER_SELECTOR == TWI_PRESCALER_64
	SET_BIT(TWSR, TWPS0);
	SET_BIT(TWSR, TWPS1);
#endif

	// Calculate TWBR register value for given bit rate
	TWBR = (((F_CPU / A_u8BitRate) - 16) / 2) / pow(4, TWI_PRESCALER_SELECTOR);

	// Enable TWI module
	SET_BIT(TWCR, TWEN);
}

// Function to initialize I2C slave mode
void MI2C_vSlaveInit(u8 A_u8SlaveAddress) {
	// Set slave address and general call enable bit
	TWAR = (A_u8SlaveAddress << 1);
#if GENERAL_CALL == ENABLE
        SET_BIT(TWAR, TWGCE);
    #elif GENERAL_CALL == DISABLE
	CLR_BIT(TWAR, TWGCE);
#endif
	// Enable TWI module
	SET_BIT(TWCR, TWEN);
}

// Function to generate start condition
u8 MI2C_u8StartCondition(u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Generate start condition
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	SET_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);

	// Wait for start condition to be sent
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check start status and update status
	*A_u8Status =
			(TWSR & MASK_PRESCALER_BITS) == START_STATUS ?
					SENT_START : START_ERROR;

	return SUCCESS;
}

// Function to generate repeated start condition
u8 MI2C_u8RepeatedStartCondition(u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Generate repeated start condition
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	SET_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);

	// Wait for repeated start condition to be sent
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check repeated start status and update status
	*A_u8Status =
			(TWSR & MASK_PRESCALER_BITS) == REPEATED_START_STATUS ?
					SENT_REPEATED_START : START_ERROR;

	return SUCCESS;
}

// Function to generate stop condition
void MI2C_vStopCondition(void) {
	// Generate stop condition
	SET_BIT(TWCR, TWINT);
	SET_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);
}

// Function to transmit address and select master transmitter mode
u8 MI2C_MasterTransmitterMode(u8 A_u8SlaveAddress, u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Load slave address into data register and clear read/write bit for write operation
	TWDR = (A_u8SlaveAddress << 1);
	CLR_BIT(TWDR, 0);

	// Clear stop condition and start condition bits
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);
	// Wait for address to be transmitted
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check address transmission status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == ADDRESS_WRITE_ACK_STATUS)
		*A_u8Status = RECEIVED_ACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == ADDRESS_WRITE_NACK_STATUS)
		*A_u8Status = RECEIVED_NACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == ARBITRATION_LOST_STATUS)
		*A_u8Status = ARBITRATION_LOST;
	else
		*A_u8Status = ADDRESS_ERROR;

	return SUCCESS;
}

// Function to transmit data in master mode
u8 MI2C_u8MasterSendByteToSlave(u8 A_u8Data, u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Load data into data register
	TWDR = A_u8Data;

	// Clear stop condition and start condition bits
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);
	// Wait for data to be transmitted
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check transmission status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == SENT_DATA_ACK_STATUS)
		*A_u8Status = RECEIVED_ACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == SENT_DATA_NACK_STATUS)
		*A_u8Status = RECEIVED_NACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == ARBITRATION_LOST_STATUS)
		*A_u8Status = ARBITRATION_LOST;
	else
		*A_u8Status = DATA_ERROR;

	return SUCCESS;
}

// Function to receive address and select master receiver mode
u8 MI2C_u8MasterRecieverMode(u8 A_u8SlaveAddress, u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Load slave address into data register and set read/write bit for read operation
	TWDR = (A_u8SlaveAddress << 1);
	SET_BIT(TWDR, 0);

	// Clear stop condition and start condition bits
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);

	// Wait for address to be transmitted
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check address transmission status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == ADDRESS_READ_ACK_STATUS)
		*A_u8Status = RECEIVED_ACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == ADDRESS_READ_NACK_STATUS)
		*A_u8Status = RECEIVED_NACK;
	else if ((TWSR & MASK_PRESCALER_BITS) == ARBITRATION_LOST_STATUS)
		*A_u8Status = ARBITRATION_LOST;
	else
		*A_u8Status = ADDRESS_ERROR;

	return SUCCESS;
}

// Function to receive data in master mode
u8 MI2C_u8MasterReadByteFromSlave(u8 *A_u8Data, u8 *const A_u8Status,
		const u8 A_u8Response) {
	if (A_u8Status == NULL || A_u8Data == NULL
			|| (A_u8Response != SEND_ACK && A_u8Response != SEND_NACK))
		return ERROR;

	// Send ACK or NACK based on the response parameter
	if (A_u8Response == SEND_ACK)
		SET_BIT(TWCR, TWEA);
	else if (A_u8Response == SEND_NACK)
		CLR_BIT(TWCR, TWEA);

	// Clear stop condition and start condition bits
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);

	// Wait for data to be received
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check received data status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == RECEIVED_DATA_ACK_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == RECEIVED_DATA_NACK_STATUS) {
		*A_u8Status =
				(TWSR & MASK_PRESCALER_BITS) == RECEIVED_DATA_ACK_STATUS ?
						SENT_ACK : SENT_NACK;
		*A_u8Data = TWDR;
	} else {
		*A_u8Status = DATA_ERROR;
	}

	return SUCCESS;
}

// Function to check slave address in slave mode
u8 MI2C_u8SlaveCheckAddress(u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Clear start condition and stop condition bits, and enable ACK
	CLR_BIT(TWCR, TWSTA);
	CLR_BIT(TWCR, TWSTO);
	SET_BIT(TWCR, TWEA);

	// Wait for address to be received
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check address status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == SLA_ADDRESSED_ACK_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == GC_ADDRESSED_ACK_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == LOST_SLA_ADDRESSED_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == LOST_GC_ADDRESSED_STATUS) {
		*A_u8Status = SENT_ACK;
	} else {
		*A_u8Status = ADDRESS_ERROR;
	}

	return SUCCESS;
}

// Function to receive data in slave mode
u8 MI2C_u8SlaveReciverMode(u8 *A_u8Data, u8 *const A_u8Status,
		const u8 A_u8Response) {
	if (A_u8Status == NULL || A_u8Data == NULL)
		return ERROR;

	// Send ACK or NACK based on the response parameter
	if (A_u8Response == SEND_ACK)
		SET_BIT(TWCR, TWEA);
	else if (A_u8Response == SEND_NACK)
		CLR_BIT(TWCR, TWEA);

	// Wait for data to be received
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check received data status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == SLA_ADDRESSED_ACK_DATA_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == GC_ADDRESSED_ACK_DATA_STATUS) {
		*A_u8Status = SENT_ACK;
		*A_u8Data = TWDR;
	} else if ((TWSR & MASK_PRESCALER_BITS) == SLA_ADDRESSED_NACK_DATA_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == GC_ADDRESSED_NACK_DATA_STATUS) {
		*A_u8Status = SENT_NACK;
		*A_u8Data = TWDR;
	} else {
		*A_u8Status = DATA_ERROR;
	}

	return SUCCESS;
}

// Function to transmit data in slave mode
u8 MI2C_u8RecieveSendByteToMaster(u8 A_u8Data, u8 *const A_u8Status) {
	if (A_u8Status == NULL)
		return ERROR;

	// Load data into data register
	TWDR = A_u8Data;

	// Clear stop condition and start condition bits
	SET_BIT(TWCR, TWINT);
	CLR_BIT(TWCR, TWSTO);
	CLR_BIT(TWCR, TWSTA);
	// Enable TWI module
	SET_BIT(TWCR, TWEN);

	// Wait for data to be transmitted
	while (!GET_BIT(TWCR, TWINT))
		;

	// Check transmission status and update status
	if ((TWSR & MASK_PRESCALER_BITS) == SLAVE_SENT_ACK_STATUS
			|| (TWSR & MASK_PRESCALER_BITS) == SLAVE_LAST_DATA_ACK_STATUS) {
		*A_u8Status = RECEIVED_ACK;
	} else if ((TWSR & MASK_PRESCALER_BITS) == SLAVE_SENT_NACK_STATUS) {
		*A_u8Status = RECEIVED_NACK;
	} else {
		*A_u8Status = DATA_ERROR;
	}

	return SUCCESS;
}

// Function to enable I2C interrupts
void MI2C_vInterruptEnable(void) {
	SET_BIT(TWCR, TWIE);
}

// Function to disable I2C interrupts
void MI2C_vInterruptDisable(void) {
	CLR_BIT(TWCR, TWIE);
}

// Function to set callback for I2C interrupt
void MI2C_vSetCallBack(void (*ptr_to_fun)(void)) {
	ptr_to_I2CINT = ptr_to_fun;
}

// ISR for I2C interrupt
void __vector_19(void) {
	if (ptr_to_I2CINT != NULL)
		ptr_to_I2CINT();
}
