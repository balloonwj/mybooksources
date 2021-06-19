/** 
 * 封装curl的HTTP库，CurlClient.h
 * zhangyl 2019.08.27
 */

#ifndef __CURL_CLIENT_H__
#define __CURL_CLIENT_H__

#include <string>

#include "curl.h"

class CCurlClient final
{
public:
    CCurlClient();
    ~CCurlClient();

    CCurlClient(const CCurlClient& rhs) = delete;
    CCurlClient& operator=(const CCurlClient& rhs) = delete;

    /**
     * 初始化libcurl
     * 非线程安全函数，建议在程序初始化时调用一次，以免因多线程调用curl_easy_init而崩溃
     */
    static void init();
    /**
     * 反初始化libcurl
     * 非线程安全函数，建议在程序退出时调用一次
     */
    static void uninit();

    /** 发送HTTP GET请求
     * @param URL请求的网址
     * @param headers随请求发送的自定义HTTP头信息，多个自定义头之间使用\r\n分割，最后一个以\r\n结束，若无自定义HTTP头信息，则将其设置为nullptr
     * @param response如果请求成功，则存储HTTP请求结果（注意：response在函数调用中是追加模式，也就是说如果上一次response的值不清空，则调用这个函数时会追加，而不是覆盖）
     * @param autoRedirect请求得到HTTP 3xx的状态码是否自动重定向至新的URL
     * @param bReserveHeaders请求的结果中（存储于response），是否保留HTTP头信息
     * @param connTimeout连接超时时间，单位为秒（对于某些HTTP URI资源不好的，若总是返回超时，则可以将该参数设置得大一点）
     * @param readTimeout读取数据超时时间，单位为秒（对于某些HTTP URI资源不好的，若总是返回超时，则可以将该参数设置得大一点）
     */
    bool get(const char* url, const char* headers, std::string& response, bool autoRedirect = false, bool reserveHeaders = false, int64_t connTimeout = 1L, int64_t readTimeout = 5L);
    
    /** 发送HTTP POST请求
     * @param url请求的网址
     * @param headers随请求发送的自定义HTTP头信息，多个自定义头之间使用\r\n分割，最后一个以\r\n结束，若无自定义HTTP头信息，则将其设置为nullptr
     * @param postParams post参数内容
     * @param response如果请求成功，则存储HTTP请求的结果（注意：response在函数调用中是追加模式，也就是说如果上一次response的值不清空，则调用这个函数时会追加，而不是覆盖）
     * @param autoRedirect请求得到HTTP 3xx的状态码是否自动重定向至新的URL
     * @param bReserveHeaders请求的结果中（存储于response）是否保留HTTP头信息
     * @param connTimeout 连接超时时间，单位为秒（对于某些HTTP URI资源不好的，若总是返回超时，则可以将该参数设置得大一点）
     * @param readTimeout读取数据超时时间，单位为秒（对于某些HTTP URI资源不好的，若总是返回超时，则可以将该参数设置得大一点）
     */
    bool post(const char* url, const char* headers, const char* postParams, std::string& response, bool autoRedirect = false, bool reserveHeaders = false, int64_t connTimeout = 1L, int64_t readTimeout = 5L);


private:
    static bool  m_bGlobalInit;
};

#endif //!__CURL_CLIENT_H__