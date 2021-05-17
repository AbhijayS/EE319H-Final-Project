#include <stdint.h>
#include "Composite.h"
#include "inc/tm4c123gh6pm.h"
#include "ti/common.h"

void composite_video_init()
{
	// enable Port E clock
	ti_gpio_enable_clock(PORTE);

	// configure PE1 and PE2 as outputs
	ti_gpio_set_pin_direction(&GPIO_PORTE_DIR_R, PIN1, PIN_OUT);
	ti_gpio_set_pin_direction(&GPIO_PORTE_DIR_R, PIN2, PIN_OUT);

}
