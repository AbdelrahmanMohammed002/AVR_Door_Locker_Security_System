/*
 * KDP_Interface.h
 *
 *  Created on: Aug 25, 2023
 *      Author: Abdelrahman (Ta'laab) Mohammed
 */

#ifndef COTS_HAL_KPD_DRIVER_KPD_INTERFACE_H_
#define COTS_HAL_KPD_DRIVER_KPD_INTERFACE_H_

#define NOT_PRESSED 0


void HKPD_vInit(void);

u8 *HKPD_u8GetPressedKey(void);


#endif /* COTS_HAL_KPD_DRIVER_KPD_INTERFACE_H_ */
