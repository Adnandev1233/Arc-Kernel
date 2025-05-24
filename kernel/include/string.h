#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// String manipulation
void* memset(void* dest, int val, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
int memcmp(const void* ptr1, const void* ptr2, size_t count);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t count);
char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t count);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t count);
size_t strlen(const char* str);
char* strchr(const char* str, int ch);
char* strrchr(const char* str, int ch);
char* strstr(const char* str1, const char* str2);

#endif // STRING_H 