#ifndef VGA_HPP
#define VGA_HPP

#include <stddef.h>
#include <stdint.h>
#include "../string.hpp"

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

extern size_t terminal_row;
extern size_t terminal_column;
extern uint8_t terminal_color;
extern uint16_t* terminal_buffer;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
uint16_t vga_entry(char c, uint8_t color);

void terminal_initialize();
void terminal_setcolor(uint8_t fg, uint8_t bg);
void terminal_putentry(uint8_t c, uint8_t color, size_t x, size_t y);
void terminal_putchar(uint8_t c);
void terminal_writestring(const char* data);

#endif
