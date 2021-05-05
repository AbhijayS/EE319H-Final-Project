// WavPlay.c
// Ramesh Yerraballi

// 4-bit DAC on PB0-3

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Buttons.h"
#include "Texas.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void DAC_Init(void);

int main(void){
	TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  DAC_Init();
  Button_Init();
  EnableInterrupts();
  while(1){}
}
