#include <lib/type.h>

/*
 * A very shabby implementation, optimize it if you like.
 */

void memset(void *dst, int c, u64 len)
{
	u64 i = 0;
	for (; i < len; i += 1) {
		((u8 *)dst)[i] = (u8)c;
	}
}

void memcpy(void *dst, void *src, u64 len)
{
	u64 i = 0;
	for (; i < len; i += 1) {
		((u8 *)dst)[i] = ((u8 *)src)[i];
	}
}

void strcpy(char *dst, const char *src)
{
	u64 i = 0;
	while (src[i] != 0) {
		dst[i] = src[i];
		i += 1;
	}
}

u32
strcmp(const char *p, const char *q)
{
	while (*p && *p == *q) {
		p += 1;
		q += 1;
	}
	return (u32) ((u8) *p - (u8) *q);
}


u32
strncmp(const char *p, const char *q, size_t n)
{
	while (n > 0 && *p && *p == *q) {
		n -= 1;
		p += 1;
		q += 1;
	}
	if (n == 0)
		return 0;
	else
		return (u32) ((u8) *p - (u8) *q);
}
