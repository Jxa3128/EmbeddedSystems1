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

// Bitband alias -- remeber depending on what port you have to change the port base
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 2*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 5*4)))
#define BLUE_LED     (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 4*4)))
//because this is in PA3 _thefore it is in 3rd bit
#define YELLOW_LED   (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 3*4)))
#define SW1          (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4)))
#define SW2          (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4)))

// PortF masks
#define RED_LED_MASK 4
#define GREEN_LED_MASK 32
#define BLUE_LED_MASK 16
#define YELLOW_LED_MASK 8
#define SW1_MASK 2
#define SW2_MASK 4
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, sysdivider of 5, creating system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN
            | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks port A & port E + other ones idk
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R4
            | SYSCTL_RCGCGPIO_R3;
    _delay_cycles(3);

    // Configure LED pin
    GPIO_PORTA_DIR_R |= RED_LED_MASK | GREEN_LED_MASK | BLUE_LED_MASK
            | YELLOW_LED_MASK;
    // make bit an output
    GPIO_PORTA_DR2R_R |= RED_LED_MASK | GREEN_LED_MASK | BLUE_LED_MASK
            | YELLOW_LED_MASK;

    // set drive strength to 2mA (not needed since default configuration -- for clarity)

    GPIO_PORTA_DEN_R |= RED_LED_MASK | GREEN_LED_MASK | BLUE_LED_MASK
            | YELLOW_LED_MASK;  // enable LED

    //enable the _swtiches
    GPIO_PORTE_DR2R_R |= SW1_MASK | SW2_MASK;
    GPIO_PORTE_DIR_R &= SW1_MASK | SW2_MASK;
    GPIO_PORTE_DEN_R |= SW1_MASK | SW2_MASK;

    // enable internal pull-up for push button
    GPIO_PORTE_PUR_R |= SW1_MASK;
    GPIO_PORTE_PDR_R |= SW2_MASK; //pull down

}
void waitSW2() {
    while(!SW2);
}
void waitSW1(){
    while(SW1);
}
// Approximate busy waiting (in units of microseconds), given a 40 MHz system clock
void waitMicrosecond(uint32_t us)
{
    __asm("WMS_LOOP0:   MOV  R1, #6");
    // 1
    __asm("WMS_LOOP1:   SUB  R1, #1");
    // 6
    __asm("             CBZ  R1, WMS_DONE1");
    // 5+1*3
    __asm("             NOP");
    // 5
    __asm("             NOP");
    // 5
    __asm("             B    WMS_LOOP1");
    // 5*2 (speculative, so P=1)
    __asm("WMS_DONE1:   SUB  R0, #1");
    // 1
    __asm("             CBZ  R0, WMS_DONE0");
    // 1
    __asm("             NOP");
    // 1
    __asm("             B    WMS_LOOP0");
    // 1*2 (speculative, so P=1)
    __asm("WMS_DONE0:");
    // ---
    // 40 clocks/us + error
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
    // Initialize hardware
    initHw();
    RED_LED = 1;
    YELLOW_LED = 1;
    BLUE_LED = 0;
    GREEN_LED = 1;


    waitSW2();
    RED_LED = 0;
    GREEN_LED = 0;
    waitMicrosecond(1000000);
    BLUE_LED = 1;
    waitSW1();
    waitMicrosecond(500000);
    while (true)
    {
        YELLOW_LED ^= 1;
//        BLUE_LED ^= 1;
//        GREEN_LED^= 1;
//        RED_LED ^= 1;
        waitMicrosecond(500000);
    }
}
