/*
 * ButtonPress.h
 *
 * Created: 12/27/2014 9:21:21 PM
 *  Author: Lukas
 */ 


#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>

void btn_init(uint8_t port,
        uint8_t portBit);

char gpio_val(uint8_t port, uint8_t portBit);

char btn_pressed(int buttonNumber,
        unsigned char port,
        unsigned char portBit);

#endif
