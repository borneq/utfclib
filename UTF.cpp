#include <stdint.h>
#include "UTF.h"

int UTF::one8len(char *text)
{
	uint8_t b0 = *text;
	if ((b0 & 0x80) == 0)
		return 1;
	else if ((b0 & 0x20) == 0)
		return 2;
	else if ((b0 & 0x10) == 0)
		return 3;
	else if ((b0 & 0x08) == 0)
		return 4;
	else
		return 0;
}

int UTF::one16len(wchar_t *text)
{
	uint16_t surrogate1 = (uint16_t)*text;
	if (surrogate1 >= 0xD800 && surrogate1 <= 0xDBFF)
		return 2;
	else
		return 1;
}

int UTF::one8to32(char *text, char **end)
{
	*end = text;
	uint8_t b0 = **end;
	(*end)++;
	if ((b0 & 0x80) == 0)
		return b0;
	else if ((b0 & 0x20) == 0) {
		uint8_t b1 = **end;
		(*end)++;
		return (b1 & 63) | ((int)(b0 & 31) << 6);
	}
	else if ((b0 & 0x10) == 0) {
		uint8_t b1 = **end;
		(*end)++;
		uint8_t b2 = **end;
		(*end)++;
		return (b2 & 63) | ((int)(b1 & 63) << 6) | ((int)(b0 & 15) << 12);
	}
	else {
		uint8_t b1 = **end;
		(*end)++;
		uint8_t b2 = **end;
		(*end)++;
		uint8_t b3 = **end;
		(*end)++;
		return (b3 & 63) | ((int)(b2 & 63) << 6) | ((int)(b1 & 63) << 12) | ((int)(b0 & 7) << 18);
	}
}

int UTF::one32to16(int d, wchar_t *buf)
{
	if (d < 0x10000)
	{
		buf[0] = (wchar_t)d;
		return 1;
	}
	else
	{
		buf[0] = (wchar_t)((d - 0x10000) / 0x400 + 0xD800);
		buf[1] = (wchar_t)((d - 0x10000) % 0x400 + 0xDC00);
		return 2;
	}
}

int UTF::one16to32(wchar_t *text, wchar_t **end)
{
	*end = text;
	uint16_t surrogate1 = (uint16_t)**end;
	(*end)++;
	if (surrogate1 >= 0xD800 && surrogate1 <= 0xDBFF)
	{
		uint16_t surrogate2 = (uint16_t)**end;
		(*end)++;
		return 0x400*((int)surrogate1 - 0xD800) + ((int)surrogate2 - 0xDC00) + 0x10000;
	}
	else
		return surrogate1;
}

int UTF::one32to8(int d, char *buf)
{
	if (d <= 0x7f)
	{
		buf[0] = (char)d;
		return 1;
	}
	else if (d <= 0x7ff)
	{
		buf[0] = 0xc0 | (d >> 6);
		buf[1] = 0x80 | (d & 0x3f);
		return 2;
	}
	else if (d <= 0xffff)
	{
		buf[0] = 0xe0 | (d >> 12);
		buf[1] = 0x80 | ((d >> 6) & 0x3f);
		buf[2] = 0x80 | (d & 0x3f);
		return 3;
	}
	else
	{
		buf[0] = 0xf0 | (d >> 18);
		buf[1] = 0x80 | ((d >> 12) & 0x3f);
		buf[2] = 0x80 | ((d >> 6) & 0x3f);
		buf[3] = 0x80 | (d & 0x3f);
		return 4;		
	}
}

void UTF::utf8To16(char *str, wchar_t *wstr)
{
	char *s = str;
	wchar_t *ws = wstr;
	while (*s != 0)
	{
		int d = one8to32(s, &s);
		ws += one32to16(d, ws);
	}	
	*ws = 0;
}


void UTF::utf16To8(wchar_t *wstr, char *str)
{
	wchar_t *ws = wstr;
	char *s = str;
	while (*ws != 0)
	{
		int d = one16to32(ws, &ws);
		s += one32to8(d, s);
	}
	*s = 0;
}