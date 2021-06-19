#include "MailHelper.h"

#include "EncodeUtil.h"
#include "Base64Util.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

bool MailHelper::m_bIsFromMobilePlatform = false;
std::string MailHelper::m_strMail;

std::vector<std::string> MailHelper::m_vecBoundary;
std::vector<std::wstring> MailHelper::m_vecText;
std::vector<std::wstring> MailHelper::m_vecHTML;
std::map<std::wstring, std::string>	MailHelper::m_vecAttachment;

wchar_t MailHelper::g_szEmailSavePath[256] = { 0 };

void MailHelper::splitString(const std::string& str, std::vector<std::string>& v, const char* delimiter/* = "|"*/)
{
    if (delimiter == NULL || str.empty())
        return;

    std::string buf(str);
    size_t pos = std::string::npos;
    std::string substr;
    int delimiterlength = strlen(delimiter);
    while (true)
    {
        pos = buf.find(delimiter);
        if (pos != std::string::npos)
        {
            substr = buf.substr(0, pos);
            if (!substr.empty())
                v.push_back(substr);

            buf = buf.substr(pos + delimiterlength);
        }
        else
        {
            if (!buf.empty())
                v.push_back(buf);
            break;
        }
    }
}

int MailHelper::analyzeTags(const std::vector<std::string>& vecString, std::map<std::string, std::string>& mapString)
{
    mapString.clear();
    size_t nCount = vecString.size();
    std::string strTemp, strContent, strTag;
    for (size_t i = 0, j = 0; i < nCount; ++i)
    {
        const std::string& strItem = vecString[i];
        if (strItem.empty())
            continue;

        if (strItem[0] == 0x20 || strItem[1] == 0x09)
        {
            if (!strTag.empty())
                strContent += strItem;

            continue;
        }

        if (!strTag.empty())
        {
            mapString.insert(std::pair<std::string, std::string>(strTag, strContent));
            strTag.clear();
        }

        strTemp = strItem;
        MailHelper::stringToUpper(strTemp);

        for (j = 0; j < sizeof(EMAIL_TAG_LIST) / sizeof(EMAIL_TAG_LIST[0]); ++j)
        {
            //以EMAIL_TAG_LIST开头
            if (strTemp.find(EMAIL_TAG_LIST[j]) == 0)
            {
                strTag = EMAIL_TAG_LIST[j];
                strContent = strItem.substr(strlen(EMAIL_TAG_LIST[j]));
            }
        }


        if (i == nCount - 1 && !strTag.empty())
        {
            mapString.insert(std::pair<std::string, std::string>(strTag, strContent));
            strTag.clear();
        }
    }

    return (long)mapString.size();
}

//将"3 Mar 2020 17:56:49"格式转换成日期结构体
bool MailHelper::parseTimeString(const std::string& dateString, TimeItem& timeItem)
{
    std::vector<std::string> vecItems;
    MailHelper::splitString(dateString, vecItems, " ");
    if (vecItems.size() != 4)
        return false;

    timeItem.day = atoi(vecItems[0].c_str());
    timeItem.year = atoi(vecItems[2].c_str());
    if (vecItems[1] == "Jan")
        timeItem.month = 1;
    else if (vecItems[1] == "Feb")
        timeItem.month = 2;
    else if (vecItems[1] == "Mar")
        timeItem.month = 3;
    else if (vecItems[1] == "Apr")
        timeItem.month = 4;
    else if (vecItems[1] == "May")
        timeItem.month = 5;
    else if (vecItems[1] == "Jun")
        timeItem.month = 6;
    else if (vecItems[1] == "Jul")
        timeItem.month = 7;
    else if (vecItems[1] == "Aug")
        timeItem.month = 8;
    else if (vecItems[1] == "Sept")
        timeItem.month = 9;
    else if (vecItems[1] == "Oct")
        timeItem.month = 10;
    else if (vecItems[1] == "Nov")
        timeItem.month = 11;
    else if (vecItems[1] == "Dec")
        timeItem.month = 12;

    std::vector<std::string> vecItems2;
    //17:56:49拆分出来时分秒
    MailHelper::splitString(vecItems[3], vecItems2, ":");
    if (vecItems2.size() != 3)
        return false;

    timeItem.hour = atoi(vecItems2[0].c_str());
    timeItem.minute = atoi(vecItems2[1].c_str());
    timeItem.second = atoi(vecItems2[2].c_str());

    return true;
}

