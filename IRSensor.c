// IRSensor.c: starter file for CECS346 Lab 7 - Obstacle Avoidance Sensor and Battery Power Supply.
// group Brandon Jamjampour, Anastacia Estrella and Daniel Banuleos
//In this lab we were tasked with intefacing with an ir sensor whose output would be connceted to port 
//PD6 we would then use an interrupt that is both edges triggered to capture when the sensors sees a change from 1 -> 0 or 0 -> 1
//if there is an object within 15cm the on board leds on portF would show red if it is greater than 15cm it would show green

#include <stdint.h> // C99 data types
#include "tm4c123gh6pm.h"
#include "systick_actualH.h"
#include "stepperMotor.h"
#include "SwButton.h"

// TODO: LED bit address definition for PF3 and PF1
#define LED       (*((volatile uint32_t*)0x40025038)) 
	
// Sensor bit address defenition for PD6
#define SENSOR 		(*((volatile uint32_t *)0x40007100)) 
	
#define  SW1 		(*((volatile uint32_t *)0x40025040)) 
#define  SW2 		(*((volatile uint32_t *)0x40025004)) 
	
#define mask_Sw1_Sw2 0x11

#define PD6_MASK 0x40
#define RED  0x02
#define BLUE 0x04
#define GREEN 0x08



#define T1ms 16000U
#define highest_speed      2*T1ms
#define STANDARD_SPEED		10*T1ms  // stepper motor speed: output every 10ms, frequency for the stepper motor is 100Hz.
#define halfsec 10*T1ms
#define One_sec 1000*T1ms
// External Function Prototypes (external functions from startup.s)
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // Go to low power mode while waiting for the next interrupt

uint32_t i = 0;
uint16_t counter = 0;
uint16_t check = 0;
uint16_t garage = 0,flag1 = 0,flag = 0;
uint16_t down = 0, up = 0,hold = 0,stop = 0,stop1;
uint16_t carDetected = 0;
int main(void){
	DisableInterrupts();
	Sensor_Init();    // initialize GPIO Port F interrupt
  LEDInit();
	EnableInterrupts();
	SysTick_Init(halfsec);
	GarageDoorInit();
	ButtonPress_Init();



  while(1){
		//when restart press press SW1 to turn on system and adjust stepper motor to close the garage
		if(flag1 == 1){
			LED = GREEN;
			Stepper_CW(highest_speed,180);
			flag1 = 0;
			down = 1;
		}
		//open garage when down
		if(garage == 1 && down == 1 && hold == 0){
			LED = 0x00;
			Stepper_CCW(highest_speed,2000);
			SysTick_Wait(halfsec);
			up = 1;
			down = 0;
			LED = BLUE;
		}
		//close garage when opened
		if(garage == 0 && up == 1 && hold == 0){
			while(i != 3000000){
				i += 1;
			}
			LED = 0x00;
			Stepper_CW(highest_speed,2000);
			SysTick_Wait(halfsec);
			up = 0;
			down = 1;
			LED = GREEN;
		}
		if(flag == 1 && stop == 0 && garage == 0){
			LED = 0x00;
			Stepper_CCW(highest_speed,2000);
			up = 1;
			down = 0;
			LED = BLUE;
			hold = 1;
			stop = 1;
			stop1 = 1;
		}
		if(flag == 0 && stop1 == 1 && garage == 0){
			LED = 0x00;
			Stepper_CW(highest_speed,2000);
			SysTick_Wait(halfsec);
			up = 0;
			down = 1;
			LED = GREEN;
			stop1 = 0;
			stop = 0;
		}
		if(garage == 1 && hold == 1){
			Stop_Car();
			carDetected = 1;
		}
		if(garage == 0 && hold == 1 && up == 1){
				Stop_Car();
				if(carDetected == 1){
					LED = 0x00;
					Stepper_CW(highest_speed,2000);
					SysTick_Wait(halfsec);
					up = 0;
					down = 1;
					LED = GREEN;
					hold = 0;
					carDetected = 0;
					stop = 0;
					stop1 = 0;
					flag = 0;
				}
		}
	}
}


// change LED color whenever an abstacle cross detectable distance
void GPIOPortD_Handler(void) 
{
	for (uint32_t i=0;i<160000;i++) {} // debouncer
	if(GPIO_PORTD_RIS_R & PD6_MASK) {		//if the source for reset is set PD3
		GPIO_PORTD_ICR_R |= PD6_MASK;//acknowledge for port PD3
		if(SENSOR == 0){
			garage = 1;
//			LED = RED;
		}else{
			garage = 0;
//			LED = GREEN;
		}
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
		if(SW2 == 0 && down == 1){
			flag = 1;
		}else if (SW2 == 0 && down == 0){
			flag = 0;
		}
	}
}
void SysTick_Handler(void){
	if((garage == 1 && down == 1 && check == 50) || (garage == 0 && up == 1 && check == 50) || flag % 2 == 1 && check == 50){
		LED ^= RED;
		check = 0;
	}else{
		if(counter < 50){
			counter = counter + 1;
		}else{
			check = 50;
			counter = 0;
		}
	}
}

