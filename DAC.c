// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 7/7/2014 
// Last Modified:  
// Lab number: 5
// Hardware connections

#include "tm4c123gh6pm.h"
#include <stdint.h>
// put code definitions for the software (actual C code)
// this file explains how the module works


// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
// PortB pins3-0 is the DAC
  SYSCTL_RCGCGPIO_R |= 0x02;           // activate port B
  while((SYSCTL_PRGPIO_R&0x02)==0){}; // allow time for clock to start
  GPIO_PORTB_AMSEL_R &= ~0x0F;// disable analog on PB3-0
  GPIO_PORTB_PCTL_R &= ~0x0000FFFF; // configure PB3-0 as GPIO
  GPIO_PORTB_DIR_R |= 0x0F;   // make PB3-0 out
  GPIO_PORTB_AFSEL_R &= 0x0F; // disable alt funct on PB3-0
  GPIO_PORTB_DEN_R |= 0x0F;   // enable digital I/O on PB3-0
  GPIO_PORTB_DR8R_R |= 0x0F;  // enable 8 mA drive on PB3-0
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
void DAC_Out(uint32_t data){
	    GPIO_PORTB_DATA_R = data;
}	 
