#include "UTF.h"

void test32_16()
{
	for (int d = 0; d < 17 * 256 * 256; d++)
	{
		if (d >= 0xD800 && d <= 0xDfff) continue;
		wchar_t buf[64];
		UTF::one32to16(d, buf);
		wchar_t *end;
		int d1 = UTF::one16to32(buf, &end);
		if (d != d1)
			throw "";
	}
}

void test32_8()
{
	for (int d = 0; d < 17 * 256 * 256; d++)
	{
		char buf[64];
		UTF::one32to8(d, buf);
		char *end;
		int d1 = UTF::one8to32(buf, &end);
		if (d != d1)
			throw "";
	}
}

void test1()
{
	char buf[64];
	UTF::utf16To8(L"b\405k", buf);
}

void test2()
{
	wchar_t buf[64];
	UTF::utf8To16("b\304\205k", buf);
}

