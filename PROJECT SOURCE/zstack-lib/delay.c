/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "ZComDef.h"
#include "delay.h"

/*********************************************************************
 * @fn      delay_us
 * @brief   Microsecond delay (approximate)
 * @param   microSecs - number of microseconds to wait
 *********************************************************************/

void delay_us(uint16 microSecs)
{
    while (microSecs--) {
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
        asm("NOP");
    }
}

/*********************************************************************
 * @fn      delay_ms
 * @brief   Millisecond delay
 * @param   milliSecs - number of milliseconds to wait
 *********************************************************************/

void delay_ms(uint16 milliSecs)
{
    while (milliSecs--) {
        delay_us(1000);
    }
}