/*
 * KDP_Config.h
 *
 *  Created on: Aug 25, 2023
 *      Author: Abdelrahman (Ta'laab) Mohammed
 */

#ifndef COTS_HAL_KPD_DRIVER_KPD_CONFIG_H_
#define COTS_HAL_KPD_DRIVER_KPD_CONFIG_H_

#define KPD_PORT PORT_C

#define ROWS 4
#define COLS 4

static const u8 KPD_Buttions[ROWS][COLS]={
		{'7', '8', '9', '/'},
		{'4', '5', '6', '*'},
		{'1', '2', '3', '-'},
		{'#', '0', '=', '+'}
};

#define KPD_ROW1	0
#define KPD_ROW2	1
#define KPD_ROW3	2
#define KPD_ROW4	3
#define KPD_COL1	4
#define KPD_COL2	5
#define KPD_COL3	6
#define KPD_COL4	7

#endif /* COTS_HAL_KPD_DRIVER_KPD_CONFIG_H_ */