void MailHelper::analyzeString(const std::string& strData, std::wstring& strResult)
{
    strResult.clear();
    long lOffset = strData.find("=?", 0);
    if (lOffset == std::string::npos)
    {
        strResult = EncodeUtil::s2ws(strData);
        return;
    }

    long lPhaseEnd = 0, lCharsetEnd = 0, lCodeEnd = 0;
    long lCharsetType = 0;
    std::string strPhase, strCharset, strLastCharset;
    char cCodeType = 0;
    std::string strTemp;
    std::wstring strUnicode;
    //取出=?之前的数据
    lOffset = 0;
    while ((lOffset = strData.find("=?", lOffset)) >= 0)
    {
        lOffset += 2;
        strPhase = strData.substr(lOffset);	// 处理多段情况,取出一段

        lPhaseEnd = MailHelper::findOneOf(strPhase, "\x09\x0A\x0D\x20");
        if (lPhaseEnd != std::string::npos)
            strPhase = strPhase.substr(0, lPhaseEnd);

        lPhaseEnd = strPhase.rfind('?');
        if (lPhaseEnd == std::string::npos || strPhase.rfind('=') != lPhaseEnd + 1)
            lPhaseEnd = strPhase.length() - 1;

        //取出=?与?=之间的数据
        strPhase = strPhase.substr(0, lPhaseEnd);

        //取字符集
        lCharsetEnd = strPhase.find('?', 0);
        strCharset = strPhase.substr(0, lCharsetEnd);
        lCharsetType = getCharset(strCharset.c_str());

        strPhase = strPhase.substr(lCharsetEnd + 1);

        //取编码方式
        lCodeEnd = strPhase.find('?', 0);
        cCodeType = strPhase[0];
        strPhase = strPhase.substr(lCodeEnd + 1);

        if (!strPhase.empty())
        {
            switch (cCodeType)
            {
            case 'b':
            case 'B':
                Base64Util::decode(strPhase.c_str(), strPhase.length(), strTemp);
                break;
            case 'q':
            case 'Q':
            {
                char szDest[128] = { 0 };
                EncodeUtil::DecodeQuotedPrintable(strPhase.c_str(), szDest);
                strTemp = szDest;
                break;
            }
            }

            if (lCharsetType == CHARSET_UTF8_ID)
            {
                EncodeUtil::UTF8ToUnicode(strTemp.c_str(), strUnicode);
                strResult += strUnicode;
            }
            else if (lCharsetType == CHARSET_GB2312_ID || lCharsetType == CHARSET_GBK_ID)
            {
                EncodeUtil::GB2312ToUnicode(strTemp.c_str(), strUnicode);
                strResult += strUnicode;
            }

            lOffset += (lPhaseEnd + 1);
        }
    }
}

size_t MailHelper::findOneOf(const std::string& str, const std::string& matchPatterns)
{
    size_t pos = std::string::npos;
    for (size_t i = 0; i < matchPatterns.length(); ++i)
    {
        pos = str.find(matchPatterns[i]);
        if (pos != std::string::npos)
            return pos;
    }

    return pos;
}

long MailHelper::getCharset(const char* pszData)
{
    std::string strCharset(pszData);
    MailHelper::stringToUpper(strCharset);
    if (strCharset == "UTF-8")
        return CHARSET_UTF8_ID;
    if (strCharset == "GB2312")
        return CHARSET_GB2312_ID;
    if (strCharset == "GBK")
        return CHARSET_GBK_ID;

    return CHARSET_GB2312_ID;
}

// 字符全部转换为大写
void MailHelper::stringToUpper(std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        //在ASCII表里大写字符的值比对应小写字符的值小32.
        //*p -= 0x20; // 0x20的十进制就是32
        if (str[i] >= 'a' && str[i] <= 'z')           
            str[i] -= 32;
    }
}

