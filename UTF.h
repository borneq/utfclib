#pragma once
class UTF
{
public:
	int one8len(char *text);
	int one16len(wchar_t *text);
	static int one8to32(char *text, char **end);
	static int one32to16(int d, wchar_t *buf);
	static int one16to32(wchar_t *text, wchar_t **end);
	static int one32to8(int d, char *buf);
	static void utf8To16_z(char *str, wchar_t *wstr);
	static void utf16To8_z(wchar_t *wstr, char *str);
};

