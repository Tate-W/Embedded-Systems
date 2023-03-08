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
#include "circbuffer.h"


// All LED 2 Defines
#define     LD2_PORT        GPIOA
#define     LD2_PIN         GPIO5
#define     LD2_RCC         RCC_GPIOA

// All Button 1 Defines
#define     B1_PORT         GPIOC
#define     B1_PIN          GPIO13
#define     B1_RCC          RCC_GPIOC

//Cipher Define
const char key[21]= {'B', 'O' , 'L', 'D', 'F', 'E', 'A', 'R', 'L', 'E', 'S', 'S', 'C', 'O', 'N', 'F', 'I', 'D', 'E', 'N', 'T'};

//Buffer init
struct circbuffer circBufferIn;
struct circbuffer circBufferOut;
char bufferI[256];
char bufferO[256];

uint16_t globalPeriod = 1000;

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

/******INITALIZATION FUNCTIONS*********/
static void clock_enable(void){

    rcc_osc_on(RCC_HSI16);
  //main clock initializtion to 40MHz
	/* 16MHz / 4 = > 4 * 20 = 80MHz / 2 = VCO => 40MHz main pll  */
	rcc_set_main_pll(RCC_PLLCFGR_PLLSRC_HSI16, 4, 40, 0, 0, RCC_PLLCFGR_PLLR_DIV2);
	rcc_osc_on(RCC_PLL);
   rcc_periph_clock_enable(RCC_TIM2);
   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_USART2);
}

static void usart_setup(void)
{
   nvic_enable_irq(NVIC_USART2_IRQ);

   /* Setup GPIO pins for USART */
   //Transmit
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
   //Recieve
   gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);   
   //gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO3);	

   /* Setup USART pins as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
   gpio_set_af(GPIOA, GPIO_AF7, GPIO3);
    

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 57600);
	usart_set_databits(USART2, 9);  // check the effect of parity on this fcn.
	usart_set_stopbits(USART2,USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_NONE); //Changed from ODD to NONE for testing
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

   usart_enable_rx_interrupt(USART2);

   usart_enable(USART2);

}

void usart_isr(void)
{
  if(((USART_CR1(USART2) & USART_CR1_RXNEIE) != 0) && (usart_get_flag(USART2, USART_ISR_RXNE)))
  {
      circbuffer_queue(&circBufferIn, usart_recv(USART2));
  }

  if(((USART_CR1(USART2) & USART_CR1_TXEIE) != 0) && (usart_get_flag(USART2, USART_ISR_TXE)))
  {
   char charSending = '\0';
   circbuffer_dequeue(&circBufferOut, &charSending);
   usart_send(USART2, charSending);

   if(bufferIsEmpty(&circBufferOut))
   {
      usart_disable_tx_interrupt(USART2);
   }
  }

}

static void timer_setup(void)
{
   rcc_periph_clock_enable(RCC_TIM2);
   rcc_periph_reset_pulse(RST_TIM2);

   /* Setup timer parameters. */
   // timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

   timer_set_prescaler(TIM2, 2000);
   
   timer_set_period(TIM2, 1000);
   timer_enable_counter(TIM2);

   timer_enable_irq(TIM2,TIM_DIER_CC1IE);
}

void timer_isr(void)
{
  if(timer_get_flag(TIM2, TIM_SR_CC1IF))
  {
    timer_set_period(TIM2, globalPeriod);
    timer_clear_flag(TIM2, TIM_SR_CC1IF);

    TOGGLE_LD2;
  }
}
/**************Cipher Functions to clean up main******************/
void convertCaps(char* toConvert){
   if(*toConvert >= 'a' && *toConvert <= 'z')
      *toConvert = (*toConvert - 'a') + 'A';
}
//Encrypts with Vigenere cipher using 'key'
char encrypt(char toEncrypt){
   static uint8_t pos = 0;
   
   toEncrypt = ((toEncrypt + key[pos]) % 26) + 'A';
   ++pos;

   pos = (pos == 21) ? 0 : pos;

   return toEncrypt;
}
//Decrypts with Vigenere cipher using 'key'
char decrypt(char toDecrypt){
   static uint8_t pos = 0;

   toDecrypt = ((toDecrypt - key[pos] + 26) % 26) + 'A';
   ++pos;

   pos = (pos == 21) ? 0 : pos;

   return toDecrypt;
}
//Basic delay loop
static void delay_loop(uint32_t loops)
{
    while(loops > 0)
    {
       asm("nop");
       loops--;
    }
}
#endif
