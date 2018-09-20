// Keyboard.c
// Runs on LM4F120 or TM4C123, 
// Unisinos TGA Eletrônica 4 
// There are two keys in the keyboard
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

// Port E bits 1-0 have 2 keyboard keys

#include "Keyboard.h"
#include "tm4c123gh6pm.h"

// **************Keyboard_Init*********************
// Initialize keyboard key inputs
// Input: none
// Output: none
void Keyboard_Init(void) {
	unsigned long volatile delay;
	SYSCTL_RCGC2_R |= 0x00000010;     // activate port E
	delay = SYSCTL_RCGC2_R;           // execute some delay
  GPIO_PORTE_AMSEL_R &= ~0x03;      // no analog in PE0-1
  GPIO_PORTE_PCTL_R &= ~0x0000FFFF; // regular function on PE0-1
  GPIO_PORTE_DIR_R &= ~0x03;        // make PE0-1 IN
  //GPIO_PORTE_DR8R_R |= 0x04;      // can drive up to 8mA out
  GPIO_PORTE_AFSEL_R &= ~0x03;      // disable alt funct on PE0-1
  GPIO_PORTE_DEN_R |= 0x03;         // enable digital I/O on PE0-1
}

// **************Keyboard_In*********************
// Input from keyboard key inputs
// Input: none 
// Output: 0 to 3 depending on keys
// 0x01 is the start key pressed, 0x02 is the stop key pressed
unsigned long Keyboard_In(void) {
	static unsigned short debounceCounter = 0;
	static unsigned long _lastStatus = 0;
	unsigned long _currentStatus = 0;
	
	// Read the current status for keys START and STOP
	_currentStatus = (GPIO_PORTE_DATA_R & 0X03);
	
	// if there is some key pressed and the value still the same increment the debounce counter.
	// This statement make it only possible to acionate alternating the keys.
	// That is, once started you can only stop, vice-versa.
	if( (_currentStatus!=0) && (_currentStatus != _lastStatus) ) {
		debounceCounter++;
	} else {
		debounceCounter = 0;
	}
	
	// Check for the debounce value, if reached return the key pressed
	if(debounceCounter > 5) {
		debounceCounter = 0;
		_lastStatus = _currentStatus;
		return _currentStatus;
	} else {
		return 0x00;
	}
	
}
