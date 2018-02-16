#include "UtfDetector.h"
#include <algorithm>
#include <vector>
#include <assert.h>

using namespace std;

UtfDetector::BomType UtfDetector::detectBom(char* beginDoc, intptr_t sizeDoc)
{
	if (sizeDoc>=3 || sizeDoc<0)
	{
		bool found = true;
		for (int i=0;i<3; i++)
			if (Utf8Bom[i]!=beginDoc[i])
			{
				found = false;
				break;
			}
		if (found) return BomUtf8;
	}

	if (sizeDoc >= 2 || sizeDoc<0)
	{
		bool found = true;
		for (int i = 0; i<2; i++)
			if (Utf16LEBom[i] != beginDoc[i])
			{
				found = false;
				break;
			}
		if (found) return BomUtf16LE;
	}

	if (sizeDoc >= 2 || sizeDoc<0)
	{
		bool found = true;
		for (int i = 0; i<2; i++)
			if (Utf16BEBom[i] != beginDoc[i])
			{
				found = false;
				break;
			}
		if (found) return BomUtf16BE;
	}

	if (sizeDoc >= 4 || sizeDoc<0)
	{
		bool found = true;
		for (int i = 0; i<4; i++)
			if (Utf32LEBom[i] != beginDoc[i])
			{
				found = false;
				break;
			}
		if (found) return BomUtf32LE;
	}

	if (sizeDoc >= 4 || sizeDoc<0)
	{
		bool found = true;
		for (int i = 0; i<4; i++)
			if (Utf32BEBom[i] != beginDoc[i])
			{
				found = false;
				break;
			}
		if (found) return BomUtf32BE;
	}
	return NoBom;
}

bool UtfDetector::allInRange(int from, int to, int cf[])
{
	for (int i=0; i<4; i++)
		if (cf[i]<from || cf[i]>to) return false;
	return true;
}

void UtfDetector::fillHistograms(char* beginDoc, intptr_t checkSize)
{
	memset(histograms, 0, sizeof(histograms));
	checkSize = (checkSize/4)*4;
	//checkSize must be aligned to 4 bytes;
	for (char*s = beginDoc; s<beginDoc+checkSize; s+=4)
	{
		for (int j=0; j<4; j++)
			histograms[j][uint8_t(*(s+j))]++;
	}
}

void UtfDetector::computeCountFreq()
{
	for (int j = 0; j<4; j++)
	{
		vector<HistPair> sorted;
		for (int i = 0; i<256; i++)
			if (histograms[j][i])
				sorted.push_back(HistPair{ i,histograms[j][i]});
		sort(sorted.begin(), sorted.end());
		int sumAll = 0;
		for (int i = 0; i<sorted.size(); i++)
			sumAll += sorted[i].count;
		int sum = 0;
		int median = 0;
		for (int i = 0; i<sorted.size(); i++)
		{
			sum += sorted[i].count;
			if (sum>sumAll*levelMedian)
			{
				median = sorted[i].count;
				break;
			}
		}
		double lambda;
		lambda = median>0 ? (double)sorted[0].count / median : 1;
		double thr = median / lambda;

		countFreq[j] = 0;
		for (int i = 0; i<sorted.size(); i++)
		{
			if (sorted[i].count >= thr) countFreq[j]++;
		}
	}
}

UtfDetector::UtfType UtfDetector::findMaxPoints()
{
	UtfType result = UtfType(0);
	double maxratio = 0;
	for (int i = 0; i <= Utf32BEhi; i++)
	{
		double ratio = (double)points[i] / pointsMax[i];
		if (ratio > maxratio)
		{
			maxratio = ratio;
			result = UtfType(i);
		}
	}
	return result;
}

void UtfDetector::countRandom(int *points, bool b)
{
	if (b || allInRange(180, 256, countFreq))
		points[Random] += 2;
}

void UtfDetector::countBinary(int *points, bool b)
{
	if (b || allInRange(40, 256, countFreq))
		points[Binary] += 1;

	if (b || allInRange(40, 179, countFreq))
		points[Binary] += 1;
}

