#include "display.h"

#include <string.h>

#include "lcd.h"
#include "timer.h"

char _buffer[2][100];
size_t print_loc[2];

/**
 * Display string str on line. If the string fits on the screen, it is centered,
 * otherwise it will scroll over the line.
 */
void display_center_string(int line, const char *str) {
    size_t len = strlen(str);
    char *buf = _buffer[line];
    size_t white = 16 - len;

    if (len < 16) {
        memset(buf, ' ', white / 2);
        strcpy(buf + white / 2, str);
        for (int i = white/2 + len; i < 16; i++) {
            buf[i] = ' ';
        }
        buf[16] = '\0';
    } else {
        strncpy(buf, str, 99);
        buf[99] = '\0';
    }
}

void display_run(void) {
    static size_t pref_inc = 0;

    for(int i = 0; i < 2; i++) {
        if(_buffer[i][0] != '\0') {
            const char *buf = _buffer[i];
            size_t buf_len = strlen(buf);

            lcd_set_location(0, i);
            if(buf_len <= 16) {
                lcd_send_chars(buf, 16);
            } else {
                size_t wait_time;
                if(print_loc[i] == 0 || print_loc[i] + 16 == buf_len) {
                    wait_time = 1000;
                } else {
                    wait_time = 200;
                }
                if(buf_len > 16
                        && time_ms() - pref_inc > wait_time ) {
                    print_loc[i]++;
                    pref_inc = time_ms();
                    if (print_loc[i] + 16 > buf_len)
                        print_loc[i] = 0;
                }
                lcd_send_chars(buf + print_loc[i], 16);
            }
        }
    }
}
