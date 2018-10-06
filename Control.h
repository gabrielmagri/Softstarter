// control.h
// Runs on  TM4C123, 
// Unisinos TGA Eletrônica 4 
// Use the SysTick timer to request interrupts at a particular period for controlling a 6-bit DAC.
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

#define NONE_CLICKED  0
#define START_CLICKED 1
#define STOP_CLICKED  2

#define NOT_FLAGGED 0
#define FLAGGED     1

#define SM_STARTED  0
#define SM_STARTING 1
#define SM_STOPPING 2
#define SM_STOPPED  3

static const double SYSTEM_TIME = 0.0000000125; // The system time when running at 80MHz
static const int DEFAULT_RELOAD = 1860;         // Reload value as defined on the comments above (file's top) 
static const short DATA_SIZE    = 44;           // The amount of data that are defined and can be "outputed" through DAC.

// ************Control_GetMotorState*******************
// Returns the current motor state
// Input: none
// Output: short - the motor state
short Control_GetMotorState(void);

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
void Start_Clicked(double desiredTime);

// **************Stop_Clicked*********************
// Sinalize that the stop button was clicked
// Input: none
// Output: none
void Stop_Clicked(double desiredTime);
