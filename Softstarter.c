// Softstarter.c
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

/* This software constitutes part of the PGA for "Eletrônica 4" subject.
 * It's role is to control the voltage level applied to the pin 11 of a
 * TDA, aiming to control the acceleration and deceleration of a motor
 * when starting or stopping.
 * 
 * More details on control.c file.
 */

#include "tm4c123gh6pm.h"
#include "PLL.h"
#include "control.h"
#include "Debug.h"
#include "Keyboard.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

unsigned int lastKeyboardIn = 0;
unsigned int KeyboardInNow = 0;

int main(void){
	
	PLL_Init();
	Debug_Init();
	Keyboard_Init();
	Control_Init();
 
  EnableInterrupts();
	
  for(;;) {
		switch(Keyboard_In()) {
			case KEY_START_PRESSED:
				Start_Clicked();
				break;
			case KEY_STOP_PRESSED:
				Stop_Clicked();
				break;
			default:
				
				break;
		}
  }
}

