/* libraries */
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include <util/delay.h>

/* MCAL */
#include "../../MCAL/DIO_Driver/DIO_Interface.h"

/* Component */
#include "KPD_Config.h"
#include "KPD_Interface.h"

void HKPD_vInit(void)
{
	/* SET Rows pins as input high */
	MDIO_vSetPinDirection(KPD_PORT,KPD_ROW1, LOW);
	MDIO_vSetPinDirection(KPD_PORT,KPD_ROW2, LOW);
	MDIO_vSetPinDirection(KPD_PORT,KPD_ROW3, LOW);
	MDIO_vSetPinDirection(KPD_PORT,KPD_ROW4, LOW);

	MDIO_vSetPinValue(KPD_PORT, KPD_ROW1, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_ROW2, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_ROW3, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_ROW4, HIGH);

	/* SET Columns pins as output high */
	MDIO_vSetPinDirection(KPD_PORT,KPD_COL1, HIGH);
	MDIO_vSetPinDirection(KPD_PORT,KPD_COL2, HIGH);
	MDIO_vSetPinDirection(KPD_PORT,KPD_COL3, HIGH);
	MDIO_vSetPinDirection(KPD_PORT,KPD_COL4, HIGH);

	MDIO_vSetPinValue(KPD_PORT, KPD_COL1, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_COL2, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_COL3, HIGH);
	MDIO_vSetPinValue(KPD_PORT, KPD_COL4, HIGH);
}
u8 *HKPD_u8GetPressedKey(void)
{
    static u8 local_u8Value = NOT_PRESSED; // Static variable to retain value across function calls
    u8 local_u8Status;

    for (u8 local_u8Columns = KPD_COL1; local_u8Columns <= KPD_COL4; local_u8Columns++) {

        MDIO_vSetPinValue(KPD_PORT, local_u8Columns, LOW);
        _delay_ms(5); // Reduced delay for column activation

        for(u8 local_u8Rows = KPD_ROW1; local_u8Rows <= KPD_ROW4; local_u8Rows++) // Fixed variable increment in loop
        {
            local_u8Status = MDIO_u8GetPinValue(KPD_PORT, local_u8Rows);

            if (local_u8Status == 0)
            {
                local_u8Value = KPD_Buttions[local_u8Rows - KPD_ROW1][local_u8Columns - KPD_COL1]; // Adjusted array indexing

                // Wait for key release
                while (MDIO_u8GetPinValue(KPD_PORT, local_u8Rows) == LOW)
                {
                    _delay_ms(5); // Adjusted delay for debounce
                }
            }
        }
        MDIO_vSetPinValue(KPD_PORT, local_u8Columns, HIGH);
    }
    return &local_u8Value; // Return value of static variable
}