void UtfDetector::countLocal(int *points, bool b)
{
	if (b || allInRange(16, 45, countFreq))
		points[Locale] += 2;
}

void UtfDetector::countUtf16LELo(int *points, bool b)
{
	if (b || countFreq[3] == 1)
		points[Utf16LELo] += 1;
	if (b || countFreq[3] < 4)
		points[Utf16LELo] += 1;
	if (b || countFreq[0] == countFreq[2])
		points[Utf16LELo] += 2;
	if (b || countFreq[1] == countFreq[3])
		points[Utf16LELo] += 2;
}
void UtfDetector::countUtf16BELo(int *points, bool b)
{
	if (b || countFreq[0] == 1)
		points[Utf16BELo] += 1;
	if (b || countFreq[2] < 4)
		points[Utf16BELo] += 1;
	if (b || countFreq[0] == countFreq[2])
		points[Utf16BELo] += 2;
	if (b || countFreq[1] == countFreq[3])
		points[Utf16BELo] += 2;
}
void UtfDetector::countUtf16LEHi(int *points, bool b)
{
	if (b || countFreq[3] == 1)
		points[Utf16LEHi] += 2;
	if (b || countFreq[2] == 1)
		points[Utf16LEHi] += 2;
	if (b || countFreq[2] < 4)
		points[Utf16LEHi] += 1;
	if (b || countFreq[3] < 4)
		points[Utf16LEHi] += 1;
	if (b || countFreq[0] == countFreq[2])
		points[Utf16LEHi] += 2;
	if (b || countFreq[1] == countFreq[3])
		points[Utf16LEHi] += 2;
}
void UtfDetector::countUtf16BEHi(int *points, bool b)
{
	if (b || countFreq[0] == 1)
		points[Utf16BEHi] += 2;
	if (b || countFreq[1] == 1)
		points[Utf16BEHi] += 2;
	if (b || countFreq[1] < 4)
		points[Utf16BEHi] += 1;
	if (b || countFreq[2] < 4)
		points[Utf16BEHi] += 1;
	if (b || countFreq[0] == countFreq[2])
		points[Utf16BEHi] += 2;
	if (b || countFreq[1] == countFreq[3])
		points[Utf16BEHi] += 2;
}
void UtfDetector::countUtf32LElo(int *points, bool b)
{
	if (b || countFreq[3] == 1)
		points[Utf32LElo] += 2;
	if (b || countFreq[2] == 1)
		points[Utf32LElo] += 2;
	if (b || countFreq[3] < 4)
		points[Utf32LElo] += 1;
	if (b || countFreq[1] < 4)
		points[Utf32LElo] += 1;
}
void UtfDetector::countUtf32LEhi(int *points, bool b)
{
	if (b || countFreq[3] == 1)
		points[Utf32LEhi] += 2;
	if (b || countFreq[2] == 1)
		points[Utf32LEhi] += 1;
	if (b || countFreq[2] < 4)
		points[Utf32LEhi] += 1;
	if (b || countFreq[1] == 16)
		points[Utf32LEhi] += 2;
}
void UtfDetector::countUtf32BElo(int *points, bool b)
{
	if (b || countFreq[0] == 1)
		points[Utf32BElo] += 2;
	if (b || countFreq[1] == 1)
		points[Utf32BElo] += 2;
	if (b || countFreq[2] < 4)
		points[Utf32BElo] += 1;
	if (b || countFreq[2] < 4)
		points[Utf32BElo] += 1;
}
void UtfDetector::countUtf32BEhi(int *points, bool b)
{
	if (b || countFreq[0] == 1)
		points[Utf32BEhi] += 2;
	if (b || countFreq[1] == 1)
		points[Utf32BEhi] += 1;
	if (b || countFreq[1] < 4)
		points[Utf32BEhi] += 1;
	if (b || countFreq[2] == 16)
		points[Utf32BEhi] += 2;
}

