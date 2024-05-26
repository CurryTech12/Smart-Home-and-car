// Stepper.c
// Runs on TM4C123
// Starter file for CECE346 Project 3.
// by Min He, 4/3/2024

// Left stepper motor connections
// PD0 connected to driver for stepper motor coil A/In1
// PD1 connected to driver for stepper motor coil A'/In2
// PD2 connected to driver for stepper motor coil B/In3
// PD3 connected to driver for stepper motor coil B'/In4

// right stepper motor connections
// PE0 connected to driver for stepper motor coil A/In1
// PE1 connected to driver for stepper motor coil A'/In2
// PE2 connected to driver for stepper motor coil B/In3
// PE3 connected to driver for stepper motor coil B'/In4

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "systick.h"

struct State{
  uint8_t Left;     // Output
  uint8_t Right;     // Output
  uint8_t Next[2]; // CW/CCW Next[0]->CW, Next[1]->CCW
};
typedef const struct State StateType;

// TODO: finish the following constant definitions
#define FORWARD 0      // Next index
#define BACKWARD 1       // Next index
#define HUNDRED_STEPS 18       // each step moves 0.18 degree, 100 steps moves 18 degree

enum states {S0, S1, S2, S3};

// TODO: finish the FSM definition
StateType fsm[4]={
	// index 0: state 0,state goes from 0 to 3,
	// to move forward: PD3-0:left CCW; PE3-0,right CW
	// CW state transition is: 3->6->12->9 then repeat
	// CCW state transition is: 3->9->12->6 then repeat
	{3,3,{1,3}}, // state 0, PD3-0:0011 CW next state->1, CCW next state->3
  {6,6,{2,0}}, // state 1, PD3-0:0110
  {12,12,{3,1}}, // state 2, PD3-0:1100
  {9,9,{0,2}}  // state 3, PD3-0:1001
};

uint8_t s = 0, l_s = 0, r_s = 0; // current state

// TODO: define bit addresses for stepper motor pins
#define LEFT_STEPPER   (*((volatile uint32_t *)0x4000703C))  // PORT D, pin: 0,1,2,3
#define RIGHT_STEPPER  (*((volatile uint32_t *)0x4002403C))  // PORT E, pin: 0,1,2,3

// stop the stepper motor car and reset FSM states
void Stop_Car(void) {
	s = 0;
	l_s = 0;
	r_s = 0;
}
void Stop_Car1(void) {
	for(uint32_t i = 0; i < 250000; i++){
	}
}

// Move forward, speed is the systick reload value to control time to wait after each step
// each step moves 0.18 degree: one complete circle is 360 degrees
void Step_Forward(uint32_t speed, uint16_t degree){
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	for (i=0;i<n;i++) {
		LEFT_STEPPER = fsm[l_s].Left; // step motor
		RIGHT_STEPPER = fsm[r_s].Right;
		SysTick_Wait(speed);
		l_s = fsm[l_s].Next[BACKWARD]; // clock wise circular
		r_s = fsm[r_s].Next[FORWARD]; 
}
}
// Move backward, speed is the systick reload value to control time to wait after each step
// each step moves 0.18 degree: one complete circle is 360 degrees
void Step_Backward(uint32_t speed, uint16_t degree){
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	
	for (i=0;i<n;i++) {
		LEFT_STEPPER = fsm[l_s].Left; // step motor
		RIGHT_STEPPER = fsm[r_s].Right;
		SysTick_Wait(speed);
		l_s = fsm[l_s].Next[FORWARD]; // clock wise circular
		r_s = fsm[r_s].Next[BACKWARD]; 
	}
	
}

// car turn right: left wheel move forward(CCW), right wheel move backward(CCW)
void Step_Right(uint32_t speed, uint16_t degree){	
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	
	for (i=0;i<1250;i++) {
		LEFT_STEPPER = fsm[l_s].Left; // step motor
		RIGHT_STEPPER = fsm[r_s].Right;
		SysTick_Wait(speed);
		l_s = fsm[l_s].Next[BACKWARD]; // clock wise circular
		r_s = fsm[r_s].Next[BACKWARD]; 
	}
}

// car turn left: left wheel move backward(CW), right wheel move forward(CW)
void Step_Left(uint32_t speed, uint16_t degree){	
	uint16_t i,n=(100*degree)/HUNDRED_STEPS;
	
	for (i=0;i<1250;i++) {
		LEFT_STEPPER = fsm[l_s].Left; // step motor
		RIGHT_STEPPER = fsm[r_s].Right;
		SysTick_Wait(speed);
		l_s = fsm[l_s].Next[FORWARD]; // clock wise circular
		r_s = fsm[r_s].Next[FORWARD]; 
	}
}

// Initialize left Stepper motor interface
void Stepper_Init(void){
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // 1) activate port D
  s = 0; 
                                    
  GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO configure PD3-0 as GPIO
  GPIO_PORTD_DIR_R |= 0x0F;   // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= ~0x0F;// 6) disable alt funct on PD3-0
  GPIO_PORTD_DR8R_R |= 0x0F;  // enable 8 mA drive
  GPIO_PORTD_DEN_R |= 0x0F;   // 7) enable digital I/O on PD3-0 
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4; // 1) activate port D
  s = 0; 
                                    
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PE3-0
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // 4) GPIO configure PE3-0 as GPIO
  GPIO_PORTE_DIR_R |= 0x0F;   // 5) make PE3-0 out
  GPIO_PORTE_AFSEL_R &= ~0x0F;// 6) disable alt funct on PE3-0
  GPIO_PORTE_DR8R_R |= 0x0F;  // enable 8 mA drive
  GPIO_PORTE_DEN_R |= 0x0F;   // 7) enable digital I/O on PE3-0 
}

