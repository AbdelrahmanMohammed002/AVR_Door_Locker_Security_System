/*
 * Buzzer_Interface.h
 *
 *  Created on: Apr 16, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef HAL_BUZZER_DRIVER_BUZZER_INTERFACE_H_
#define HAL_BUZZER_DRIVER_BUZZER_INTERFACE_H_

#define BUZZER_PORT PORT_A
#define BUZZER_PIN	3

void HBuzzer_vInit(void);

void HBuzzer_vOn(void);

void HBuzzer_vOFF(void);

#endif /* HAL_BUZZER_DRIVER_BUZZER_INTERFACE_H_ */
