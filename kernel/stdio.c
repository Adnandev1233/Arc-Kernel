#include "include/stdio.h"
#include "include/string.h"

// Helper function to convert number to string
static void number_to_string(char* str, int num, int base) {
    int i = 0;
    int is_negative = 0;
    
    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    
    // Handle zero
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    
    // Convert number to string
    while (num > 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
    
    // Add negative sign if needed
    if (is_negative) {
        str[i++] = '-';
    }
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
    
    str[i] = '\0';
}

// Minimal sprintf implementation
int sprintf(char* str, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(str, format, args);
    va_end(args);
    return result;
}

// Minimal vsprintf implementation
int vsprintf(char* str, const char* format, va_list args) {
    int i = 0;
    int j = 0;
    
    while (format[i] != '\0') {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int num = va_arg(args, int);
                    char num_str[32];
                    number_to_string(num_str, num, 10);
                    int k = 0;
                    while (num_str[k] != '\0') {
                        str[j++] = num_str[k++];
                    }
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    int k = 0;
                    while (s[k] != '\0') {
                        str[j++] = s[k++];
                    }
                    break;
                }
                case '%': {
                    str[j++] = '%';
                    break;
                }
                default:
                    str[j++] = format[i];
                    break;
            }
        } else {
            str[j++] = format[i];
        }
        i++;
    }
    
    str[j] = '\0';
    return j;
}

// Minimal snprintf implementation
int snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(str, size, format, args);
    va_end(args);
    return result;
}

// Minimal vsnprintf implementation
int vsnprintf(char* str, size_t size, const char* format, va_list args) {
    if (size == 0) return 0;
    
    size_t i = 0;
    size_t j = 0;
    
    while (format[i] != '\0' && j < size - 1) {
        if (format[i] == '%') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int num = va_arg(args, int);
                    char num_str[32];
                    number_to_string(num_str, num, 10);
                    size_t k = 0;
                    while (num_str[k] != '\0' && j < size - 1) {
                        str[j++] = num_str[k++];
                    }
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    size_t k = 0;
                    while (s[k] != '\0' && j < size - 1) {
                        str[j++] = s[k++];
                    }
                    break;
                }
                case '%': {
                    str[j++] = '%';
                    break;
                }
                default:
                    str[j++] = format[i];
                    break;
            }
        } else {
            str[j++] = format[i];
        }
        i++;
    }
    
    str[j] = '\0';
    return j;
} 