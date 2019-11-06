#include "led.h"


void delay(volatile int32_t number)
{


    int multiplier = 4000;               //for the Mhz system clock
    int delay = number*multiplier;
    while (delay!=0)                    //perform desired delay with the following loop
    {
        __asm volatile ("nop");
        delay--;
    }
}


void blink(int LED_no, int delay_counter)
{
//	log_blink_led();

    if (LED_no==0)              //LED is Red
    {
   // 	log_blink_led();
        LED_RED_ON();
        delay(delay_counter);   //give a delay to the led
        LED_RED_OFF();
    }

    if (LED_no==1)               //LED is Green
    {
  //  	log_blink_led();
        LED_GREEN_ON();
        delay(delay_counter);    //give a delay to the led
        LED_GREEN_OFF();
    }

    if (LED_no==2)               //LED is Blue
    {
    //	log_blink_led();
        LED_BLUE_ON();
        delay(delay_counter);  //give a delay to the led
        LED_BLUE_OFF();
    }
}
