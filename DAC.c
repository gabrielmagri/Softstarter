// DAC.h
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// 4-bit Digital analog converter
// Gabriel Magri, Jaqueline Isabel Prass, Marcos
// September 18, 2018

// Port B bits 3-0 have the 4-bit DAC

#include "DAC.h"
#include "tm4c123gh6pm.h"

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void){
  unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000002;     // activate port B
	delay = SYSCTL_RCGC2_R;           // execute some delay
  GPIO_PORTB_AMSEL_R &= ~0x3F;      // no analog in PB0-5
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // regular function on PB0-5
  GPIO_PORTB_DIR_R |= 0x3F;         // make PB0-5 OUT
  GPIO_PORTB_DR8R_R |= 0x3F;        // can drive up to 8mA out
  GPIO_PORTB_AFSEL_R &= ~0x3F;      // disable alt funct on PB0-5
  GPIO_PORTB_DEN_R |= 0x3F;         // enable digital I/O on PB0-5
}


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data){
  GPIO_PORTB_DATA_R = ((GPIO_PORTB_DATA_R & ~0x3F)|(data & 0x3F));
}
