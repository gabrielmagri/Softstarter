// control.c
// Runs on  TM4C123, 
// Unisinos TGA Eletrônica 4 
// Use the SysTick timer to request interrupts at a particular period for controlling a 6-bit DAC.
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

/*  This routine controls the 6-bit DAC outputs, through wich we control the voltage level
 * applied to the pin 11 of a TDA. This way we can control the sine wave applied to the load,
 * speeding up or down in case that the load is an AC motor.
 *
 * By the Professor's specification:
 *  When accelerating we must start the motor acceleration with a trigger angle
 * of 120° and start decreasing it all the way down to 0.
 *  When slowing down we must start with a trigger angle of 0º and start increasing
 * it all the way up to 120° when we will jump directly to 180° in order to stop the motor
 *
 * The DAC resolution is about 5/64 ~= 0.078125
 * When passing 63 (maximum) to the 6-bit DAC the output voltage level will be 5/(2^6)*63 ~= 4.921875
 * If 4.921875 correspondes to 180°, 120° is equals to 3.28125. Which divided by the resolution gives us
 *  a value of ~= 42, which will be the start value when accelerating and the threshold before jump to
 *  180º (equals to 63 that represents 4.921875 on pin 11) when slowing down the motor.
 *
 * We want to full speed up or down with a time interval of 10 seconds. Once we have 43 values to update
 *  (42->0 when speeding up or 1->63 when speeding down) we want to update the DAC output 4.3 times/sec.
 * If the update frequence is 4.3Hz the time is 1/f = 0.232558139sec. If the microcontroller is running at
 *  80Mhz we have one increment NVIC_ST_CURRENT_R register each 12ns. So, if we want to execute one interrupt
 *  for each single DAC output update we would want a NVIC_ST_RELOAD_R of (0.232558139/12.5 - 1) that is equals to
 *  18604650.16 or 18604650. We will execute 10x faster for two reasons, first, the NVIC_ST_RELOAD_R register
 *  suports a assign value of at most 2^24-1, second, we want that the first value's update occur as soon as possible.
 *  For these reasons we will set the NVIC_ST_RELOAD_R to 1860465 and update the DAC output just once for each 
 *  ten (10) systick interrupts occured.
 * 
 */

#include "control.h"
#include "DAC.h"
#include "LEDs.h"
#include "tm4c123gh6pm.h"

//
const unsigned char output[44] = {63, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 
	25, 24, 23, 22, 21, 20, 19, 18, 17, 16 ,15 ,14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

unsigned int Index=0;     // Index varies from 0 to 43
unsigned int InterruptsCounter = 0;
unsigned char buttonClicked = NONE_CLICKED;
unsigned char starting = 0;
unsigned char stopping = 0;
unsigned char started = 0;

//
void Systick_Init(void) {
	NVIC_ST_CTRL_R = 0;                           // disable SysTick during setup
  NVIC_ST_RELOAD_R = 1860465;                   // reload value
  NVIC_ST_CURRENT_R = 0;                        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;                  // enable with core clock and interrupts
}
																		
// **************Control_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Control_Init(void){
	DAC_Init();
  Systick_Init();
}

// **************Start_Clicked*********************
// Sinalize that the start button was clicked
// Input: none
// Output: none
void Start_Clicked(void){
	buttonClicked = START_CLICKED;
}

// **************Stop_Clicked*********************
// Sinalize that the stop button was clicked
// Input: none
// Output: none
void Stop_Clicked(void){
	buttonClicked = STOP_CLICKED;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	
	switch(buttonClicked) {
		case START_CLICKED:
			stopping=0;
			starting=1;
		  InterruptsCounter = 10; // force to immediately update
			buttonClicked = NONE_CLICKED; // Clear the click
			break;
		case STOP_CLICKED:
			starting=0;
			stopping=1;
			InterruptsCounter = 10; // force to immediately update
			buttonClicked = NONE_CLICKED; // Clear the click
			break;
		case NONE_CLICKED:
			break;
		default:
			break;
	}
	
	if(InterruptsCounter>=10) {
		InterruptsCounter=0; //Reset the counter
		
		if(starting==1) {
			LEDs_Green();
			if(Index>=43) { //Reached the end of starting
				starting=0;
				Index = 43; //Lock the index into 43 to hold the angle as 0
				started = 1; //Indicate that the motor reached the nominal motor
			} else {
				Index++;
			}
		} else if(stopping==1) {
			LEDs_Red();
			if(Index==0) { //Reached the end of stopping
				stopping=0;
				Index = 0; //Lock the index into 0 to hold the angle as 180
				started = 0; //Indicate that the motor is no longer receiving voltage
			} else {
				Index--;
			}
		} else {
			if(started==1) {
				LEDs_Blue();
			} else {
				LEDs_None();
			}
		}
		
		DAC_Out(output[Index]); //output one value each 10 interrupts
	}
	
	InterruptsCounter++;
	
}
