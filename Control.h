// Sound.c
// Runs on  TM4C123, 
// Unisinos TGA Eletrônica 4 
// Use the SysTick timer to request interrupts at a particular period.
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

#define C_NOTE 5095
#define D_NOTE 4539
#define E_NOTE 4044
#define G_NOTE 3400

// **************Sound_Init*********************
// Initialize Systick periodic interrupts
// Also initializes DAC
// Input: none
// Output: none
void Sound_Init(void);

// **************Sound_Tone*********************
// Change SysTick periodic interrupts to start sound output
// Input: interrupt period
//           Units of period are 12.5ns
//           Maximum is 2^24-1
//           Minimum is determined by length of ISR
// Output: none
void Sound_Tone(unsigned long period);


// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void);

