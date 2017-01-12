#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <strings.h>

int main(int argc, char **argv) {
    const size_t bufferLength = 128;
    char buffer[bufferLength];

    snprintf(buffer, bufferLength, "first last");
    assert(0 == strcmp(buffer, "first last"));

    snprintf(buffer, bufferLength, "%s %s", "first", "last");
    assert(0 == strcmp(buffer, "first last"));

    snprintf(buffer, bufferLength, "%s %d %s", "first", 2, "last");
    assert(0 == strcmp(buffer, "first 2 last"));

    snprintf(buffer, bufferLength, "%s %d %s %d", "first", 294967296, "last", 48395);
    assert(0 == strcmp(buffer, "first 294967296 last 48395"));

    snprintf(buffer, bufferLength, "%s %d %s %d", "first", 294967296, "last", -48395);
    assert(0 == strcmp(buffer, "first 294967296 last -48395"));
}

int write_char(char * restrict buf, size_t size, size_t * buf_idx, char c) {
    if (*buf_idx < size) {
        buf[(*buf_idx)++] = c;
        return 0;
    }
    return -1;
}

int write_signed_int(char * restrict str, size_t size, size_t * str_idx, int d) {
    char int_buf[10];
    size_t int_buf_idx = 0;

    if (d < 0) {
        printf("negative\n");
        d = ~d + 1;
        printf("new int arg: %d\n", d);
        if (write_char(str, size, str_idx, '-') < 0) {
            return -1;
        }
    }

    while (d && int_buf_idx < 10) {
        printf("int char: %c\n", (d % 10) + '0');
        int_buf[int_buf_idx++] = (d % 10) + '0';
        d = d / 10;
    }

    while (int_buf_idx) {
        printf("copying int char: %d\n", int_buf[int_buf_idx]);
        if (write_char(str, size, str_idx, int_buf[--int_buf_idx]) < 0) {
            return -1;
        }
    }

    return 0;
}

int write_string(char * restrict str, size_t size, size_t * str_idx, char * s) {
    while (*s) {
        printf("string char: %c\n", *s);
        if (write_char(str, size, str_idx, *s) < 0) {
            return -1;
        }
        s++;
    }
    return 0;
}

int snprintf(char * restrict str, size_t size, const char * restrict format, ...) {
    va_list ap;
    char last_char = 0;
    char * s;
    int d;
    size_t str_idx = 0;

    va_start(ap, format);

    while (*format) {
        printf("format: %c\n", *format);
        if(last_char == '%') {
            switch(*format) {
                case 'd':
                    d = va_arg(ap, int);
                    printf("int arg: %d\n", d);
                    write_signed_int(str, size, &str_idx, d);
                    break;
                case 's':
                    s = va_arg(ap, char*);
                    printf("string arg: %s\n", s);
                    write_string(str, size, &str_idx, s);
                    break;
                default:
                    break;
            }
        } else if (*format == '%') {
            /* do nothing, record the last character, advance the pointer */
        } else {
            if (write_char(str, size, &str_idx, *format) < 0) {
                return -1;
            }
        }
        last_char = *format;
        format++;
    }

    str[str_idx] = 0;
    printf("str_idx: %lu, str: %s\nstr_chars: ", str_idx, str);
    for (size_t i = 0; i < str_idx; i++) {
        printf("%d ", str[i]);
    }
    printf("\n");
    return str_idx;
}
