// Sound.c
// Runs on  TM4C123, 
// Unisinos TGA Eletrônica 4 
// Use the SysTick timer to request interrupts at a particular period.
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

// This routine calls the 4-bit DAC

#include "control.h"
#include "DAC.h"
#include "tm4c123gh6pm.h"

const unsigned char SineWave[30] = {9, 11, 12, 13, 14, 15, 15, 15, 15, 14, 13, 12, 11, 9, 8, 6, 4, 3, 2, 1, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7};
unsigned int Index=0;     // Index varies from 0 to 29
unsigned int canOutput=0; // Inidicate if the systick ISR can output to the DAC or must be quiet
unsigned int systickRunning = 0;
																		
void Systick_Init(void) {
	NVIC_ST_CTRL_R = 0;                           // disable SysTick during setup
  NVIC_ST_RELOAD_R = 5095;                      // reload value
  NVIC_ST_CURRENT_R = 0;                        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;                  // enable with core clock and interrupts
}
																		
// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Sound_Init(void){
	DAC_Init();
  Systick_Init();
}

// **************Sound_Tone*********************
// Change Systick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period){
// this routine sets the RELOAD and starts SysTick
	canOutput = 1;
	NVIC_ST_CTRL_R = 0;          // disable SysTick during setup
	NVIC_ST_RELOAD_R = period;   // reload value
  NVIC_ST_CURRENT_R = 0;       // any write to current clears it
	NVIC_ST_CTRL_R = 0x00000007; // enable with core clock and interrupts
	
}


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
 // this routine stops the sound output
	canOutput = 0;
}


// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	systickRunning = ((systickRunning+1) & 0xFFFF);
	if(canOutput) {
		Index++;
		if(Index>29) Index = 0;
		DAC_Out(SineWave[Index]);    // output one value each interrupt
	} else {
		DAC_Out(0);
		Index = 0;
	}
}
