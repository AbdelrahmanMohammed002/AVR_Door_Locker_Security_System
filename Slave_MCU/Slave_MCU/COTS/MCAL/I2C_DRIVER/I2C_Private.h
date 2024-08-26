/*
 * I2C_Private.h
 *
 *  Created on: Mar 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#ifndef MCAL_I2C_DRIVER_I2C_PRIVATE_H_
#define MCAL_I2C_DRIVER_I2C_PRIVATE_H_

#include "../../LIB/STD_TYPES.h"

// Register addresses
#define TWBR  	 *((volatile u8 *)0x20)
#define TWCR  	 *((volatile u8 *)0x56)
#define TWSR  	 *((volatile u8 *)0x21)
#define TWDR  	 *((volatile u8 *)0x23)
#define TWAR  	 *((volatile u8 *)0x22)

// Bit positions in TWCR register
#define TWINT   7
#define TWEA   6
#define TWSTA   5
#define TWSTO   4
#define TWWC   3
#define TWEN   2
#define TWIE   0

// Bit positions in TWSR register
#define TWPS1   1
#define TWPS0   0

// Bit position in TWAR register
#define TWGCE   0

// I2C prescaler values
#define TWI_PRESCALER_1     0
#define TWI_PRESCALER_4     1
#define TWI_PRESCALER_16    2
#define TWI_PRESCALER_64    3

// Enable/Disable values
#define ENABLE      1
#define DISABLE     0

// Status codes
#define START_STATUS                    0x08
#define REPEATED_START_STATUS           0x10
#define ADDRESS_WRITE_ACK_STATUS        0x18
#define ADDRESS_WRITE_NACK_STATUS       0x20
#define ADDRESS_READ_ACK_STATUS         0x40
#define ADDRESS_READ_NACK_STATUS        0x48
#define SENT_DATA_ACK_STATUS            0x28
#define SENT_DATA_NACK_STATUS           0x30
#define RECEIVED_DATA_ACK_STATUS        0x50
#define RECEIVED_DATA_NACK_STATUS       0x58
#define ARBITRATION_LOST_STATUS        0x38
#define SLA_ADDRESSED_ACK_STATUS       0x60
#define GC_ADDRESSED_ACK_STATUS        0x70
#define LOST_SLA_ADDRESSED_STATUS      0x68
#define LOST_GC_ADDRESSED_STATUS       0x78
#define SLA_ADDRESSED_ACK_DATA_STATUS  0x80
#define GC_ADDRESSED_ACK_DATA_STATUS   0x90
#define SLA_ADDRESSED_NACK_DATA_STATUS 0x88
#define GC_ADDRESSED_NACK_DATA_STATUS  0x98
#define SLA_ADDRESSED_READ_ACK_STATUS  0xA8
#define LOST_SLA_ADDRESSED_READ_STATUS 0xB0
#define SLAVE_SENT_ACK_STATUS          0xB8
#define SLAVE_SENT_NACK_STATUS         0xC0
#define SLAVE_LAST_DATA_ACK_STATUS     0xC8

// Mask for prescaler bits in status register
#define MASK_PRESCALER_BITS    0xF8

// I2C interrupt vector
void __vector_19() __attribute__((signal));

#endif /* MCAL_I2C_DRIVER_I2C_PRIVATE_H_ */
