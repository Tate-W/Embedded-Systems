/*
1. The period is 1 second. This was found by 1. Using a stopwatch and manually finding it and 2. Using delay_loop and
   using the clock frequency to calculate the period.

2. The frequency of the main clock is set at 80MHz by default as it is the max clock speed. This is in the datasheet.

3.

4. Since the delay_loop is already making the LED flash at 1 second, all that would need to be done is to divide the
   input by 7.
*/

#ifndef STM32L4
   #define STM32L4
#endif

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "user_app.h"

static void delay_loop(int32_t loops)
{
    while(loops > 0)
    {
       asm("nop");
       loops--;
    }
}

int main(void)
{
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(LED2_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED2_Pin);
    while(1)
    {
       gpio_toggle(LED2_GPIO_Port, LED2_Pin);
       delay_loop(400000);
    }
}
