// Timing C/ASM Mix Example
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   PF1 drives an NPN transistor that powers the red LED

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// Bitband alias
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))

// PortF masks
#define RED_LED_MASK 2

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, sysdivider of 5, creating system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    // Configure LED pin
    GPIO_PORTF_DIR_R |= RED_LED_MASK;  // make bit an output
    GPIO_PORTF_DR2R_R |= RED_LED_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTF_DEN_R |= RED_LED_MASK;  // enable LED
}

// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock
void wait10Seconds()
{
    //2 above clocks


        __asm(".const");
        __asm("TIME .field 44444444");
        __asm("prep:       LDR R1, TIME"); //2 clocks for this thiccy
        __asm("loop:       SUB R1, #1"); //N times
        __asm("            CBZ R1, DONE"); //(n-1) + 3
        __asm("             NOP");
        __asm("             NOP");
        __asm("             NOP");
        __asm("             NOP");
        __asm("             NOP");
        __asm("            B loop"); //(n-1)*2
        __asm("DONE:                    ");

        //2 below clocks
}

//nathans
//void wait10Seconds()
//{
//    __asm(".const");
//    __asm("LENGTH .field 50000000");
//    __asm("WMS_PREP:    LDR  R1, LENGTH");
//    __asm("WMS_LOOP:    SUB  R1, #1");
//    __asm("             CBZ  R1, WMS_DONE");
//    __asm("             NOP");
//    __asm("             NOP");
//    __asm("             NOP");
//    __asm("             NOP");
//    __asm("             B    WMS_LOOP");
//    __asm("WMS_DONE:");
//}
//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    // Initialize hardware
    initHw();

    // Toggle red LED every second
    while(true)
    {
      RED_LED ^= 1;
      wait10Seconds();
    }
}