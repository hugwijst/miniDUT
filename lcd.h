#ifndef LCD_H
#define LCD_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    LCD_CLEAR_SCREEN = 0x1,
} LcdCommand;

void lcd_init(void);
void lcd_send_cmd(LcdCommand cmd);
void lcd_set_location(uint8_t x, uint8_t y);
void lcd_send_char(unsigned char ch);
void lcd_send_chars(const char *chs, size_t len);
void lcd_send_string(const char *str);

#endif
