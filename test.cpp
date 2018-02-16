#include "UTF.h"

void test32_16()
{
	UTF utf;
	for (int d = 0; d < 17 * 256 * 256; d++)
	{
		if (d >= 0xD800 && d <= 0xDfff) continue;
		wchar_t buf[64];
		utf.one32to16(d, buf);
		wchar_t *end;
		int d1 = utf.one16to32(buf, &end);
		if (d != d1)
			throw "";
	}
}

void test32_8()
{
	UTF utf;
	for (int d = 0; d < 17 * 256 * 256; d++)
	{
		char buf[64];
		utf.one32to8(d, buf);
		char *end;
		int d1 = utf.one8to32(buf, &end);
		if (d != d1)
			throw "";
	}
}

void test1()
{
	char buf[64];
	UTF utf;
	utf.utf16To8_z(L"b\405k", buf);
}

void test2()
{
	wchar_t buf[64];
	UTF utf;
	utf.utf8To16_z("b\304\205k", buf);
}

