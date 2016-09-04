#ifndef _STRING_H_
#define _STRING_H_

#include "type.h"

void* memcpy(void *dest, void *src, unsigned int count);

void* memset(void *dest, char val, unsigned int count);

short* memsetw(short *dest, short val, unsigned int count);

int strlen(char *str);

int strnlen(char *str, unsigned int size);

char* strcpy(char *dst, const char *src);

char* strncpy(char *dst, const char *src, unsigned int cnt);

char* strchr(const char *str, int c);

char* strrchr(const char *str, int c);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char* s2, unsigned int n);

#endif
