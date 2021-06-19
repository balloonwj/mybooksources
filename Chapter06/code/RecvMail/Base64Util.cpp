#include "Base64Util.h"
/////////////////////////////////////////////////////////////////////////////////////////////////
//解码表
static const char DECODE_TABLE[] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	62,													// '+'
	0, 0, 0,
	63,													// '/'
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61,				// '0'-'9'
	0, 0, 0, 0, 0, 0, 0,
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
	0, 0, 0, 0, 0, 0,
	26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
	39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
};

static const char ENCODE_TABLE[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" };

int Base64Util::encode(char* pDest, const char* pSource, int lenSource, char chMask, int maxDest)
{
	char c1, c2, c3;
	int i = 0, lenDest(0), lDiv(lenSource / 3), lMod(lenSource % 3);
	for (; i < lDiv; ++i, lenDest += 4)
	{
		if (lenDest + 4 >= maxDest)
			return 0;
		c1 = *pSource++;
		c2 = *pSource++;
		c3 = *pSource++;
		*pDest++ = ENCODE_TABLE[c1 >> 2];
		*pDest++ = ENCODE_TABLE[((c1 << 4) | (c2 >> 4)) & 0X3F];
		*pDest++ = ENCODE_TABLE[((c2 << 2) | (c3 >> 6)) & 0X3F];
		*pDest++ = ENCODE_TABLE[c3 & 0X3F];
	}
	if (lMod == 1)
	{
		if (lenDest + 4 >= maxDest) return(0);
		c1 = *pSource++;
		*pDest++ = ENCODE_TABLE[(c1 & 0XFC) >> 2];
		*pDest++ = ENCODE_TABLE[((c1 & 0X03) << 4)];
		*pDest++ = chMask;
		*pDest++ = chMask;
		lenDest += 4;
	}
	else if (lMod == 2)
	{
		if (lenDest + 4 >= maxDest) return(0);
		c1 = *pSource++;
		c2 = *pSource++;
		*pDest++ = ENCODE_TABLE[(c1 & 0XFC) >> 2];
		*pDest++ = ENCODE_TABLE[((c1 & 0X03) << 4) | ((c2 & 0XF0) >> 4)];
		*pDest++ = ENCODE_TABLE[((c2 & 0X0F) << 2)];
		*pDest++ = chMask;
		lenDest += 4;
	}

	*pDest = 0;

	return lenDest;
}

int Base64Util::decode(char* pDest, const char* pSource, int lenSource, char chMask, int maxDest)
{
	int lenDest = 0, nValue = 0, i = 0;
	for (; i < lenSource; i += 4)
	{
		nValue = DECODE_TABLE[(int)(*pSource)] << 18;
		pSource++;
		nValue += DECODE_TABLE[(int)*pSource] << 12;
		pSource++;
		if (++lenDest >= maxDest)
			break;

		*pDest++ = char((nValue & 0X00FF0000) >> 16);

		if (*pSource != chMask)
		{
			nValue += DECODE_TABLE[(int)*pSource] << 6;
			pSource++;
			if (++lenDest >= maxDest)
				break;
			*pDest++ = (nValue & 0X0000FF00) >> 8;

			if (*pSource != chMask)
			{
				nValue += DECODE_TABLE[(int)*pSource];
				pSource++;
				if (++lenDest >= maxDest)
					break;
				*pDest++ = nValue & 0X000000FF;
			}
		}
	}

	*pDest = 0;

	return lenDest;
}

void Base64Util::decode(const char* pData, int nDataByte, std::string& strDecode)
{
	strDecode.clear();
	int nValue = 0, i = 0, OutByte = 0;
	while (i < nDataByte)
	{
		if (*pData != '\r' && *pData != '\n')
		{
			nValue = DECODE_TABLE[*pData++] << 18;
			nValue += DECODE_TABLE[*pData++] << 12;
			strDecode += (char)((nValue & 0x00FF0000) >> 16);
			OutByte++;
			if (*pData != '=')
			{
				nValue += DECODE_TABLE[*pData++] << 6;
				strDecode += (char)((nValue & 0x0000FF00) >> 8);
				OutByte++;
				if (*pData != '=')
				{
					nValue += DECODE_TABLE[*pData++];
					strDecode += (char)(nValue & 0x000000FF);
					OutByte++;
				}
			}
			i += 4;
		}
		else	// 回车换行,跳过
		{
			pData++;
			i++;
		}
	}
}

bool Base64Util::check(char* lpString)
{
	for (; *lpString; ++lpString)
	{
		switch (*lpString)
		{
		case	'+': *lpString = '@';	break;
		case	'@': *lpString = '+';	break;
		case	'=': *lpString = '$';	break;
		case	'$': *lpString = '=';	break;
		case	'/': *lpString = '#';	break;
		case	'#': *lpString = '/';	break;
		default:
			if (*lpString >= 'A' && *lpString <= 'Z')
				*lpString = *lpString - 'A' + 'a';
			else if (*lpString >= 'a' && *lpString <= 'z')
				*lpString = *lpString - 'a' + 'A';
			else if (*lpString >= '0' && *lpString <= '4')
				*lpString = *lpString - '0' + '5';
			else if (*lpString >= '5' && *lpString <= '9')
				*lpString = *lpString - '5' + '0';
			else
				return false;
		}
	}
	return true;
}
