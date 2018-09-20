// Softstarter.c
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

/*
 *
 *
 *
 */

#include "tm4c123gh6pm.h"
#include "DAC.h"
#include "PLL.h"
#include "Debug.h"
#include "Keyboard.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

unsigned int lastKeyboardIn = 0;
unsigned int KeyboardInNow = 0;
//int count = 0; 

int main(void){
	
	PLL_Init();
	Keyboard_Init();
	DAC_Init();
	Debug_Init();
 
  EnableInterrupts();
	
  for(;;) {
		switch(Keyboard_In()) {
			case KEY_START_PRESSED:
				Debug_TooglePin();
				break;
			case KEY_STOP_PRESSED:
				Debug_TooglePin();
				break;
			default:
				
				break;
		}
  }
}

