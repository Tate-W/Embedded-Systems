/*
    Name: Tate Wieber
    Course: Embedded Systems
    Date: 2/24/23
    Desciption: Header File for GPIO Pin & Ports
*/


#ifndef EMBSYSS20_H
#define EMBSYSS20_H

#ifndef STM32L4
  #define STM32L4
#endif

/**********************INCLUDES*****************/
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


// All LED 2 Defines
#define     LD2_PORT        GPIOA
#define     LD2_PIN         GPIO5
#define     LD2_RCC         RCC_GPIOA

// All Button 1 Defines
#define     B1_PORT         GPIOC
#define     B1_PIN          GPIO13
#define     B1_RCC          RCC_GPIOC


// Functions

//LED2 interaction functions; Set, Clear, and Toggle
#define TURN_ON_LD2 gpio_set(LD2_PORT, LD2_PIN)
#define TURN_OFF_LD2 gpio_clear(LD2_PORT, LD2_PIN)
#define TOGGLE_LD2 gpio_toggle(LD2_PORT, LD2_PIN)

//LED2 status functions; Check if on and check if off
#define IS_LD2_SET gpio_get(LD2_PORT, LD2_PIN)
#define IS_LD2_RESET !gpio_get(LD2_PORT, LD2_PIN)

//Button 1 status functions; Check if pressed and check if off
#define IS_B1_PRESSED !gpio_get(B1_PORT, B1_PIN)
#define IS_B1_RELEASED gpio_get(B1_PORT, B1_PIN)

static void clock_enable(void){

    rcc_osc_on(RCC_HSI16);
  //main clock initializtion to 40MHz
	/* 16MHz / 4 = > 4 * 20 = 80MHz / 2 = VCO => 40MHz main pll  */
	rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 4, 20, 0, 0, RCC_PLLCFGR_PLLR_DIV2);
	rcc_osc_on(RCC_PLL);
}

#endif
