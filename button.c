#include "button.h"

#include <avr/io.h>

#include "timer.h"

#define NUMBER_OF_BUTTONS 5

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
        unsigned char portBit,
        uint16_t debounceTime)
{
    uint16_t cur_time = time_ms();
	if (bit_is_clear(port, portBit))
	{
		LastPressedTime[buttonNumber] = cur_time;						//Increase Pressed Confidence
		if (LastReleasedTime[buttonNumber] - cur_time > debounceTime)	//Indicator of good button press
		{
            Pressed[buttonNumber] = 1;
		}
	}
	else
	{
		LastReleasedTime[buttonNumber] = cur_time;						//This works just like the pressed
		if (LastPressedTime[buttonNumber] - cur_time > debounceTime)
		{
			Pressed[buttonNumber] = 0;
		}
	}
	return Pressed[buttonNumber];
}
