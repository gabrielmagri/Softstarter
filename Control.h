// control.h
// Runs on  TM4C123, 
// Unisinos TGA Eletrônica 4 
// Use the SysTick timer to request interrupts at a particular period for controlling a 6-bit DAC.
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

#define NONE_CLICKED  0
#define START_CLICKED 1
#define STOP_CLICKED  2

// **************Control_Init*********************
// Initialize Systick periodic interrupts
// Also calls DAC_Init() to initialize DAC
// Input: none
// Output: none
void Control_Init(void);

// **************Start_Clicked*********************
// Sinalize that the start button was clicked
// Input: none
// Output: none
void Start_Clicked(void);

// **************Stop_Clicked*********************
// Sinalize that the stop button was clicked
// Input: none
// Output: none
void Stop_Clicked(void);