void UtfDetector::setPoints(int *points, bool b)
{
	memset(points, 0, sizeof(int)*(Utf32BEhi + 1));
	countRandom(points, b);
	countBinary(points, b);
	countLocal(points, b);
	countUtf16LELo(points, b);
	countUtf16BELo(points, b);
	countUtf16LEHi(points, b);
	countUtf16BEHi(points, b);
	countUtf32LElo(points, b);
	countUtf32LEhi(points, b);
	countUtf32BElo(points, b);
	countUtf32BEhi(points, b);
}

int UtfDetector::countNonAScii()
{
	int result = 0;
	for (int i=128; i<256; i++)
		for (int j=0; j<4; j++)
		result += histograms[j][i];
	return result;
}

int UtfDetector::countZeros()
{
	int result = 0;
	for (int j = 0; j<4; j++)
			result += histograms[j][0];
	return result;
}

int UtfDetector::one8len(char *text)
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

int UtfDetector::one8test(char *text, char **end)
{
	*end = text;
	uint8_t b0 = **end;
	(*end)++;
	if ((b0 & 0x80) == 0)
		return b0;
	else if ((b0 & 0x20) == 0) {
		uint8_t b1 = **end;
		(*end)++;
		if ((b1 & 0b11000000) != 128)
			return 1;
		else
			return 0;
	}
	else if ((b0 & 0x10) == 0) {
		uint8_t b1 = **end;
		(*end)++;
		uint8_t b2 = **end;
		(*end)++;
		if ((b1 & 0b11000000) != 128 || (b2 & 0b11000000) != 128)
			return 1;
		else
			return 0;
	}
	else if ((b0 & 0x08) == 0) {
		uint8_t b1 = **end;
		(*end)++;
		uint8_t b2 = **end;
		(*end)++;
		uint8_t b3 = **end;
		(*end)++;
		if ((b1 & 0b11000000) != 128 || (b2 & 0b11000000) != 128 || (b3 & 0b11000000) != 128)
			return 1;
		else
			return 0;
	}
	else return 1;
}

bool UtfDetector::isUtf8hi(char *str, size_t len)
{
	int hiSymbols = 0, loSymbols = 0;
	char *s = str;
	char *sentinel = str + len;
	while (s < sentinel)
	{
		if (*s > 0)
		{
			s++;
			continue;
		}
		int len = one8len(s);
		if (len == 0)
		{
			s++;
			continue;
		}
		if (s + len>sentinel)
		{
			break;
		}
		if (len>1)
		{
			if (len >= 4)
				hiSymbols++;
			else
				loSymbols++;
		}
		s += len;
	}
	//high symbols must be counting as two low symbols
	return hiSymbols*2 >= loSymbols;
}

int UtfDetector::testUtf8(char *str, size_t len)
{
	int errors=0;
	char *s = str;
	char *sentinel = str + len;
	while (s < sentinel)
	{
		if (*s > 0)
		{
			s++;
			continue;
		}
		int len = one8len(s);
		if (len == 0)
		{
			s++;
			errors++;
			continue;
		}
		if (s + len>sentinel)
		{
			errors++;
			break;
		}
		errors += one8test(s, &s);
	}
	return errors;
}

UtfDetector::UtfType UtfDetector::detectUTf(char* beginDoc, intptr_t sizeDoc)
{
	const int MaxCheck = 4096;
	int checkSize = (int)min(sizeDoc, (intptr_t)MaxCheck);
	fillHistograms(beginDoc, checkSize);
	int zeros = countZeros();
	computeCountFreq();
	setPoints(pointsMax, true);
	setPoints(points, false);
	UtfType found = findMaxPoints();

	if (found==Locale || found == Binary)
	{
		if (zeros > levelAcsiiZero*checkSize)
			return Binary;
		int nonAscii = countNonAScii();
		if (nonAscii==0)
			return Ascii;
		int errors = testUtf8(beginDoc, checkSize);
		assert(nonAscii>0);
		if (errors <= levelUtf8Errors*nonAscii)
		{
			if (isUtf8hi(beginDoc, checkSize))
				return Utf8hi;
			else
				return Utf8lo;
		}
		else return Locale;
	}
	else return found;
}