// 字符全部转换为小写
void MailHelper::stringToLower(std::string& str)
{
    for (size_t i = 0; i < str.length(); ++i)
    {
        //在ASCII表里大写字符的值比对应小写字符的值小32.
        //*p -= 0x20; // 0x20的十进制就是32
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] += 32;
    }
}

//修剪字符串
void MailHelper::trim(std::string& str, const char delimiter/* = ' '*/)
{
    std::string temp1;
    size_t length = str.length();
    for (size_t i = 0; i < length; ++i)
    {
        if (str[i] == delimiter)
            continue;
        else
        {
            temp1 = str.substr(i);
            break;
        }
    }

    std::string temp2;
    length = temp1.length();
    if (length > 0)
    {
        for (size_t i = length - 1; i >= 0; --i)
        {
            if (temp1[i] == delimiter)
                continue;
            else
            {
                temp2 = temp1.substr(0, i + 1);
                break;
            }
        }
    }
    
    str = temp2;
}

void MailHelper::trim(std::wstring& str, const wchar_t delimiter/* = L' '*/)
{
    std::wstring temp1;
    size_t length = str.length();
    for (size_t i = 0; i < length; ++i)
    {
        if (str[i] == delimiter)
            continue;
        else
        {
            temp1 = str.substr(i + 1);
        }
    }

    std::wstring temp2;
    length = temp1.length();
    if (length > 0)
    {
        for (size_t i = length - 1; i >= 0; --i)
        {
            if (temp1[i] == delimiter)
                continue;
            else
            {
                temp2 = temp1.substr(0, i);
            }
        }
    }

    str = temp2;
}

bool MailHelper::analyzeBody(const std::string& strBody)
{
    if (strBody.empty())
        return false;

    m_strMail = strBody;

    if (!getBoundaryTag() && !detectMobilePlatform())
        return false;

    size_t lCount = m_vecBoundary.size();
    std::string strBoundary;
    for (size_t i = 0; i < lCount; ++i)
    {
        strBoundary = "--";
        strBoundary += m_vecBoundary[i];
        analyzeBoundary(strBoundary, m_vecBoundary[i]);
    }

    for (const auto& iter : m_vecAttachment)
    {
        if (!iter.first.empty() && !iter.second.empty())
        {
            std::string strUtf8File = EncodeUtil::UnicodeToUtf8(iter.first);
            FILE* file = fopen(strUtf8File.c_str(), "wb+");
            if (file == nullptr)
                continue;

            //TODO：实际开发中由于邮件附件可能比较大，一次性未必全部写成功
            //应该在一个循环中每次发送一点直到全部发送完成
            size_t writtenLength = fwrite(iter.second.c_str(), sizeof(char), iter.second.length(), file);
            if (writtenLength != iter.second.length())
            {
                //perror("write err");
                //写文件失败
                fclose(file);
                continue;
            }

            fclose(file);
        }
    }

    

    return true;
}

bool MailHelper::getBoundaryTag()
{
    m_vecBoundary.clear();
    if (m_strMail.empty())
        return false;

    const char BOUNDARY_TAG[] = "boundary=\"";
    const long BOUNDARY_TAG_LEN = ARRAYSIZE(BOUNDARY_TAG) - 1;
    long lStart = 0, lEnd = 0;
    std::string strItem;
    while ((lStart = m_strMail.find(BOUNDARY_TAG, lStart)) != std::string::npos)
    {
        lEnd = m_strMail.find('"', lStart + BOUNDARY_TAG_LEN);
        strItem = m_strMail.substr(lStart + BOUNDARY_TAG_LEN, lEnd - lStart - BOUNDARY_TAG_LEN);
        m_vecBoundary.push_back(strItem);
        lStart = lEnd + 1;
    }

    return m_vecBoundary.size() > 0;
}

