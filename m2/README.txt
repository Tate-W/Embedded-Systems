Tate Wieber
2-26-23
ECE-4140

src contains all files needed to make the main m2a.c file.

m2a: circbuffer.h builds function prototypes to be used in circbuffer.c
     circbuffer.c is used to create new Circular Buffers and manage them
     embsys20.h is used to initialize the system clock, LED2, and Button1 while also providing
        constants relating to these items.
    m2a.c is the main c file which gets the LED2 to blink at a slow rate when Button1 is released and
        a fast pace when Button1 is pressed. When Button1 is pressed, the code is meant to encrypt text
        inputted via serial connection and USART using the Vigenere cipher. When Button1 is released, the
        program will decyper using the Vigenere cipher.

