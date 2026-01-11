#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static void int_to_str(char *buf, int val) {
  char tmp[20];
  int i = 0, j = 0;
  int is_negative = 0;
  if (val == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }
  if (val < 0) {
    is_negative = 1;
    val = -val;
  }
  while (val > 0) {
    tmp[i++] = '0' + (val % 10);
    val /= 10;
  }
  if (is_negative) {
    buf[j++] = '-';
  }
  while (i > 0) {
    buf[j++] = tmp[--i];
  }
  buf[j] = '\0';
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  char *p = out;

  while (*fmt) {
    if (*fmt == '%') {
      fmt++;
      switch (*fmt) {
        case 'd': {
          int val = va_arg(ap, int);
          char buf[20];
          int_to_str(buf, val);
          char *s = buf;
          while (*s) {
            *p++ = *s++;
          }
          break;
        }
        case 's': {
          char *s = va_arg(ap, char*);
          if (s == NULL) {
            s = "(null)";
          }
          while (*s) {
            *p++ = *s++;
          }
          break;
        }
        case 'c': {
          char c = (char)va_arg(ap, int);
          *p++ = c;
          break;
        }
        case 'x': {
          unsigned int val = va_arg(ap, unsigned int);
          char hex_buf[20];
          int i = 0;
          if (val == 0) {
            hex_buf[i++] = '0';
          } else {
            while (val > 0) {
              int digit = val & 0xf;
              hex_buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
              val >>= 4;
            }
          }
          while (i > 0) {
            *p++ = hex_buf[--i];
          }
          break;
        }
        case 'p': {
          uintptr_t ptr = (uintptr_t)va_arg(ap, void*);
          *p++ = '0';
          *p++ = 'x';
          char hex_buf[20];
          int i = 0;
          if (ptr == 0) {
              hex_buf[i++] = '0';
          } else {
              while (ptr > 0) {
                int digit = ptr & 0xf;
                hex_buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
                ptr >>= 4;
              }
          }
          while (i > 0) {
              *p++ = hex_buf[--i];
          }
          break;
        }
        case '%': {
          *p++ = '%';
          break;
        }
        default:
          *p++ = '%';
          *p++ = *fmt;
          break;
      }
      fmt++;
    } else {
      *p++ = *fmt++;
    }
  }
  *p = '\0';
  return p - out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  int len;
  va_start(ap, fmt);
  len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  int len;
  va_start(ap, fmt);
  len = vsprintf(out, fmt, ap);
  va_end(ap);
  return len;
}

int printf(const char *fmt, ...) {
  char buffer[1024];
  va_list ap;
  int len;

  va_start(ap, fmt);
  len = vsprintf(buffer, fmt, ap);
  va_end(ap);

  for (int i = 0; i < len; i++) {
    _putc(buffer[i]);
  }

  return len;
}

#endif
