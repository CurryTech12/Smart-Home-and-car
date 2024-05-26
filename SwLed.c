// SwLed.c
// Runs on TM4C123
// Starter file for CECE346 Project 3.
// Provide functions that initialize the SysTick module and generate 
// specified number of machine cycles delay using busy waiting approach
// Min He, 4/14/2023

#include <stdint.h>
#include "tm4c123gh6pm.h"
#define mask_Sw1_Sw2 0x11 // PF4 and PF0 mask 
#define prority 4U
#define portF_Pri 0x00E00000
#define portF_NVIC 0x40000000

#define PD6_MASK 0x40
#define PORT_D_PRI 0xE0000000
#define PORTD_INT_PRI 0U
#define NVIC_EN0_PORTD 0x00000008


//uint8_t flag = 0;
// Initialize SysTick with core clock.
void SwLed_Init(void){
	  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5;    // allow time for clock to start

			
    GPIO_PORTF_LOCK_R = 0x4C4F434B; // unlock PortF PF0
    GPIO_PORTF_CR_R |= 0x1F; // allow changes to PF0
    GPIO_PORTF_DIR_R &= ~mask_Sw1_Sw2; // make PF0 an input
    GPIO_PORTF_AFSEL_R &= ~mask_Sw1_Sw2; // disable alt funct on PF0
    GPIO_PORTF_DEN_R |= mask_Sw1_Sw2; // enable digital I/O on PF0
    GPIO_PORTF_PCTL_R &= ~0x000F000F; // configure PF0 as GPIO
    GPIO_PORTF_AMSEL_R &= ~mask_Sw1_Sw2; // disable analog functionality on PF
		GPIO_PORTF_PUR_R |= mask_Sw1_Sw2;
    GPIO_PORTF_IS_R &= ~mask_Sw1_Sw2; // PF0 is edge-sensitive
    GPIO_PORTF_IBE_R &= ~mask_Sw1_Sw2; // PF0 is not both edges
    GPIO_PORTF_IEV_R &= ~mask_Sw1_Sw2; // PF0 falling edge event
    GPIO_PORTF_ICR_R |= mask_Sw1_Sw2; // clear flag4367052
    GPIO_PORTF_IM_R |= mask_Sw1_Sw2; // arm interrupt on PF0
    NVIC_PRI7_R = (NVIC_PRI7_R&~portF_Pri)|prority << 21;
    NVIC_EN0_R |= portF_NVIC; // enable interrupt 30 in NVIC
}
void Sensor_Init(void) 
{
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // activate port D
	while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R3)!=SYSCTL_RCGCGPIO_R3){};
	
	GPIO_PORTD_DIR_R &= ~PD6_MASK; // make PD6 input
  GPIO_PORTD_AFSEL_R &= ~PD6_MASK;// disable alt funct on PD6
  GPIO_PORTD_DEN_R |= PD6_MASK; // enable digital I/O on PD6                
  GPIO_PORTD_PCTL_R &= ~0x0F000000; // configure PD6
  GPIO_PORTD_AMSEL_R &= ~PD6_MASK;   // disable analog functionality on PD6
	GPIO_PORTD_IS_R &= ~PD6_MASK;     // (d) PD6 is not level triggered
  GPIO_PORTD_IBE_R |= PD6_MASK;    //     PD6  set both edges
  //GPIO_PORTD_IEV_R |= PD6_MASK;    //      so set IEV PD6 high level trigger
  GPIO_PORTD_ICR_R |= PD6_MASK;     // (e) clear flag PD6
  GPIO_PORTD_IM_R |= PD6_MASK;      // (f) arm interrupt on PD6
	NVIC_PRI0_R = (NVIC_PRI0_R&~PORT_D_PRI)|PORTD_INT_PRI<<21; // (g) PORTD Interrupt priority bits: 31-29, priority set to 4
  NVIC_EN0_R |= NVIC_EN0_PORTD;     // (h) PORTD interrupt number is 30, enable bit 30 in NVIC. 
}
