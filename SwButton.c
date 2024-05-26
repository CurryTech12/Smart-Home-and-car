#include <stdint.h> // C99 data types
#include "tm4c123gh6pm.h"


#define mask_Sw1_Sw2 0x11
#define prority 4U
#define portF_Pri 0x00E00000
#define portF_NVIC 0x40000000

void ButtonPress_Init(void){
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
