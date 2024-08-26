/*
 * I2C_Config.h
 *
 *  Created on: Mar 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */
#ifndef MCAL_I2C_DRIVER_I2C_CONFIG_H_
#define MCAL_I2C_DRIVER_I2C_CONFIG_H_

/*
 * Configure TWI Pre-scaler
 *
 * Options:
 * - TWI_PRESCALER_1
 * - TWI_PRESCALER_4
 * - TWI_PRESCALER_16
 * - TWI_PRESCALER_64
 */
#define TWI_PRESCALER_SELECTOR      TWI_PRESCALER_1

/*
 * Configure General call
 *
 * Options:
 * - ENABLE
 * - DISABLE
 */
#define GENERAL_CALL                DISABLE

#endif /* MCAL_I2C_DRIVER_I2C_CONFIG_H_ */
