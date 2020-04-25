#pragma once
#include <lib/type.h>

void memset(void *dst, int c, u64 len);

void memcpy(void *dst, void *src, u64 len);

void strcpy(char *dst, const char *src);

u32 strcmp(const char *s1, const char *s2);

u32 strncmp(const char *s1, const char *s2, size_t size);

