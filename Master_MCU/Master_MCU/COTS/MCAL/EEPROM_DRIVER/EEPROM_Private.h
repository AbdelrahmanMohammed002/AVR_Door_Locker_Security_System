/*
 * EEPROM_Private.h
 *
 *  Created on: Apr 25, 2024
 *      Author: Abdelrahman (Ta'laab) Mohammed
 *		Github: https://github.com/AbdelrahmanMohammed002
 */

#ifndef MCAL_EEPROM_DRIVER_EEPROM_PRIVATE_H_
#define MCAL_EEPROM_DRIVER_EEPROM_PRIVATE_H_

#define EEARL	  *((volatile u8 *)0x3E)
#define EEARH	  *((volatile u8 *)0x3D)
#define EEDR  	  *((volatile u8 *)0x3D)
#define EECR	  *((volatile u8 *)0x3C)

#define EERIE	3
#define EEMWE	2
#define EEWE	1
#define EERE	0

void __vector_17(void) __attribute__((signal));


#endif /* MCAL_EEPROM_DRIVER_EEPROM_PRIVATE_H_ */
