#ifndef SSD1306_H
#define SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hardware/i2c.h"

void ssd1306_init(i2c_inst_t *i2c, uint8_t addr, int width, int height);
void ssd1306_clear(void);
void ssd1306_show(void);
void ssd1306_draw_pixel(int x, int y, bool on);
void ssd1306_draw_char(int x, int y, char c);
void ssd1306_draw_string(int x, int y, const char *str);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_H
