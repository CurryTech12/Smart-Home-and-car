#include <stdint.h> // C99 data types
#include "tm4c123gh6pm.h"
#include "systick_actualH.h"

struct State{
  uint8_t out;     // Output   // Output
  uint8_t Next[2]; // CW/CCW Next[0]->CW, Next[1]->CCW
};
typedef const struct State StateType;
enum states {S0, S1, S2, S3};

// TODO: finish the FSM definition
StateType fsm[4]={
	// index 0: state 0,state goes from 0 to 3,
	// to move forward: PD3-0:left CCW; PE3-0,right CW
	// CW state transition is: 3->6->12->9 then repeat
	// CCW state transition is: 3->9->12->6 then repeat
	{3,{1,3}}, // state 0, PD3-0:0011 CW next state->1, CCW next state->3
  {6,{2,0}}, // state 1, PD3-0:0110
  {12,{3,1}}, // state 2, PD3-0:1100
  {9,{0,2}}  // state 3, PD3-0:1001
	
};
#define  Garage 		(*((volatile uint32_t *)0x4000703C)) 

#define FORWARD 0        // Next index
#define BACKWARD 1   // Next index
#define HUNDRED_STEPS 18  // each step moves 0.18 degree, 100 steps moves 18 degree


#define PF1_PF3_MASK 0x0E
#define PD6_MASK 0x40

#define PORT_D_PRI 0xE0000000
#define PORTD_INT_PRI 3U
#define NVIC_EN0_PORTD 0x00000008
#define GarageControl 0x0F
uint16_t S = 0;

void Stop_Car(void) {
	S = 0;
}
void Stepper_CCW(uint32_t speed,uint32_t degree){
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	for (i=0;i<n;i++) {
		Garage = fsm[S].out; // step motor
		SysTick_Wait(speed);
		S = fsm[S].Next[BACKWARD]; 
	}
}


void Stepper_CW(uint32_t speed,uint32_t degree){
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	for (i=0;i<n;i++) {
		Garage = fsm[S].out; // step motor
		SysTick_Wait(speed);
		S = fsm[S].Next[FORWARD]; 
	}
}
// Initialize Port F LEDs: PF1 & PF3
void LEDInit(void) 
{
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // activate port F
	while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R5)!=SYSCTL_RCGCGPIO_R5){};
//	GPIO_PORTF_LOCK_R = PORT_F_LOCK ; //unlocks PF0 so that on the board it works
//	GPIO_PORTF_CR_R |= PORT_COMMIT;

	GPIO_PORTF_DIR_R |= PF1_PF3_MASK; // PF1 and PF3
  GPIO_PORTF_AFSEL_R &= ~PF1_PF3_MASK;// disable alt funct on PF1 and PF3
  GPIO_PORTF_DEN_R |= PF1_PF3_MASK; // enable digital I/O on PF1 and PF3                
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // configure PF1 and PF3
  GPIO_PORTF_AMSEL_R &= ~PF1_PF3_MASK;   // disable analog functionality on PF1 and PF3
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
void GarageDoorInit(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // activate port D
	while ((SYSCTL_RCGCGPIO_R&SYSCTL_RCGCGPIO_R3)!=SYSCTL_RCGCGPIO_R3){};
	GPIO_PORTD_DIR_R |= GarageControl; //  PB0 - PB3
  GPIO_PORTD_AFSEL_R &= ~GarageControl;// disable alt funct on  PD0 - PD3
  GPIO_PORTD_DEN_R |= GarageControl; // enable digital I/O on  PD0 - PD3
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // configure PD0 - PD3
  GPIO_PORTD_AMSEL_R &= ~GarageControl;   // disable analog functionality  PD0 - PD3
}


	