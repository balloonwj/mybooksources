#ifndef __MAIL_HELPER_H__
#define __MAIL_HELPER_H__

#include <string>
#include <map>
#include <vector>

#define LINE_END_FLAG		            "\r\n"
#define COMMAND_END_FLAG	            "\r\n"
#define DATA_END_FLAG		            ".\r\n"
#define TITLE_CODE_TYPE		            "=?gb2312?B?"
#define TITLE_CODE_TYPE_END	            "?="

#define EMAIL_DATE_TAG			        "DATE:"
#define EMAIL_SUBJECT_TAG		        "SUBJECT:"
#define EMAIL_MESSAGE_ID_TAG	        "MESSAGE-ID:"
#define EMAIL_FROM_TAG			        "FROM:"
#define EMAIL_CONTENT_TAG		        "CONTENT-TYPE:"
#define EMAIL_BOUNDARY_TAG		        "BOUNDARY="
#define	EMAIL_ENCODING_TAG		        "CONTENT-TRANSFER-ENCODING:"
#define EMAIL_CONTENT_DESPOSITION_TAG   "Content-Disposition:"

#define	PLAIN_CONTENT_TYPE				"TEXT/PLAIN"
#define	HTML_CONTENT_TYPE				"TEXT/HTML"
#define	APPLICATION_CONTENT_TYPE		"APPLICATION"

#define ENCODE_BASE64_TYPE				"BASE64"
#define ECONDE_QP_TYPE					"QUOTED-PRINTABLE"
#define ECONDE_7BIT_TYPE				"7BIT"
#define MAIL_ADRRESS_SPLIT_LINE			_T("--split line--")
#define MAIL_ADRRESS_SPLIT_LINE_DISPLAY	_T("-----------------\r\n")

const char EMAIL_TAG_LIST[][32] =
{
    EMAIL_SUBJECT_TAG,
    EMAIL_DATE_TAG,
    EMAIL_MESSAGE_ID_TAG,
    EMAIL_FROM_TAG,
    EMAIL_CONTENT_TAG,
    EMAIL_ENCODING_TAG,
};

enum TEXT_TYPE_ID
{
    UNKONW_TEXT_TYPE_ID = 0,
    PLAIN_TYPE_ID = 1,
    HTML_TYPE_ID,
    APPLICATION_TYPE_ID
};

enum ENCODE_TYPE_ID
{
    ENCODE_BASE64_ID = 1,
    ENCODE_QUOTE_PRINTABLE_ID,
    ENCODE_7_BIT_ID
};

enum CHARSET_TYPE_ID
{
    CHARSET_GB2312_ID = 0,
    CHARSET_UTF8_ID,
    CHARSET_GBK_ID,
    CHARSET_ASCII_ID
};

struct TimeItem
{
    int year;
    int month;
    int day;

    int hour;
    int minute;
    int second;
};

class MailHelper final
{
public:
	MailHelper() = delete;
	~MailHelper() = delete;

public:
    static void splitString(const std::string& str, std::vector<std::string>& v, const char* delimiter = "|");
    static int analyzeTags(const std::vector<std::string>& aryString, std::map<std::string, std::string>& mapString);
    static bool parseTimeString(const std::string& dateString, TimeItem& timeItem);
    static void analyzeString(const std::string& strData, std::wstring& strResult);
    static size_t findOneOf(const std::string& str, const std::string& matchPatterns);
    static long getCharset(const char* pszData);
    static void stringToUpper(std::string& str);
    static void stringToLower(std::string& str);
    static void trim(std::string& str, const char delimiter = ' ');
    static void trim(std::wstring& str, const wchar_t delimiter = L' ');
    static bool analyzeBody(const std::string& strBody);
    static bool getBoundaryTag();
    static bool analyzeBoundary(const std::string& strString, const std::string& strBoundary);
    static bool detectMobilePlatform();
    static bool isExistBoundary(const std::string& strData, const char* lpExclude);
    static int analyzeBoundary(const std::string& strData, std::wstring& strResult, std::string& strAttachment, std::wstring& strFileName);
    static size_t analyzeTag(const std::vector<std::string>& vecString, std::map<std::string, std::string>& mapString);
    static long findCharset(const char* pszData);
private:
    static bool		                                m_bIsFromMobilePlatform;

    static std::string                              m_strMail;

    static std::vector<std::string>	                m_vecBoundary;
    static std::vector<std::wstring>	            m_vecText;
    static std::vector<std::wstring>	            m_vecHTML;

    static std::map<std::wstring, std::string>	    m_vecAttachment;	//¸½¼þ

    static wchar_t                                  g_szEmailSavePath[256];
};

#endif //!__MAIL_HELPER_H__
