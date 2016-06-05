#pragma once
class UTF
{
public:
	static int one8to32(char *text, char **end);
	static int one32to16(int d, wchar_t *buf);
	static int one16to32(wchar_t *text, wchar_t **end);
	static int one32to8(int d, char *buf);
	static void utf8To16(char *str, wchar_t *wstr);
	static void utf16To8(wchar_t *wstr, char *str);
};

