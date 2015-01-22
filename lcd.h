#ifndef LCD_H
#define LCD_H

typedef enum {
    LCD_CLEAR_SCREEN = 0x1,
} LcdCommand;

void lcd_init(void);
void lcd_send_cmd(LcdCommand cmd);
void lcd_send_char(unsigned char ch);

#endif
