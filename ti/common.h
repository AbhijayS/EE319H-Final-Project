
#ifndef __TI_COMMON_H__
#define __TI_COMMON_H__

#include <stdint.h>

// Port definitions
#define PORTA 1
#define PORTB 2
#define PORTC 4
#define PORTD 8
#define PORTE 16
#define PORTF 32

// Port pin definitions
#define PIN0 1
#define PIN1 2
#define PIN2 4
#define PIN3 8
#define PIN4 16
#define PIN5 32
#define PIN6 64
#define PIN7 128

// Pin direction definitions
#define PIN_IN 0
#define PIN_OUT 1

/*
 * Enables the clock on #port.
 * #port is defined in this header.
 * This method is port friendly.
 * This method doesn't check for invalid arguments or addresses.
 */
inline void
ti_gpio_enable_clock(volatile uint32_t *gpio_clock_register, uint8_t port)
{
	// enable port clock
	*gpio_clock_register |= port;

	// wait a few clock cycles
	__asm__{
		NOP
		NOP
		NOP
		NOP
	};
}

/*
 * Sets the direction of a #pin in the #port_direction_register.
 * #port_direction_register defined in tm4c header.
 * #pin and #port are define in this header.
 * This method is port friendly.
 * This method doesn't check for invalid arguments or addresses.
 * TODO: switch statement can be removed if use DMA and register offsets.
 */
inline void
ti_gpio_set_pin_direction(
	volatile uint32_t *port_direction_register,
	uint8_t pin,
	uint8_t direction)
{
	*port_direction_register =
		(*port_direction_register & ~(1 << pin)) | (direction << pin);
}

inline void
ti_gpio_set_pin_max_strength(volatile uint32_t *port_8ma_drive_register, uint8_t pin)
{
	*port_8ma_drive_register |= pin;
}

inline void
ti_gpio_enable_pin(volatile uint32_t *port_enable_register, uint8_t pin)
{
	*port_enable_register |= pin;
}

inline void
ti_gpio_disable_pin(volatile uint32_t *port_disable_register, uint8_t pin)
{
	*port_disable_register |= pin;
}

inline void
ti_nvic_configure(
	volatile uint32_t *nvic_enable_register,
	uint8_t interrupt_number,
	volatile uint32_t *nvic_priority_register,
	uint8_t priority_level)
{
	// enable the nvic interrupt bit (pg 142)
	*nvic_enable_register |= 1 << (interrupt_number % 32);
	
	// clear the existing priority (pg 152)
	*nvic_priority_register &= ~((7 << 5) << ((interrupt_number % 4) * 8));

	// apply the new priority level (pg 152)
	*nvic_priority_register |= (priority_level << 5) << ((interrupt_number % 4) * 8);
}

#endif