bool MailHelper::analyzeBoundary(const std::string& strString, const std::string& strBoundary)
{
    std::vector<std::string> vecString;
    splitString(m_strMail, vecString, strString.c_str());
    if (vecString.empty()) 
        return false;

    long lStart = 0, lEnd = 0;
    long lTextType = 0;
    std::wstring strContent;
    std::wstring strFileName;
    std::string strAttachment;
    std::string strTemp;
    size_t lCount = vecString.size();
    for (size_t i = 0; i < lCount; ++i)
    {
        const std::string& strItem = vecString[i];
        if (strItem.empty())
            continue;

        if (strItem.find("boundary=") != std::string::npos)
            continue;

        if (strItem.find("Content-Type") == std::string::npos) 
            continue;

        if (isExistBoundary(strItem, strBoundary.c_str()))
            continue;

        lTextType = analyzeBoundary(strItem, strContent, strAttachment, strFileName);
        if (strContent.empty() && lTextType != APPLICATION_TYPE_ID) 
            continue;

        switch (lTextType)
        {
        case PLAIN_TYPE_ID:
            m_vecText.push_back(strContent);
            break;
        case HTML_TYPE_ID:
            m_vecHTML.push_back(strContent);
            break;
        case APPLICATION_TYPE_ID:
        {
            for (int i = 0; i < 10; ++i)
            {
                if (m_vecAttachment.find(strFileName) == m_vecAttachment.end())
                {
                    m_vecAttachment.insert(std::pair<std::wstring, std::string>(strFileName, strAttachment));
                    break;
                }

                char szTemp[32] = { 0 };
                sprintf(szTemp, "(%d)", i + 1);
                strTemp = szTemp;
                std::wstring strTempW = EncodeUtil::AnsiToUnicode(strTemp);
                strTempW += strFileName;
                //strTemp.Format(_T("(%d)%s"), i + 1, strFileName);
                strFileName = strTempW;
            }
        }

        //if (!strFileName.empty() && !strContent.empty())
        //{
        //    std::string strUtf8File = EncodeUtil::UnicodeToUtf8(strFileName);
        //    FILE* file = fopen(strUtf8File.c_str(), "wb+");
        //    if (file == nullptr)
        //        continue;

        //    size_t writtenLength = fwrite(strContent.c_str(), sizeof(wchar_t), strContent.length(), file);
        //    if (writtenLength != strContent.length())
        //    {
        //        //写文件失败
        //        fclose(file);
        //        continue;
        //    }

        //    fclose(file);
        //}
        break;
        }
    }

    return true;
}

bool MailHelper::detectMobilePlatform()
{
    if ((m_strMail.find("Content-Type") != std::string::npos) && (m_strMail.find("text/plain") != std::string::npos) &&
        (m_strMail.find("Content-Transfer-Encoding") != std::string::npos) && (m_strMail.find("base64") != std::string::npos) &&
        (m_strMail.find("gb2312") != std::string::npos || m_strMail.find("utf-8") != std::string::npos))
    {
        long lIndex1 = m_strMail.find("\r\n\r\n");
        long lIndex2 = m_strMail.find("\r\n.\r\n");
        std::string strTmp;
        if (lIndex1 != std::string::npos && lIndex2 != std::string::npos && lIndex2 > lIndex1)
        {
            strTmp = m_strMail.substr(lIndex1 + 3, lIndex2 - lIndex1 - 2);
            std::string strDecoded;
            Base64Util::decode(strTmp.c_str(), strTmp.length(), strDecoded);
            std::wstring strResult;
            if (m_strMail.find("gb2312") != std::string::npos)
                EncodeUtil::GB2312ToUnicode(strDecoded.c_str(), strResult);
            else if (m_strMail.find("utf-8") != std::string::npos)
                EncodeUtil::UTF8ToUnicode(strDecoded.c_str(), strResult);
            //strResult.Trim();
            trim(strResult);
            m_vecText.push_back(strResult);
            //m_aryHTML.push_back(CString(strResult));
            m_bIsFromMobilePlatform = true;
        }
    }

    return m_bIsFromMobilePlatform;
}

bool MailHelper::isExistBoundary(const std::string& strData, const char* lpExclude)
{
    size_t lCount = m_vecBoundary.size();
    for (size_t i = 0; i < lCount; ++i)
    {
        if (m_vecBoundary[i].compare(lpExclude) == 0)
            continue;

        if (strData.find(m_vecBoundary[i]) != std::string::npos) 
            return true;
    }

    return false;
}

