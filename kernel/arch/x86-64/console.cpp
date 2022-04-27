#include <stdarg.h>

#include "include/console.hpp"
#include "include/stl/types.hpp"
#include "include/stl/string.hpp"

namespace
{
  enum vga_color
  {
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    LIGHT_MAGENTA = 13,
    LIGHT_BROWN = 14,
    WHITE = 15,
  };

  uint8_t make_color(vga_color fg, vga_color bg)
  {
    return fg | bg << 4;
  }

  uint16_t make_vga_entry(char c, uint8_t color)
  {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
  }

  template <typename N>
  uint64_t digits(N number)
  {
    if (number < 10)
    {
      return 1;
    }

    uint64_t i = 0;

    while (number != 0)
    {
      number /= 10;
      ++i;
    }

    return i;
  }

  template <int B, typename D>
  void print_unsigned(D number)
  {
    if (number == 0)
    {
      k_print('0');
      return;
    }

    char buffer[B];
    int i = 0;

    while (number != 0)
    {
      buffer[i++] = '0' + number % 10;
      number /= 10;
    }

    --i;

    for (; i >= 0; --i)
    {
      k_print(buffer[i]);
    }
  }

  template <int B, typename U, typename D>
  void print_signed(D number)
  {
    if (number < 0)
    {
      k_print('-');
      print_unsigned<B>(static_cast<U>(-1 * number));
    }
    else
    {
      print_unsigned<B>(static_cast<U>(number));
    }
  }

}

void set_column(long column)
{
  current_column = column;
}

long get_column()
{
  return current_column;
}

void set_line(long line)
{
  current_line = line;
}

long get_line()
{
  return current_line;
}

void k_print(uint8_t number)
{
  print_unsigned<3>(number);
}

void k_print(uint16_t number)
{
  print_unsigned<5>(number);
}

void k_print(uint32_t number)
{
  print_unsigned<10>(number);
}

void k_print(uint64_t number)
{
  print_unsigned<20>(number);
}

void k_print(int8_t number)
{
  print_signed<3, uint8_t>(number);
}

void k_print(int16_t number)
{
  print_signed<5, uint16_t>(number);
}

void k_print(int32_t number)
{
  print_signed<10, uint32_t>(number);
}

void k_print(int64_t number)
{
  print_signed<20, uint64_t, int64_t>(number);
}

void next_line()
{
  ++current_line;

  if (current_line == 25)
  {
    auto vga_buffer_fast = reinterpret_cast<uint64_t *>(0x0B8000);
    auto destination = vga_buffer_fast;
    auto source = &vga_buffer_fast[20];

    std::copy_n(destination, source, 24 * 20);

    auto vga_buffer = reinterpret_cast<uint16_t *>(0x0B8000);
    for (uint64_t i = 0; i < 80; ++i)
    {
      vga_buffer[24 * 80 + i] = make_vga_entry(' ', make_color(WHITE, BLACK));
    }

    current_line = 24;
  }

  current_column = 0;
}

void k_print(char key)
{
  if (key == '\n')
  {
    next_line();
  }
  else if (key == '\b')
  {
    --current_column;
    k_print(' ');
    --current_column;
  }
  else if (key == '\t')
  {
    k_print("  ");
  }
  else
  {
    uint16_t *vga_buffer = reinterpret_cast<uint16_t *>(0x0B8000);

    vga_buffer[current_line * 80 + current_column] = make_vga_entry(key, make_color(WHITE, BLACK));

    ++current_column;

    if (current_column == 80)
    {
      next_line();
    }
  }
}

void k_print(const char *str)
{
  for (uint64_t i = 0; str[i] != 0; ++i)
  {
    k_print(str[i]);
  }
}

void k_print(const std::string &s)
{
  for (auto c : s)
  {
    k_print(c);
  }
}

void k_print(const char *str, uint64_t end)
{
  for (uint64_t i = 0; str[i] != 0 && i < end; ++i)
  {
    k_print(str[i]);
  }
}

