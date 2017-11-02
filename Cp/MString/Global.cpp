#include <stdlib.h>

size_t StrLen(const char * str)
{
	size_t len = 0;
	while (str[len] != '\0') len++;
	return len;
}

char * StrCpy(char * dst, size_t size, const char * sou)
{
	int n;
	for (n = 0; n < (int)size - 1; n++) {
		if (sou[n] == '\0') break;
		dst[n] = sou[n];
	}
	dst[n] = '\0';
	return dst;
}

char * StrCat(char * dst, size_t size, const char * sou)
{
	int n = 0;
	while (dst[n] != '\0') n++;
	for ( ; n < (int)size - 1; n++) {
		if (*sou == '\0') break;
		dst[n] = *sou++;
	}
	dst[n] = '\0';
	return dst;
}

int StrCmp(const char * str1, const char * str2)
{
	while (*str1 != '\0' || *str2 != '\0') {
		if (*str1 != *str2) break;
		str1++, str2++;
	}
	return *str1 - *str2;
}

