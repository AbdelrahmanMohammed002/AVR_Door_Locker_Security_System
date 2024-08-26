/*
 * I2C_Interface.h
 *
 *  Created on: Mar 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef MCAL_I2C_DRIVER_I2C_INTERFACE_H_
#define MCAL_I2C_DRIVER_I2C_INTERFACE_H_

// Operation status
#define SENT_START                  0
#define START_ERROR                 1
#define SENT_REPEATED_START         2
#define REPEATED_START_ERROR        3
#define RECEIVED_ACK                4
#define RECEIVED_NACK               5
#define ARBITRATION_LOST            6
#define ADDRESS_ERROR               7
#define DATA_ERROR                  8
#define SENT_ACK                    9
#define SENT_NACK                   10

// Error and success codes
#define ERROR       0
#define SUCCESS    1

// Acknowledge values
#define SEND_ACK    1
#define SEND_NACK   0

/**
 * @brief Initialize I2C module in master mode.
 *
 * @param A_u8BitRate: Bit Rate .
 */
void MI2C_vMasterInit(u32 A_u8BitRate);

/**
 * @brief Initialize I2C module in slave mode.
 *
 * @param A_u8SlaveAddress: Slave address to be set.
 */
void MI2C_vSlaveInit(u8 A_u8SlaveAddress);

/**
 * @brief Start condition in master transmitter mode.
 *
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: SENT_START, START_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8StartCondition(u8 *const A_u8Status);

/**
 * @brief Repeated start condition in master transmitter mode.
 *
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: SENT_REPEATED_START, REPEATED_START_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8RepeatedStartCondition(u8 *const A_u8Status);

/**
 * @brief Stop condition in master or slave mode.
 */
void MI2C_vStopCondition(void);

/**
 * @brief Transmit data byte to slave in master mode.
 *
 * @param A_u8Data: Data byte to be transmitted.
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: RECEIVED_ACK, RECEIVED_NACK, ARBITRATION_LOST, DATA_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8MasterSendByteToSlave(u8 A_u8Data, u8 *const A_u8Status);

/**
 * @brief Receive data byte from slave in master mode.
 *
 * @param A_u8SlaveAddress: Address of the slave device.
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: RECEIVED_ACK, RECEIVED_NACK, ARBITRATION_LOST, ADDRESS_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8MasterRecieverMode(u8 A_u8SlaveAddress, u8 *const A_u8Status);

/**
 * @brief Read data byte from slave in master mode.
 *
 * @param A_u8Data: Pointer to a variable to store the received data byte.
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: SENT_ACK, SENT_NACK, DATA_ERROR.
 * @param A_u8Response: Response to be sent after receiving data byte.
 *                      Possible values: SEND_ACK or SEND_NACK.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8MasterReadByteFromSlave(u8 *A_u8Data, u8 *const A_u8Status, const u8 A_u8Response);

/**
 * @brief Check if addressed in slave mode.
 *
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: SENT_ACK, ADDRESS_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8SlaveCheckAddress(u8 *const A_u8Status);

/**
 * @brief Receive data from master in slave mode.
 *
 * @param A_u8Data: Pointer to a variable to store the received data byte.
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: SENT_ACK, SENT_NACK, DATA_ERROR.
 * @param A_u8Response: Response to be sent after receiving data byte.
 *                      Possible values: SEND_ACK or SEND_NACK.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8SlaveReciverMode(u8 *A_u8Data, u8 *const A_u8Status, const u8 A_u8Response);

/**
 * @brief Transmit data to master in slave mode.
 *
 * @param A_u8Data: Data byte to be transmitted.
 * @param A_u8Status: Pointer to a variable to store the status of the operation.
 *                    Possible values: RECEIVED_ACK, RECEIVED_NACK, DATA_ERROR.
 * @return Operation status: NO_ERROR or ERROR.
 */
u8 MI2C_u8RecieveSendByteToMaster(u8 A_u8Data, u8 *const A_u8Status);

/**
 * @brief Enable I2C interrupt.
 */
void MI2C_vInterruptEnable(void);

/**
 * @brief Disable I2C interrupt.
 */
void MI2C_vInterruptDisable(void);

/**
 * @brief Set callback function for I2C interrupt.
 *
 * @param ptr_to_fun: Pointer to the callback function.
 */
void MI2C_vSetCallBack(void (*ptr_to_fun)(void));

#endif /* MCAL_I2C_DRIVER_I2C_INTERFACE_H_ */
