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
 * 
 * UPDATES:
 * 43kHz the time is 1/f = 23.25581395x10^-6sec.
 * NVIC_ST_RELOAD_R of (23.25581395uS/12.5nS - 1) that is equals to 1860.46 or 1860.
 * 
 */

#include "control.h"
#include "DAC.h"
#include "LEDs.h"
#include "tm4c123gh6pm.h"
#include "Nokia5110.h"
//#include "Debug.h"

//
const unsigned char output[DATA_SIZE] = {63, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 
	25, 24, 23, 22, 21, 20, 19, 18, 17, 16 ,15 ,14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0};

unsigned short _dataIndex = 0;                // Index varies from 0 to 43.
unsigned int   _interruptsCounter = 0;        // The counter that needs to reach some value to generate the DAC out.
unsigned int   _interruptsToBeReached = 9997; // The variable that holds the current amount of interrupts to wait before output to DAC
unsigned char  _buttonClicked = NONE_CLICKED; // The variable that holds the button events.
//unsigned char  _startingFlag = NOT_FLAGGED;   // A flag to indicate that the motor is on the middle of a start process.
//unsigned char  _stoppingFlag = NOT_FLAGGED;   // A flag to indicate that the motor is on the middle of a stop process.
//unsigned char  _startedFlag = NOT_FLAGGED;    // A flag to indicate that the motor has finished the start process.


unsigned short _motorState = SM_STOPPED; // The motor control state initiate as stopped.

short Control_GetMotorState(void) {
	return _motorState;
}
	
//
void Systick_Init(void) {
	
	_interruptsToBeReached = 9997;                // The nearest value when aiming for 10 seconds on the output total time	
	
	NVIC_ST_CTRL_R = 0;                           // disable SysTick during setup
  NVIC_ST_RELOAD_R = DEFAULT_RELOAD;            // reload value
  NVIC_ST_CURRENT_R = 0;                        // any write to current clears it
  NVIC_SYS_PRI3_R = NVIC_SYS_PRI3_R&0x00FFFFFF; // priority 0               
  NVIC_ST_CTRL_R = 0x00000007;                  // enable with core clock and interrupts

}

void updateInterruptsToBeReached(double desiredTime) {
	double doubleLoops = desiredTime/( (double)(DATA_SIZE-1) * SYSTEM_TIME * (double)(DEFAULT_RELOAD+1) );
	_interruptsToBeReached = (int)(doubleLoops + 0.5); //Round to the nearest aproach
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
// Input: A double representing the desired total time to
//        the motor speeding up action. Precision 1mS.
// Output: none
void Start_Clicked(double desiredTime){
	// Only allow to start if the motor is currently stopping or stopped
	if( (_motorState == SM_STOPPED) || (_motorState == SM_STOPPING) ) {
		updateInterruptsToBeReached(desiredTime);
	  _buttonClicked = START_CLICKED;
	}
}

// **************Stop_Clicked*********************
// Sinalize that the stop button was clicked
// Input: A double representing the desired total time to
//        the motor slow down action. Precision 1mS.
// Output: none
void Stop_Clicked(double desiredTime){
	// Only allow to stop if the motor is currently starting or started
	if( (_motorState == SM_STARTED) || (_motorState == SM_STARTING) ) {
		updateInterruptsToBeReached(desiredTime);
		_buttonClicked = STOP_CLICKED;
	}
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
	//Debug_TooglePin3();
	
	// ------------------------------------------------------------------
	// Check for button clicks notified by any caller.
	// ------------------------------------------------------------------
	switch(_buttonClicked) {
		case START_CLICKED:
			_motorState = SM_STARTING;
		  _interruptsCounter = _interruptsToBeReached; // force to immediately update
			_buttonClicked = NONE_CLICKED; // Clear the click
			break;
		case STOP_CLICKED:
			_motorState = SM_STOPPING;
			_interruptsCounter = _interruptsToBeReached; // force to immediately update
			_buttonClicked = NONE_CLICKED; // Clear the click
			break;
		case NONE_CLICKED:
			break;
		default:
			break;
	}
	// ------------------------------------------------------------------
	
	// ------------------------------------------------------------------
	// If the amount of interrupts was reachead, execute the core logic to DAC out.
	// ------------------------------------------------------------------
	if(_interruptsCounter>=_interruptsToBeReached) {
		_interruptsCounter=0; //Reset the counter
		
		if( _motorState == SM_STARTING ) {
			LEDs_Green();
			if(_dataIndex>=43) { //Reached the end of starting
				_dataIndex = 43; //Lock the index into 43 to hold the angle as 0
				_motorState = SM_STARTED; //Indicate that the motor reached the nominal motor P
			} else {
				_dataIndex++;
			}
		} else if( _motorState == SM_STOPPING ) {
			LEDs_Red();
			if(_dataIndex==0) { //Reached the end of stopping
				_dataIndex = 0; //Lock the index into 0 to hold the angle as 180
				_motorState = SM_STOPPED; //Indicate that the motor is now completely stopped
			} else {
				_dataIndex--;
			}
		} else {
			if( _motorState == SM_STARTED ) {
				LEDs_Blue();
			} else {
				LEDs_None();
			}
		}
		
		DAC_Out(output[_dataIndex]); //output one value each "_interruptsToBeReached" interrupts
	}
	// ------------------------------------------------------------------
	
	_interruptsCounter++;
	
}
