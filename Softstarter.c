// Softstarter.c
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

/* This software constitutes part of the PGA for "Eletrônica 4" subject.
 * It's role is to control the voltage level applied to the pin 11 of a
 * TCA785, aiming to control the acceleration and deceleration of a motor
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
double _desiredTime = 5.0;

#define TIME_UNIT_MILLESECONDS  0
#define TIME_UNIT_SECONDS       1
char _currentTimeUnit = 0;

char _lastMotorStatus = SM_STOPPED;

int count = 0;
int i = 0;

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
	Nokia5110_Clear();
	Nokia5110_OutString("Operational ");
	Nokia5110_OutString("------------");
	Nokia5110_OutString("Motor status");

	switch(Control_GetMotorState()) {
		case SM_STARTED:
			Nokia5110_OutString("  STARTED   ");
			break;
		case SM_STOPPED:
			Nokia5110_OutString("  STOPPED   ");
			break;
		case SM_STARTING:
			Nokia5110_OutString("  STARTING  ");
			break;
		case SM_STOPPING:
			Nokia5110_OutString("  STOPPING  ");
			break;
		default:
			break;
	}

	Nokia5110_OutString("------------");
	Nokia5110_OutString("Time: ");
	Nokia5110_OutDouble(_desiredTime);
	_state = SM_OPERATIONAL; //Just change to operational state
}

//
void operationalRoutine(void) {
	
	if(_lastMotorStatus != Control_GetMotorState()) {
		Nokia5110_SetCursor(0, 3);
		switch(Control_GetMotorState()) {
			case SM_STARTED:
				Nokia5110_OutString("  STARTED   ");
				break;
			case SM_STOPPED:
				Nokia5110_OutString("  STOPPED   ");
				break;
			case SM_STARTING:
				Nokia5110_OutString("  STARTING  ");
				break;
			case SM_STOPPING:
				Nokia5110_OutString("  STOPPING  ");
				break;
			default:
				break;
		}
	}
	
	switch(Keyboard_In()) {
			case KEY_OP_START_PRESSED:
				Start_Clicked(5.0);
				break;
			case KEY_OP_STOP_PRESSED:
				Stop_Clicked(5.0);
				break;
			case KEY_OP_ENTER_CONFIG:
				Nokia5110_Clear();
				Nokia5110_OutString("Config MENU:");
				Nokia5110_OutString("------------");
				Nokia5110_OutString("Time Unit:");
				if(_currentTimeUnit) Nokia5110_OutString(" S");
				if(!_currentTimeUnit) Nokia5110_OutString("mS");
				Nokia5110_OutString("            ");
				Nokia5110_OutString("Time: ");
				Nokia5110_OutDouble(_desiredTime);
				_state = SM_CONFIGURING;
				break;
			default:
				
				break;
		}
}

//
void configRoutine(void){
	//First read to burn the first event that would be KEY_CFG_CONFIG_OK
	// , because that's the key pressed to come from operational to config.
	//Keyboard_Continuous_In();
	
	switch(Keyboard_In()) {
		case KEY_CFG_CHANGE_TIME_UNIT:
			_currentTimeUnit = !_currentTimeUnit;
			Nokia5110_SetCursor(10, 2);
			if(_currentTimeUnit) Nokia5110_OutString(" S");
			if(!_currentTimeUnit) Nokia5110_OutString("mS");
			break;
		case KEY_CFG_CONFIG_OK:
			Nokia5110_Clear();
			Nokia5110_OutString("Operational ");
			Nokia5110_OutString("------------");
			Nokia5110_OutString("Motor status");
		
			switch(Control_GetMotorState()) {
				case SM_STARTED:
					Nokia5110_OutString("  STARTED   ");
					break;
				case SM_STOPPED:
					Nokia5110_OutString("  STOPPED   ");
					break;
				case SM_STARTING:
					Nokia5110_OutString("  STARTING  ");
					break;
				case SM_STOPPING:
					Nokia5110_OutString("  STOPPING  ");
					break;
				default:
					break;
			}
		
			Nokia5110_OutString("------------");
			Nokia5110_OutString("Time: ");
			Nokia5110_OutDouble(_desiredTime);
			
			_state = SM_OPERATIONAL;
			break;
		default:
			break;
	}
	
	switch(Keyboard_Continuous_In()) {
			case KEY_CFG_TIME_UP:
				if(_currentTimeUnit == TIME_UNIT_MILLESECONDS) {
					_desiredTime = _desiredTime+0.001;
				} else if(_currentTimeUnit == TIME_UNIT_SECONDS) {
					_desiredTime++;
				}
				
				checkBounds();
				Nokia5110_SetCursor(6, 4);
				Nokia5110_OutDouble(_desiredTime);
				break;
			case KEY_CFG_TIME_DOWN:
				if(_currentTimeUnit == TIME_UNIT_MILLESECONDS) {
					_desiredTime = _desiredTime-0.001;
				} else if(_currentTimeUnit == TIME_UNIT_SECONDS) {
					_desiredTime--;
				}
				
				checkBounds();
				Nokia5110_SetCursor(6, 4);
				Nokia5110_OutDouble(_desiredTime);
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
