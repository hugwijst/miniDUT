#include "button.h"

#include <avr/io.h>

#include "timer.h"

#define NUMBER_OF_BUTTONS 5
#define DEBOUNCE_TIME 100

volatile uint8_t* ports[4] = { &PORTA, &PORTB, &PORTC, &PORTD };
volatile uint8_t* ddrs[4] = { &DDRA, &DDRB, &DDRC, &DDRD };

void btn_init(uint8_t port,
        uint8_t portBit) {
    SFIOR &= ~(1 << PUD);
    *ddrs[port] &= ~(1 << portBit);
    *ports[port] |= 1 << portBit;
}

char gpio_val(uint8_t port, uint8_t portBit) {
    return bit_is_clear(port, portBit);
}

char Pressed[NUMBER_OF_BUTTONS];
// Time at which the button was last seen as pressed
uint16_t LastPressedTime[NUMBER_OF_BUTTONS];
// Time at which the button was last seen as released
uint16_t LastReleasedTime[NUMBER_OF_BUTTONS];

/**
 * Check if a button is considered as pressed.
 *
 * @param buttonNumber The internal number of the button.
 * @param port The port of the pin.
 * @param portBit The bit on the pin on which the button is connected.
 * @param debounceTime The amount of time that has to elapse before the state is changed
 */
char btn_pressed(int buttonNumber,
        unsigned char port,
        unsigned char portBit)
{
    uint16_t cur_time = time_ms();
	if (bit_is_clear(port, portBit))
	{
		LastPressedTime[buttonNumber] = cur_time;						//Increase Pressed Confidence
		if ((uint16_t) (cur_time - LastReleasedTime[buttonNumber]) > DEBOUNCE_TIME)	//Indicator of good button press
		{
            if (Pressed[buttonNumber] == 0) {
                Pressed[buttonNumber] = 1;
                return 1;
            }
		}
	}
	else
	{
		LastReleasedTime[buttonNumber] = cur_time;						//This works just like the pressed
		if ((uint16_t) (cur_time - LastPressedTime[buttonNumber]) > DEBOUNCE_TIME)
		{
			Pressed[buttonNumber] = 0;
		}
	}
	return 0;
}