void wipeout()
{
  current_line = 0;
  current_column = 0;

  for (int line = 0; line < 25; ++line)
  {
    for (uint64_t column = 0; column < 80; ++column)
    {
      k_print(' ');
    }
  }

  current_line = 0;
  current_column = 0;
}

void k_printf(const char *fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  char ch;

  while ((ch = *(fmt++)))
  {
    if (ch != '%')
    {
      k_print(ch);
    }
    else
    {
      ch = *(fmt++);

      size_t min_width = 0;
      while (ch >= '0' && ch <= '9')
      {
        min_width = 10 * min_width + (ch - '0');
        ch = *(fmt++);
      }

      size_t min_digits = 0;
      if (ch == '.')
      {
        ch = *(fmt++);

        while (ch >= '0' && ch <= '9')
        {
          min_digits = 10 * min_digits + (ch - '0');
          ch = *(fmt++);
        }
      }

      auto prev = current_column;

      // Signed decimal
      if (ch == 'd')
      {
        auto arg = va_arg(va, int64_t);

        if (min_digits > 0)
        {
          size_t d = digits(arg);
          if (min_digits > d)
          {
            min_digits -= d;

            if (arg < 0)
            {
              arg *= -1;
              k_print('-');
            }

            while (min_digits > 0)
            {
              while (min_digits > 0)
              {
                k_print('0');
                --min_digits;
              }
            }
          }
        }

        k_print(arg);
      }
      // Unsigned Decimal
      else if (ch == 'u')
      {
        auto arg = va_arg(va, uint64_t);

        if (min_digits > 0)
        {
          size_t d = digits(arg);
          if (min_digits > d)
          {
            min_digits -= d;
            while (min_digits > 0)
            {
              while (min_digits > 0)
              {
                k_print('0');
                --min_digits;
              }
            }
          }
        }

        k_print(arg);
      }
      // Hexadecimal
      else if (ch == 'h')
      {
        k_print("0x");

        uint8_t buffer[20];

        auto arg = va_arg(va, uint64_t);
        size_t i = 0;

        while (arg / 16 != 0)
        {
          buffer[i++] = arg % 16;
          arg /= 16;
        }

        buffer[i] = arg;

        if (min_digits > 0 && min_digits > i)
        {
          min_digits -= i + 1;
          while (min_digits > 0)
          {
            k_print('0');
            --min_digits;
          }
        }

        while (true)
        {
          uint8_t digit = buffer[i];

          if (digit < 10)
          {
            k_print(static_cast<char>('0' + digit));
          }
          else
          {
            switch (digit)
            {
            case 10:
              k_print('A');
              break;
            case 11:
              k_print('B');
              break;
            case 12:
              k_print('C');
              break;
            case 13:
              k_print('D');
              break;
            case 14:
              k_print('E');
              break;
            case 15:
              k_print('F');
              break;
            }
          }

          if (i == 0)
          {
            break;
          }

          --i;
        }
      }
      // Memory
      else if (ch == 'm')
      {
        auto memory = va_arg(va, uint64_t);

        if (memory >= 1024 * 1024 * 1024)
        {
          k_print(memory / (1024 * 1024 * 1024));
          k_print("GiB");
        }
        else if (memory >= 1024 * 1024)
        {
          k_print(memory / (1024 * 1024));
          k_print("MiB");
        }
        else if (memory >= 1024)
        {
          k_print(memory / 1024);
          k_print("KiB");
        }
        else
        {
          k_print(memory);
          k_print("B");
        }
      }
      // String
      else if (ch == 's')
      {
        auto arg = va_arg(va, const char *);
        k_print(arg);
      }

      if (min_width > 0)
      {
        size_t width = current_column - prev;

        if (min_width > width)
        {
          min_width -= width;

          while (min_width > 0)
          {
            k_print(' ');
            --min_width;
          }
        }
      }
    }
  }

  va_end(va);
}
