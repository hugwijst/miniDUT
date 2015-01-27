/*
 * ButtonPress.h
 *
 * Created: 12/27/2014 9:21:21 PM
 *  Author: Lukas
 */ 


#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

char btn_pressed(int buttonNumber,
        unsigned char port,
        unsigned char portBit,
        uint16_t debounceTime);

#endif
