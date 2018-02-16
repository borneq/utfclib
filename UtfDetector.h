#pragma once
#include <stdint.h>

class UtfDetector
{
public:
	enum BomType { NoBom, BomUtf8, BomUtf16LE, BomUtf16BE, BomUtf32LE, BomUtf32BE };
	enum UtfType { Random, Binary, Locale, Utf16LELo, Utf16BELo, Utf16LEHi, Utf16BEHi, Utf32LElo, Utf32LEhi, Utf32BElo, Utf32BEhi, Ascii, Utf8lo, Utf8hi };
private:
	const double levelAcsiiHi = 0.5 / 100;
	const double levelAcsiiZero = 0.1 / 100;
	const double levelUtf8Errors = 0.04;
	const double levelMedian = 1-(1/2.71828);
	char *UtfTypeNames[Utf8hi + 1] = { "Random", "Binary", "Local", "Utf16LELo", "Utf16BELo", "Utf16LEHi", "Utf16BEHi", "Utf32LElo", "Utf32LEhi", "Utf32BElo", "Utf32BEhi" ,"Ascii", "Utf8lo", "Utf8hi", };
	const uint8_t Utf8Bom[3] { 0xEF, 0xBB, 0xBF};
	const uint8_t Utf16LEBom[2]{ 0xFF, 0xFE };
	const uint8_t Utf16BEBom[2]{ 0xFE, 0xFF };
	const uint8_t Utf32LEBom[4]{ 0xFF, 0xFE, 0x00, 0x00 };
	const uint8_t Utf32BEBom[4]{ 0x00, 0x00, 0xFE, 0xFF };
	int histograms[4][256];
	int countFreq[4];
	int pointsMax[Utf32BEhi + 1];
	int points[Utf32BEhi + 1];
	void fillHistograms(char* beginDoc, intptr_t checkSize);
	void computeCountFreq();
	void countRandom(int *points, bool b);
	void countBinary(int *points, bool b);
	void countLocal(int *points, bool b);
	void countUtf16LELo(int *points, bool b);
	void countUtf16BELo(int *points, bool b);
	void countUtf16LEHi(int *points, bool b);
	void countUtf16BEHi(int *points, bool b);
	void countUtf32LElo(int *points, bool b);
	void countUtf32LEhi(int *points, bool b);
	void countUtf32BElo(int *points, bool b);
	void countUtf32BEhi(int *points, bool b);
	void setPoints(int *points, bool b);
	UtfType findMaxPoints();
	int one8len(char *text);
	int one8test(char *text, char **end);
	int testUtf8(char *str, size_t len);
	bool isUtf8hi(char *str, size_t len);
	int countNonAScii();
	int countZeros();
	struct HistPair
	{
		int pos;
		int count;

		bool operator<(const HistPair& rhs) const
		{
			if (count == rhs.count)
				return pos < rhs.pos;
			else
				return count > rhs.count;
		}
	};
	static bool allInRange(int from, int to, int cf[]);
public:
	UtfDetector() {};
	BomType detectBom(char* beginDoc, intptr_t sizeDoc);
	UtfType detectUTf(char* beginDoc, intptr_t sizeDoc);
	const char* name(UtfType type)
	{
		return UtfTypeNames[int(type)];
	}
};

