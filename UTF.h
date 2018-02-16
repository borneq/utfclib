#pragma once
#include <stdint.h>

class UTF
{
public:
	size_t errors = 0;
	int one8len(char *text);
	int one16len(wchar_t *text);
	int one8to32(char *text, char **end);
	int one32to16(int d, wchar_t *buf);
	int one16to32(wchar_t *text, wchar_t **end);
	int one32to8(int d, char *buf);
	size_t utf8To32(char *str, uint32_t *dstr, size_t len);
	void utf8To16_z(char *str, wchar_t *wstr);
	void utf16To8_z(wchar_t *wstr, char *str);
};

