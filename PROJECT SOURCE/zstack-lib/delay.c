#include "ZComDef.h"
#include "delay.h"

void delay_us(uint16 microSecs)
{
  while(microSecs--)
  {
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

void delay_ms(uint16 milliSecs)
{
  while(milliSecs--)
  {
    delay_us(1000);
  }
}
