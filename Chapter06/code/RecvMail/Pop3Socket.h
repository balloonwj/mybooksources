/** 
 * 收取邮件工具，Pop3Socket.h
 * zhangyl 2020.04.25
 */

#ifndef __POP3_SOCKET_H__
#define __POP3_SOCKET_H__

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "Platform.h"

class Pop3Socket
{
public:
    Pop3Socket(void);
    ~Pop3Socket(void);

    bool connect(const char* pszUrl, short port = 110);
    bool logon(const char* pszUser, const char* pszPassword);
    bool isConnected() const { return m_hSocket; }
    void quit();
    void close();

    bool getStat();
    bool getList();
    long getMailCount() const;
    long getMailID(long index) const;
    long getMailSize(long index) const;
    bool getHeader(int index);
    bool getMail(int index);
    	
    std::wstring getTitle(int nMailIndex) const;
    std::string getFrom(int nMailIndex) const;
    int64_t	getDate(int nMailIndex) const;
    int64_t	getTime(int nMailIndex) const;
    std::string getMessageID(int nMailIndex) const;
    std::wstring getSubject(int nMailIndex) const;

private:
    bool recvBody();
    bool recvData();

    bool request(bool toRecvBody);

    bool verifyRecvData();
    

private:
    SOCKET		                m_hSocket;
    std::string	                m_strSend;
    std::string	                m_strRecv;
    char		                m_szBuffer[1024 * 200];
    bool		                m_bConnected;

    struct                      Impl;
    std::unique_ptr<Impl>       m_pImpl;
};

#endif //!__POP3_SOCKET_H__
