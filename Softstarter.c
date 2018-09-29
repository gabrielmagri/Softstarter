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
#include "LEDs.h"
#include "control.h"
#include "Keyboard.h"
#include "Nokia5110.h"

// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

#define SM_INITIALIZING 0
#define SM_OPERATIONAL  1
#define SM_CONFIGURING  2
unsigned short _state = SM_INITIALIZING;
void executeStateMachine(void);
void initializeRoutine(void);
void operationalRoutine(void);
void configRoutine(void);
void checkBounds(void);

const double UPPER_BOUND = 15;
const double LOWER_BOUND = 5;
double _desiredTime = 10.0;

int main(void){
	
	PLL_Init();
	LEDs_Init();
	Keyboard_Init();
	Control_Init();
	Nokia5110_Init();
	Nokia5110_Clear();
  Nokia5110_OutString("************* LCD Test *************Letter: Num:------- ---- ");
	
  EnableInterrupts();
	
	
  for(;;) {
		executeStateMachine();
  }
}

//
void executeStateMachine() {
	switch(_state) {
		case SM_INITIALIZING:
			initializeRoutine();
			break;
		case SM_OPERATIONAL:
			operationalRoutine();
			break;
		case SM_CONFIGURING:
			configRoutine();
			break;
	}
}

//
void initializeRoutine(void) {
	//TODO write some init code, like waiting the code be ready to enable angle 0 to the TDA
	_state = SM_OPERATIONAL; //Just change to operational state
}

//
void operationalRoutine(void) {
	switch(Keyboard_In()) {
			case KEY_OP_START_PRESSED:
				Start_Clicked(5.0);
				break;
			case KEY_OP_STOP_PRESSED:
				Stop_Clicked(5.0);
				break;
			case KEY_OP_ENTER_CONFIG:
				//TODO update some info onto display
				_state = SM_CONFIGURING;
				break;
			default:
				
				break;
		}
}

//
void configRoutine(void){
	switch(Keyboard_In()) {
			case KEY_CFG_TIME_UP:
				_desiredTime++;
				checkBounds();
				break;
			case KEY_CFG_TIME_DOWN:
				_desiredTime--;
				checkBounds();
				break;
			case KEY_CFG_CHANGE_TIME_UNIT:
				
				break;
			case KEY_CFG_CONFIG_OK:
				//TODO update some info onto display
				_state = SM_OPERATIONAL;
				break;
			default:
				
				break;
		}
}

//TODO improve to circular incremental loop
void checkBounds(void) {
	if(_desiredTime > UPPER_BOUND) {
		_desiredTime = UPPER_BOUND;
	} else if(_desiredTime < LOWER_BOUND) {
		_desiredTime = LOWER_BOUND;
	}
}
