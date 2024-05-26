// SmartCar.c
// Runs on TM4C123
// Starter file for CECE346 Project 3.
// by Min He, 4/3/2024

// Before connecting a real stepper motor, remember to put the
// proper amount of delay between each CW() or CCW() step.

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
#include "StepperMotor.h"
#include "systick.h"
#include "SwLed.h"
#include "tm4c123gh6pm.h"

#define  SW1 		(*((volatile uint32_t *)0x40025040)) 
#define  SW2 		(*((volatile uint32_t *)0x40025004)) 
#define SENSOR 		(*((volatile uint32_t *)0x40007100)) 

#define PD6_MASK 0x40


#define mask_Sw1_Sw2 0x11
#define T1ms 							16000U    // Systick reload value to generate 1ms delay, assumes using 16 MHz Clock.
#define HIGHEST_SPEED 		2*T1ms  // fastest speed the stepper can move
#define FASTER_SPEED			5*T1ms
#define STANDARD_SPEED		10*T1ms  // stepper motor speed: output every 10ms, frequency for the stepper motor is 100Hz.
#define HALF_SEC      		500*T1ms //systick reload value for 0.5s
#define full_sec          10000*T1ms
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);
extern void WaitForInterrupt(void);

uint32_t i = 0;
uint16_t flag = 0;
uint16_t flag1 = 0; 
uint16_t sensorFlag;
uint16_t phase1 = 0;//change this later to zero
uint16_t movefoward = 0;
uint16_t secondButton = 0;

int main(void){
  Stepper_Init();
  SysTick_Init();
	SwLed_Init();
	DisableInterrupts();
	EnableInterrupts();	
  Sensor_Init();
	
  while(1){
//		WaitForInterrupt();

		if(flag1 == 1){
			Step_Forward(HIGHEST_SPEED, 720);
			Stop_Car();
			SysTick_Wait(HALF_SEC);
			
			Step_Left(HIGHEST_SPEED, 90);
			Stop_Car();
			SysTick_Wait(HALF_SEC);
			
			flag1 = 0;
			phase1 = 1;
		}
		if(phase1 == 1 && sensorFlag == 0){
			Step_Forward(HIGHEST_SPEED,90);
		}
		if(phase1 == 1 && sensorFlag == 1){
			Stop_Car();
			movefoward = 1;
			phase1 = 0;
		}
		if(movefoward == 1 && sensorFlag == 0){
			while(i != 15000000){
				i += 1;
			}
			Step_Forward(HIGHEST_SPEED, 720);
			movefoward = 0;
			secondButton = 1;
		}
//		// Step 2: Turn left 90 degrees: left wheel move backward, right wheel move forward
//			Step_Left(HIGHEST_SPEED, 90);
		
//		// Step 3: Move forward until an object is detected in 15cm range
//		// 				1) enable sensor with both edge interrupts
//		// 				2) move forward when no object is detected within 15cm
//		// 				3) stop the car when an object is detected in 15cm range
//		// 				4) wait until the object is removed from within 15cm
//		// 				5) Move forward the wheels 180 degrees then stop
//		
//		// Taking care of leaving home

//		// Step 1. wait until sw2 is pressed
		if(flag == 1 && secondButton == 1){
			Step_Backward(HIGHEST_SPEED,720);
			Stop_Car();
			SysTick_Wait(HALF_SEC);
			
			Step_Right(HIGHEST_SPEED,90);
			Stop_Car();
			SysTick_Wait(HALF_SEC);
			
		// Step 4. moving the wheels 270 degrees forward, then stop.
			Step_Forward(HIGHEST_SPEED, 720);
			Stop_Car();
			SysTick_Wait(HALF_SEC);
			flag = 0;
			secondButton = 0;
		}
//	
 }
}
void GPIOPortF_Handler(void) 
{
	for (uint32_t i=0;i<160000;i++) {} // debouncer
	if(GPIO_PORTF_RIS_R & mask_Sw1_Sw2) {		//if the source for reset is set PD3
		GPIO_PORTF_ICR_R |= mask_Sw1_Sw2;//acknowledge for port PD3
		if(SW1 == 0){
			flag1 = 1;
		} else {
			flag1 = 0;
		}
		if(SW2 == 0){
			flag = 1;
		} else {
			flag = 0;
		}
	}
}
void GPIOPortD_Handler(void) 
{
	for (uint32_t z=0;z<1600;z++) {} // debouncer
	if(GPIO_PORTD_RIS_R & PD6_MASK) {		//if the source for reset is set PD3
		GPIO_PORTD_ICR_R |= PD6_MASK;//acknowledge for port PD3
		if(SENSOR == 0){
			sensorFlag = 1;
		}else{
			sensorFlag = 0;
		}
	}
}
