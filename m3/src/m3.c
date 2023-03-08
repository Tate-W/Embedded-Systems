// HINT for TASK #2 -- FALL 2022


//************** I N C L U D E S *****************

       //PUT YOUR INCLUDES HERE
      #include <stdio.h>
      #include <string.h>
      #include <unistd.h>
      #include <errno.h>
      #include "embsysS20.h"


//************** G L O B A L S *****************

      // PUT YOUR GLOBAL VARIABLES HERE
      const int max = 1024;
//********* S U P P O R T     F U N C T I O N S *****************

void charHandler(char handle);
void cipherHandler(char toDo);
void fixPeriod(char* commit);
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



int main(void)  {
   //Init buffers
   circbuffer_init(&circBufferIn, bufferI, max);
   circbuffer_init(&circBufferOut, bufferO, max);
  
   // initialize the hardware
   clock_enable();
   usart_setup(); 
   timer_setup();

   rcc_periph_clock_enable(RCC_GPIOA);
   rcc_periph_clock_enable(RCC_GPIOC);
   gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO5);
   gpio_mode_setup(GPIOC, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO13);


   while(true)
   {
      if(!bufferIsEmpty(&circBufferIn))
      {
            char charSending = '\0';
            circbuffer_dequeue(&circBufferIn, &charSending);
            charHandler(charSending);
      }
   }
   return 0;    // NEVER EXECUTES
} 

void charHandler(char handle)
{
      static int myCase = 0;
      static char period[4] = {'1', '0', '0', '0'};
      static char lastPeriod[4] = {'1', '0', '0', '0'};

      if(handle == '!' && myCase != 3)
            ++myCase;
      else if(myCase == 3 && handle == 'L')
            myCase = 4;
      else if(myCase == 3 && handle == 'S')
            myCase = 5;
      else if(myCase == 3 && handle == '!')
            myCase = 10;
      
      else if(myCase < 6)
            myCase = 0;
      else if(myCase >= 6 && (handle < 0x30 || handle > 0x39)){
            myCase = 0;
            printf("Invalid!");

            for(int i = 0; i < 4; i++)
                  period[i] = lastPeriod[i];
            return;
      }

      switch (myCase)
      {
      case 0:
            cipherHandler(handle);
            break;
      case 1:
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 2:
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 3:
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 4:
            circbuffer_queue(&circBufferOut, handle);

            for (int i = 0; i < 4; i++)
            {
                  circbuffer_queue(&circBufferOut, period[i]);
            }           
            usart_enable_tx_interrupt(USART2);
            myCase = 0;
            break;
      case 5:
            ++myCase;
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 6:
            ++myCase;
            period[0] = handle;
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 7:
            ++myCase;
            period[1] = handle;
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 8:
            ++myCase;
            period[2] = handle;
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      case 9:
            myCase = 0;
            period[3] = handle;
            circbuffer_queue(&circBufferOut, handle);
            fixPeriod(period);
            for (int i = 0; i < 4; i++)
            {
                  lastPeriod[i] = period[i];
            }
            usart_enable_tx_interrupt(USART2);
            break;
      case 10:
            myCase = 3;
            circbuffer_queue(&circBufferOut, handle);
            usart_enable_tx_interrupt(USART2);
            break;
      }
}

void cipherHandler(char toDo)
{
      convertCaps(&toDo);

      if(IS_B1_PRESSED)
            toDo = decrypt(toDo);
      else
            toDo = encrypt(toDo);

      circbuffer_queue(&circBufferIn, toDo);
      usart_enable_tx_interrupt(USART2);
}

void fixPeriod(char* commit)
{
      char convert[5] = {'0','0','0','0','\0'};

      for (int i = 0; i < 4; i++)
      {
            convert[i] = commit[i];
      }

      int period = atoi(convert);
      globalPeriod = period;
      
}