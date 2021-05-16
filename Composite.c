#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "Composite.h"

void composite_video_init() {
	// enable GPIO Port E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // wait until Port E is fully enabled
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
	
	// configure PE1 and PE2 as digital outputs
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2);
	
	// select 8mA drive strength on PE1 and PE2
	GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);


}