int MailHelper::analyzeBoundary(const std::string& strData, std::wstring& strResult, std::string& strAttachment, std::wstring& strFileName)
{
    strResult.clear();
    strFileName.clear();
    strAttachment.clear();

    std::vector<std::string> vecString;
    std::map<std::string, std::string> mapString;
    //SplitString(strData, COMMAND_END_FLAG, aryString, 100);
    splitString(strData, vecString, COMMAND_END_FLAG);
    analyzeTag(vecString, mapString);

    long lStart = 0, lEnd = 0;
    std::string strTemp, strTemp2, strContent;
    long lType = 0, lCharsetType = 0;
    auto iter = mapString.find(EMAIL_CONTENT_TAG);  
    if (iter != mapString.end())
    {
        strTemp = iter->second;
        stringToUpper(strTemp);
        //strTemp.MakeUpper();
        if (strTemp.find(PLAIN_CONTENT_TYPE) != std::string::npos)
            lType = PLAIN_TYPE_ID;
        else if (strTemp.find(HTML_CONTENT_TYPE) != std::string::npos)
        {
            lStart = strTemp.find("NAME=\"");
            if (lStart == std::string::npos)
            {
                lType = HTML_TYPE_ID;
            }
            else
            {
                lType = APPLICATION_TYPE_ID;
                lEnd = strTemp.find("\"", lStart + 6);
                if (lEnd != std::string::npos)
                    strTemp2 = iter->second.substr(lStart + 6, lEnd - lStart - 6);
                //strTemp2.Trim('"');
                trim(strTemp2);
                if (!strTemp2.empty()) 
                    analyzeString(strTemp2, strFileName);
            }
        }
        else if (strTemp.find(APPLICATION_CONTENT_TYPE) != std::string::npos)
        {
            lType = APPLICATION_TYPE_ID;
            lStart = strTemp.find("NAME=\"");
            if (lStart != std::string::npos)
            {
                lEnd = strTemp.find("\"", lStart + 6);
                if (lEnd != std::string::npos)
                    strTemp2 = iter->second.substr(lStart + 6, lEnd - lStart - 6);
                //strTemp2.Trim('"');
                trim(strTemp2);
                if (!strTemp2.empty()) 
                    analyzeString(strTemp2, strFileName);
            }
            else
            {
                lStart = strTemp.find("NAME=");
                if (lStart != std::string::npos)
                    lStart = strTemp.find("\"", lStart + 5);
                if (lStart != std::string::npos)
                {
                    lEnd = strTemp.find("\"", lStart + 1);
                    if (lEnd != std::string::npos)
                        strTemp2 = iter->second.substr(lStart, lEnd - lStart);
                    //strTemp2.Trim('"');
                    trim(strTemp2);

                    if (!strTemp2.empty()) 
                        analyzeString(strTemp2, strFileName);
                }
            }
        }

        lCharsetType = findCharset(strTemp.c_str());
    }

    long lEncode = 0;
    iter = mapString.find(EMAIL_ENCODING_TAG);
    if (iter != mapString.end())
    {
        strTemp = iter->second;
        //strTemp.MakeUpper();
        stringToUpper(strTemp);
        if (strTemp.find(ENCODE_BASE64_TYPE) != std::string::npos)
            lEncode = ENCODE_BASE64_ID;
        else if (strTemp.find(ECONDE_QP_TYPE) != std::string::npos)
            lEncode = ENCODE_QUOTE_PRINTABLE_ID;
        else if (strTemp.find(ECONDE_7BIT_TYPE) != std::string::npos)
            lEncode = ENCODE_7_BIT_ID;
    }

    if (lType == 0 || lEncode == 0) 
        return 0;

    lStart = strData.find("Content-Type:");
    if (lStart == std::string::npos)
        return false;

    lEnd = strData.find("\r\n\r\n", lStart);
    if (lEnd == std::string::npos)
        return false;

    std::string strBody = strData.substr(lEnd + 4);
    if (lEncode == ENCODE_BASE64_ID)
    {
        //去掉后面的结尾
        long lEnd = strBody.find("\r\n\r\n");	 
        if (lEnd != std::string::npos)
            strBody = strBody.substr(0, lEnd);
    }

    //if (lType==APPLICATION_TYPE_ID)
    //{
    //	CFileException ex;
    //	CFile file;
    //	file.Open(_T("1112.txt"), CFile::modeWrite |CFile::shareDenyWrite | CFile::modeCreate, &ex);
    //	file.Write(strBody.GetString(),strBody.GetLength());
    //}

    switch (lEncode)
    {
    case ENCODE_BASE64_ID:
        Base64Util::decode(strBody.c_str(), strBody.length(), strContent);
        if (lType == APPLICATION_TYPE_ID)
        {
            strAttachment = strContent;
        }
        else
        {
            if (lCharsetType == CHARSET_UTF8_ID)
                EncodeUtil::UTF8ToUnicode(strContent.c_str(), strResult);
            else
                EncodeUtil::GB2312ToUnicode(strContent.c_str(), strResult);
        }
        break;

    case ENCODE_QUOTE_PRINTABLE_ID:
    {
        char* pDest = new char[strBody.length() * 2];
        EncodeUtil::DecodeQuotedPrintable(strBody.c_str(), pDest);
        strContent = pDest;
        delete[] pDest;
        if (lType == APPLICATION_TYPE_ID)
        {
            strAttachment = strContent;
        }
        else
        {
            if (lCharsetType == CHARSET_UTF8_ID)
                EncodeUtil::UTF8ToUnicode(strContent.c_str(), strResult);
            else
                EncodeUtil::GB2312ToUnicode(strContent.c_str(), strResult);
        }
    }
        
        break;

    case ENCODE_7_BIT_ID:
    {
        strContent = strBody;
        if (lType == APPLICATION_TYPE_ID)
        {
            strAttachment = strContent;
        }
        else
        {
            if (lCharsetType == CHARSET_UTF8_ID)
                EncodeUtil::UTF8ToUnicode(strContent.c_str(), strResult);
            else
                strResult = EncodeUtil::AnsiToUnicode(strBody);
        }
    }
        break;

    default:
        return 0;
    }

    return lType;
}

