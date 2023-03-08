// HINT for TASK #2 -- FALL 2022


//************** I N C L U D E S *****************

       //PUT YOUR INCLUDES HERE
      #include <stdio.h>
      #include <string.h>
      #include <unistd.h>
      #include "circbuffer.h"
      #include "embsysS20.h"


//************** G L O B A L S *****************

      // PUT YOUR GLOBAL VARIABLES HERE
      const char key[21]= {'B', 'O' , 'L', 'D', 'F', 'E', 'A', 'R', 'L', 'E', 'S', 'S', 'C', 'O', 'N', 'F', 'I', 'D', 'E', 'N', 'T'};
      const int max = 1024;
//********* S U P P O R T     F U N C T I O N S *****************

      // any functions created to help out the bigger fcns below
//Available on the libopencm3 github example page
//Makes the USART2 port the STDOUT stream
int _write(int file, char* ptr, int len){
    int i;

    if (file == STDOUT_FILENO || file == STDERR_FILENO){
	    for(i = 0; i < len; i++){
		    if(ptr[i] == '\n')
			    usart_send_blocking(USART2, '\r');

		    usart_send_blocking(USART2, ptr[i]);
	    }

	    return i;
    }

    errno = EIO;
    return -1;
}
//Converts input to caps
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

//********* I N I T I A L I Z A T I O N *****************
static void usart_setup(void)
{
   /* DO NOT USE INTERRUPTS IN THIS DESIGN */
   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_USART2);

   /* Setup GPIO pins for USART */
   //Transmit
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
   //Recieve
   gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO3);   
   gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO3);	

   /* Setup USART pins as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);
   gpio_set_af(GPIOA, GPIO_AF7, GPIO3);
    

	/* Setup UART parameters. */
	usart_set_baudrate(USART2, 57600);
	usart_set_databits(USART2, 9);  // check the effect of parity on this fcn.
	usart_set_stopbits(USART2,USART_STOPBITS_1);
	usart_set_mode(USART2, USART_MODE_TX_RX);
	usart_set_parity(USART2, USART_PARITY_ODD);
	usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);



   usart_enable(USART2);

}

static void timer_setup(void)
{
   /* DO NOT USE INTERRUPTS IN THIS DESIGN */
   rcc_periph_clock_enable(RCC_TIM2);
   rcc_periph_reset_pulse(RST_TIM2);

   /* Setup timer parameters. */
   // timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

   timer_set_prescaler(TIM2, 200);
   
   timer_set_period(TIM2, 10000);
   timer_enable_counter(TIM2);

}


int main(void)  {
   // initialize the hardware
   clock_enable();
   usart_setup(); 
   timer_setup();

   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_GPIOC);
   gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO5);
   gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13);

   //Initialize the buffer 
   static struct circbuffer myCircBuffer;
   static char buffer[1024];
   circbuffer_init(&myCircBuffer, buffer, max);

   char charSending = '\0';

   while(true) {
   //Check RX flag to recieve
      if (usart_get_flag(USART2, USART_ISR_RXNE)) 
      {  
         circbuffer_queue(&myCircBuffer, usart_recv_blocking(USART2));
      }
   //Cipher code with sending once done
      if(!bufferIsEmpty(&myCircBuffer)){
         circbuffer_dequeue(&myCircBuffer, &charSending);
         convertCaps(&charSending);

         if(IS_B1_PRESSED)
            charSending = decrypt(charSending);

         else if(IS_B1_RELEASED)
            charSending = encrypt(charSending);


         if(usart_get_flag(USART2, USART_ISR_TXE))
            usart_send_blocking(USART2, charSending);
      }

   //Timer checks to toggle led
      if(timer_get_flag(TIM2, TIM_SR_CC1IF)){
         timer_clear_flag(TIM2, TIM_SR_CC1IF);
      
         if(IS_B1_PRESSED){
            timer_set_period(TIM2,2500); //0.125 seconds
         }
         if(IS_B1_RELEASED)
         {
            timer_set_period(TIM2, 10000); //0.5 seconds
         }

         TOGGLE_LD2;
      }

   }
   return 0;    // NEVER EXECUTES
} 
