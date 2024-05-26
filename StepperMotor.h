// Stepper.h
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

// Initialize Stepper interface
void Stepper_Init(void);

void Stop_Car(void);
void Stop_Car1(void);

// Move forward, speed is the systick reload value to control time to wait after each step.
// each step moves approximately 0.18 degree. One complete circle 360 degrees need 2048 steps.
void Step_Forward(uint32_t speed, uint16_t degree);

// Move backward, speed is the systick reload value to control time to wait after each step.
// each step moves approximately 0.18 degree. One complete circle 360 degrees need 2048 steps.
void Step_Backward(uint32_t speed, uint16_t degree);

void Step_Right(uint32_t speed, uint16_t degree);

void Step_Left(uint32_t speed, uint16_t degree);