size_t MailHelper::analyzeTag(const std::vector<std::string>& vecString, std::map<std::string, std::string>& mapString)
{
    mapString.clear();
    size_t lCount = vecString.size();
    std::string strTemp, strContent, strTag;
    for (size_t i = 0, j = 0; i < lCount; ++i)
    {
        const std::string& strItem = vecString[i];
        if (strItem.empty()) 
            continue;

        if (strItem[0] == 0x20 || strItem[0] == 0x09)
        {
            if (!strTag.empty()) 
                strContent += strItem;
                
            continue;
        }

        if (!strTag.empty())
        {
            mapString.insert(std::pair<std::string, std::string>(strTag, strContent));
            strTag.clear();
        }

        strTemp = strItem;
        stringToUpper(strTemp);

        for (j = 0; j < ARRAYSIZE(EMAIL_TAG_LIST); ++j)
        {
            if (strTemp.find(EMAIL_TAG_LIST[j]) == 0)
            {
                strTag = EMAIL_TAG_LIST[j];
                strContent = strItem.substr(strlen(EMAIL_TAG_LIST[j]));
            }
        }

        if (i == lCount - 1 && !strTag.empty())
        {
            mapString.insert(std::pair<std::string, std::string>(strTag, strContent));
            strTag.clear();
        }
    }

    return (long)mapString.size();
}

long MailHelper::findCharset(const char* pszData)
{
    std::string strCharset(pszData);
    stringToUpper(strCharset);
    //strCharset.MakeUpper();

    if (strCharset.find("UTF-8") != std::string::npos) 
        return CHARSET_UTF8_ID;

    if (strCharset.find("GB2312") != std::string::npos)
        return CHARSET_GB2312_ID;

    if (strCharset.find("GBK") != std::string::npos)
        return CHARSET_GBK_ID;

    if (strCharset.find("7BIT") != std::string::npos)
        return CHARSET_ASCII_ID;

    return CHARSET_GB2312_ID;
}