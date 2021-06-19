#pragma once

#include <string>

class Base64Util final
{
private:
    Base64Util() = delete;
    ~Base64Util() = delete;
    Base64Util(const Base64Util& rhs) = delete;
    Base64Util& operator=(const Base64Util& rhs) = delete;

public:
    static int encode(char* pDest, const char* pSource, int lenSource, char chMask, int maxDest);
    static int decode(char* pDest, const char* pSource, int lenSource, char chMask, int maxDest);
    static void decode(const char* pData, int nDataByte, std::string& strDecode);

    static bool check(char* lpString);
};
