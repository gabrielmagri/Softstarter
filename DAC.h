// DAC.h
// Runs on TM4C123, 
// Unisinos TGA Eletrônica 4 
// 4-bit Digital analog converter
// Gabriel Magri, Jaqueline Isabel Prass, Marcos
// September 18, 2018

// **************DAC_Init*********************
// Initialize 4-bit DAC 
// Input: none
// Output: none
void DAC_Init(void);


// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(unsigned long data);

