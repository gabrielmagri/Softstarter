// Keyboard.h
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// There are two keys in the keyboard
// Gabriel Magri, Jaqueline Isabel Prass, Marcos Spellmeier
// September 18, 2018

#define KEY_START_PRESSED 0X01
#define KEY_STOP_PRESSED  0X02

// **************Keyboard_Init*********************
// Initialize keyboard key inputs
// Input: none
// Output: none
void Keyboard_Init(void);

// **************Keyboard_In*********************
// Input from keyboard key inputs
// Input: none 
// Output: 0 to 3 depending on keys
// 0x01 is the start key pressed, 0x02 is the stop key pressed
unsigned long Keyboard_In(void);
