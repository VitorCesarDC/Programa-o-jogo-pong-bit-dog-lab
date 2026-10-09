#include "ssd1306.h"
#include "font8x8_basic.h"
#include "pico/stdlib.h"
#include <string.h>

static i2c_inst_t *s_i2c = NULL;
static uint8_t s_addr = 0x3C;
static int s_width = 128;
static int s_height = 64;
static uint8_t s_buffer[1024];

static void ssd1306_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(s_i2c, s_addr, buf, 2, false);
}

static void ssd1306_send_cmd_list(const uint8_t *cmds, size_t count) {
    uint8_t buf[32];
    if (count > 30) return;
    buf[0] = 0x00; // Command stream
    memcpy(&buf[1], cmds, count);
    i2c_write_blocking(s_i2c, s_addr, buf, count + 1, false);
}

void ssd1306_init(i2c_inst_t *i2c, uint8_t addr, int width, int height) {
    s_i2c = i2c;
    s_addr = addr;
    s_width = width;
    s_height = height;

    static const uint8_t init_cmds[] = {
        0xAE,         // Set Display Off
        0xD5, 0x80,   // Set Display Clock Divide Ratio / Osc Freq
        0xA8, 0x3F,   // Set Multiplex Ratio (64 - 1 = 63 = 0x3F)
        0xD3, 0x00,   // Set Display Offset = 0
        0x40,         // Set Display Start Line = 0
        0x8D, 0x14,   // Enable Charge Pump
        0x20, 0x00,   // Set Memory Addressing Mode (Horizontal)
        0xA1,         // Set Segment Re-Map (Column 127 mapped to SEG0)
        0xC8,         // Set COM Output Scan Direction (remapped mode)
        0xDA, 0x12,   // Set COM Pins Hardware Configuration
        0x81, 0xCF,   // Set Contrast Control
        0xD9, 0xF1,   // Set Pre-Charge Period
        0xDB, 0x40,   // Set VCOMH Deselect Level
        0xA4,         // Entire Display On (Resume to RAM content)
        0xA6,         // Set Normal Display (0 = off, 1 = on)
        0xAF          // Set Display On
    };

    ssd1306_send_cmd_list(init_cmds, sizeof(init_cmds));
    ssd1306_clear();
    ssd1306_show();
}

void ssd1306_clear(void) {
    memset(s_buffer, 0, sizeof(s_buffer));
}

void ssd1306_show(void) {
    if (!s_i2c) return;

    static const uint8_t setup_cmds[] = {
        0x21, 0, 127, // Set Column Address (0 to 127)
        0x22, 0, 7    // Set Page Address (0 to 7)
    };
    ssd1306_send_cmd_list(setup_cmds, sizeof(setup_cmds));

    // Envia o buffer com o prefixo de dados 0x40
    uint8_t payload[1025];
    payload[0] = 0x40;
    memcpy(&payload[1], s_buffer, 1024);
    i2c_write_blocking(s_i2c, s_addr, payload, 1025, false);
}

void ssd1306_draw_pixel(int x, int y, bool on) {
    if (x < 0 || x >= s_width || y < 0 || y >= s_height) return;
    int page = y / 8;
    int bit = y % 8;
    int index = x + page * s_width;

    if (on) {
        s_buffer[index] |= (1 << bit);
    } else {
        s_buffer[index] &= ~(1 << bit);
    }
}

void ssd1306_draw_char(int x, int y, char c) {
    uint8_t ch = (uint8_t)c;
    if (ch >= 128) ch = ' ';

    for (int row = 0; row < 8; row++) {
        uint8_t line = (uint8_t)font8x8_basic[ch][row];
        for (int col = 0; col < 8; col++) {
            if ((line >> col) & 0x01) {
                ssd1306_draw_pixel(x + col, y + row, true);
            }
        }
    }
}

void ssd1306_draw_string(int x, int y, const char *str) {
    if (!str) return;
    int cursor_x = x;
    int cursor_y = y;

    while (*str) {
        if (*str == '\n') {
            cursor_x = x;
            cursor_y += 8;
        } else {
            ssd1306_draw_char(cursor_x, cursor_y, *str);
            cursor_x += 8;
        }
        str++;
    }
}
