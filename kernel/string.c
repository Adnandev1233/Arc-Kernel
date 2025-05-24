#include "include/string.h"

void* memset(void* dest, int val, size_t count) {
    unsigned char* ptr = (unsigned char*)dest;
    while (count-- > 0) {
        *ptr++ = (unsigned char)val;
    }
    return dest;
}

void* memcpy(void* dest, const void* src, size_t count) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (count-- > 0) {
        *d++ = *s++;
    }
    return dest;
}

int memcmp(const void* ptr1, const void* ptr2, size_t count) {
    const unsigned char* p1 = (const unsigned char*)ptr1;
    const unsigned char* p2 = (const unsigned char*)ptr2;
    while (count-- > 0) {
        if (*p1++ != *p2++) {
            return p1[-1] < p2[-1] ? -1 : 1;
        }
    }
    return 0;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t count) {
    char* d = dest;
    while (count-- > 0) {
        if ((*d++ = *src++) == '\0') {
            while (count-- > 0) {
                *d++ = '\0';
            }
            break;
        }
    }
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char* strncat(char* dest, const char* src, size_t count) {
    char* d = dest;
    while (*d) d++;
    while (count-- > 0) {
        if ((*d++ = *src++) == '\0') {
            return dest;
        }
    }
    *d = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int strncmp(const char* str1, const char* str2, size_t count) {
    while (count-- > 0) {
        if (*str1 != *str2) {
            return *(unsigned char*)str1 - *(unsigned char*)str2;
        }
        if (*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
    return 0;
}

size_t strlen(const char* str) {
    const char* s = str;
    while (*s) s++;
    return s - str;
}

char* strchr(const char* str, int ch) {
    while (*str && *str != (char)ch) str++;
    return (*str == (char)ch) ? (char*)str : NULL;
}

char* strrchr(const char* str, int ch) {
    const char* last = NULL;
    while (*str) {
        if (*str == (char)ch) {
            last = str;
        }
        str++;
    }
    return (char*)last;
}

char* strstr(const char* str1, const char* str2) {
    if (!*str2) return (char*)str1;
    while (*str1) {
        const char* s1 = str1;
        const char* s2 = str2;
        while (*s1 && *s2 && *s1 == *s2) {
            s1++;
            s2++;
        }
        if (!*s2) return (char*)str1;
        str1++;
    }
    return NULL;
} 