// Debug.h
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// Provide functions for debug and check that code is running well
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

#include "Debug.h"
#include "tm4c123gh6pm.h"

// **************Debug_Init*********************
// Initialize Debug features
// Input: none
// Output: none
void Debug_Init(void) {
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000020;     // activate port F
	delay = SYSCTL_RCGC2_R;           // execute some delay
  GPIO_PORTF_AMSEL_R &= ~0x04;      // no analog in PF2
  GPIO_PORTF_PCTL_R &= ~0x00000F00; // regular function on PF2
  GPIO_PORTF_DIR_R |= 0x04;         // make PF2 OUT
  //GPIO_PORTE_DR8R_R |= 0x04;      // can drive up to 8mA out
  GPIO_PORTF_AFSEL_R &= ~0x04;      // disable alt funct on PF2
  GPIO_PORTF_DEN_R |= 0x04;         // enable digital I/O on PF2
	
}

// **************Debug_TooglePin*********************
// Executes a toogle on the debug pin
// Input: none 
// Output: none
void Debug_TooglePin(void) {
	GPIO_PORTF_DATA_R ^= 0x04;
}

// **************Debug_ToogleBoardLed*********************
// Executes a toogle on the board led
// Input: none 
// Output: none
void Debug_ToogleBoardLed(void) {
	GPIO_PORTF_DATA_R ^= 0x04;
}
