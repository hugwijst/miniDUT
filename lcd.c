/*
 * lcd.c
 *
 * Created: 12/28/2014 9:00:01 PM
 *  Author: Lukas
 */ 

#include "lcd.h"

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>

// PIN definitions
#define MrLCDsCrib 				PORTB 		// Set where the LCD is connected
#define DataDir_MrLCDsCrib		DDRB
#define MrLCDsControl 			PORTD 		// Set where the LCD is controlled
#define DataDir_MrLCDsControl	DDRD
#define LightSwitch 			5
#define ReadWrite 				7
#define BiPolarMood 			2

// Create routines
void Peek_A_Boo(void);
void lcd_available(void);

void Peek_A_Boo() {
    MrLCDsControl |= 1<<LightSwitch;
    asm volatile ("nop");
    asm volatile ("nop");
    MrLCDsControl &= ~1<<LightSwitch;
}

void lcd_available(void) {
    DataDir_MrLCDsCrib = 0;
    MrLCDsControl |= 1<<ReadWrite;
    MrLCDsControl &= ~1<<BiPolarMood;

    while (MrLCDsCrib >= 0x80)		// 0b10000000 = 0x80 (always a one in the pin 7)
    {
        Peek_A_Boo();
    }

    DataDir_MrLCDsCrib = 0xFF; 	//0xFF means 0b11111111
}

void lcd_init() {
    // Initialization
    DataDir_MrLCDsControl |= 1 <<LightSwitch | 1<<ReadWrite | 1<<BiPolarMood;
    _delay_ms(15); 				//Needed for startup of LCD

    // Clear the screen
    lcd_send_cmd(LCD_CLEAR_SCREEN);
    _delay_ms(2);
    // Set function set register
    // Configuration: 8-bit data length, 2 display lines, 5x11 dots font
    lcd_send_cmd(0x38);
    _delay_us(50);
    // Set display control register
    // Configuration: Display on, no cursor and no blinking of cursor
    lcd_send_cmd(0b00001100);
    _delay_us(50);
}

void lcd_send_cmd(LcdCommand cmd) {
    lcd_available();
    MrLCDsCrib = cmd & 0xFF;
    MrLCDsControl &= ~ (1<<ReadWrite|1<<BiPolarMood);
    Peek_A_Boo();
    MrLCDsCrib = 0;
}

void lcd_set_location(uint8_t x, uint8_t y) {
    // Set cursor to (x,y)
    lcd_send_cmd(0x80 + 64*y + x);
}

void lcd_send_char(unsigned char ch) {
    lcd_available();
    MrLCDsCrib = ch;
    MrLCDsControl &= ~ (1<<ReadWrite);
    MrLCDsControl |= 1<<BiPolarMood;
    Peek_A_Boo();
    MrLCDsCrib = 0;
}

void lcd_send_chars(const char *chs, size_t len) {
    while(len--) {
        lcd_send_char(*chs++);
    }
}

void lcd_send_string(const char *str) {
    while (*str != 0) {
        lcd_send_char(*str++);
    }
}
