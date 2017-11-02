#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

size_t StrLen(const char * str);
char * StrCpy(char * dst, size_t size, const char * sou);
char * StrCat(char * dst, size_t size, const char * sou);
int StrCmp(const char * str1, const char * str2);

// 템플릿은 소스파일(.CPP)이 아닌 헤더파일(.H)에 정의하는 이유는 
// 컴파일시 호출시점에서 코드를 만들기(instance化) 위해 헤더파일에서 먼저 읽혀야 하기때문입니다.

template <size_t size> // 비타입 인수를 참조형 배열 크기에 사용(호출 시 배열의 크기 자동 전달 )
char * StrCpy(char(& dst)[size], const char * sou)
{
	int n;
	for (n = 0; n < (int)size - 1; n++) {
		if (sou[n] == '\0') break;
		dst[n] = sou[n];
	}
	dst[n] = '\0';
	return dst;
}

template<size_t size>
char * StrCat(char (& dst)[size], const char * sou)
{
	int n = 0;
	while (dst[n] != '\0') n++;
	for (; n < (int)size - 1; n++) {
		if (*sou == '\0') break;
		dst[n] = *sou++;
	}
	dst[n] = '\0';
	return dst;
}

#endif // _GLOBAL_H_
