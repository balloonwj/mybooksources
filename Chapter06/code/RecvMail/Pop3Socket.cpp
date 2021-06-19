//#include "StdAfx.h"
#include "Pop3Socket.h"

#include <vector>
#include <map>
#include <iostream>
#include <stdio.h>

#include "MailHelper.h"
#include "EncodeUtil.h"


//结构体全部1字节对齐
struct MailItem
{
    //邮件ID
    long	                                        id;
    //邮件大小
    long	                                        size;
    //邮件标题
    std::wstring		                            title;
    //邮件主题
    std::wstring	                                subject;
    int64_t		                                    date;
    int64_t		                                    time;
    //发件人邮箱
    std::string		                                from;
    //邮件ID，一般可以作                            为邮件的唯一标识
    std::string		                                messageID;
    //邮件内容
    std::wstring	                                content;
    //附件名和附件内容
    static std::vector<std::wstring, std::string>	m_vecAttachments;
};


struct Pop3Socket::Impl
{
    typedef std::vector<MailItem>   MAIL_LIST;
    MAIL_LIST	                    _MailList;
};

Pop3Socket::Pop3Socket(void) : 
    m_hSocket(INVALID_SOCKET), m_bConnected(false), m_pImpl(new Impl)
{
    memset(m_szBuffer, 0, sizeof(m_szBuffer));
}

Pop3Socket::~Pop3Socket(void)
{
}

void Pop3Socket::quit()
{
    if (m_hSocket == INVALID_SOCKET)
        return;

    m_strSend = "QUIT \r\n";

    request(false);
}

void Pop3Socket::close()
{
    if (m_hSocket != INVALID_SOCKET)
    {
        closesocket(m_hSocket);
        m_hSocket = INVALID_SOCKET;
    }

    m_bConnected = false;
}

long Pop3Socket::getMailCount() const
{
    return (long)m_pImpl->_MailList.size();
}

long Pop3Socket::getMailID(long index) const
{
    if (index < 0 || (long)m_pImpl->_MailList.size() <= index)
        return -1;

    return m_pImpl->_MailList[index].id;
}

long Pop3Socket::getMailSize(long lIndex) const
{
    if (lIndex < 0 || (long)m_pImpl->_MailList.size() <= lIndex) 
        return -1;

    return m_pImpl->_MailList[lIndex].size;
}

bool Pop3Socket::connect(const char* pszUrl, short port/* = 110*/)
{
    struct sockaddr_in server = { 0 };
    struct hostent* pHostent = NULL;
    unsigned int addr = 0;

    Pop3Socket::close();
    m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_hSocket == INVALID_SOCKET) 
        return false;

    long tmSend(15 * 1000L), tmRecv(15 * 1000L), noDelay(1);
    setsockopt(m_hSocket, IPPROTO_TCP, TCP_NODELAY, (LPSTR)& noDelay, sizeof(long));
    setsockopt(m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (LPSTR)& tmSend, sizeof(long));
    setsockopt(m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (LPSTR)& tmRecv, sizeof(long));

    if (inet_addr(pszUrl) == INADDR_NONE)
        pHostent = gethostbyname(pszUrl);
    else
    {
        addr = inet_addr(pszUrl);
        pHostent = gethostbyaddr((char*)& addr, sizeof(addr), AF_INET);
    }

    if (pHostent == nullptr) 
        return false;

    server.sin_family = AF_INET;
    server.sin_port = htons((u_short)port);
    server.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr);
    if (::connect(m_hSocket, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
        return false;

    if (!recvData()) 
        return false;

    if (!verifyRecvData()) 
        return false;
    
    return true;
}

bool Pop3Socket::recvBody()
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    int nRecv = 0;
    long lRecvSize = 0;
    long lPos = 0;
    long lEndLen = 0;
    m_strRecv.clear();
    long lTryCount = 20;
    while (true)
    {
        fd_set fd = { 0 };
        timeval tmout = { 0,1 };
        FD_SET(m_hSocket, &fd);
        if (select(0, &fd, NULL, NULL, &tmout) < 1 || !FD_ISSET(m_hSocket, &fd))
        {
            if (lTryCount <= 0) break;

            Sleep(500);
            --lTryCount;
            continue;
        }

        memset(m_szBuffer, 0, sizeof(m_szBuffer));
        nRecv = ::recv(m_hSocket, m_szBuffer, sizeof(m_szBuffer) - 1, NULL);
        if (nRecv == SOCKET_ERROR)
        {
            Pop3Socket::close();
            return false;
        }

        if (nRecv > 0)
        {
            m_strRecv.append(m_szBuffer, nRecv);
            lRecvSize += nRecv;
        }

        if (m_strRecv.find("\r\n.\r\n") != std::string::npos)
            break;
    }

    //严格判断邮件结尾,否则无效
    if (m_strRecv.find("\r\n.\r\n") == std::string::npos)
        return false;

    return true;
}

bool Pop3Socket::recvData()
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    int nRecv = 0;
    long lRecvSize = 0;
    long lPos = 0;
    long lEndLen = 0;
    m_strRecv.clear();
    long nTryCount = 5;
    while (true)
    {
        fd_set fd = { 0 };
        timeval tv = { 0,1 };
        FD_SET(m_hSocket, &fd);
        if (select(0, &fd, NULL, NULL, &tv) < 1 || !FD_ISSET(m_hSocket, &fd))
        {
            if (nTryCount <= 0)
                break;

            --nTryCount;
            sleepMS(50);
            continue;
        }

        memset(m_szBuffer, 0, sizeof(m_szBuffer));
        nRecv = ::recv(m_hSocket, m_szBuffer, sizeof(m_szBuffer) - 1, NULL);
        if (nRecv == SOCKET_ERROR)
        {
            Pop3Socket::close();
            return false;
        }

        if (nRecv > 0)
        {
            m_strRecv.append(m_szBuffer, nRecv);
            lRecvSize += nRecv;
        }

        //收到\r\n之后就停止接受数据
        if (m_strRecv.find("\r\n") != std::string::npos) 
            break;
    }

    //严格判断结尾,否则无效
    if (m_strRecv.find("\r\n") == std::string::npos)
        return false;

    return true;
}

bool Pop3Socket::verifyRecvData()
{
    if (m_strRecv.empty()) 
        return false;

    //检测收到的数据中前4个字符是不是"+OK"或"-ERR"
    char szHeader[5] = { 0 };
    long lHeaderSize = sizeof(szHeader) - 1;
    strncpy_s(szHeader, m_strRecv.c_str(), lHeaderSize);
    _strupr_s(szHeader);
    if (szHeader[0] == '+' && szHeader[1] == 'O' && szHeader[2] == 'K') 
        return true;

    if (szHeader[0] == '-' && szHeader[1] == 'E' && szHeader[2] == 'R' && szHeader[3] == 'R') 
        return false;

    return true;
}

bool Pop3Socket::logon(const char* pszUser, const char* pszPassword)
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    int nResult = 0;
    char szTmp[32] = { 0 };
    sprintf(szTmp, "USER %s\r\n", pszUser);
    m_strSend.clear();
    m_strSend.append(szTmp, strlen(szTmp));
    if (!request(false))
        return false;

    if (!verifyRecvData()) 
        return false;

    memset(szTmp, 0, sizeof(szTmp));
    sprintf(szTmp, "PASS %s\r\n", pszPassword);
    m_strSend.clear();
    m_strSend.append(szTmp, strlen(szTmp));
    if (!request(false)) 
        return false;

    if (!verifyRecvData()) 
        return false;

    m_bConnected = true;

    return true;
}

bool Pop3Socket::getStat()
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    m_strSend = "STAT\r\n";
    if (!request(FALSE)) 
        return false;

    if (!verifyRecvData()) 
        return false;

    /*
    m_strSend.Format("UIDL\r\n",0);
    if (!request(DATA_END_FLAG)) 
        return false;
    if (!verifyRecvData()) 
        return false;
    */

    return true;
}

bool Pop3Socket::getList()
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    m_strSend = "LIST\r\n";
    if (!request(true)) 
        return false;

    if (!verifyRecvData()) 
        return false;

    Impl::MAIL_LIST& mailList = m_pImpl->_MailList;
    mailList.clear();

    //解析返回的邮件列表的字符串：+OK 5 30284\r\n1 8284\r\n2 11032\r\n3 2989\r\n4 3871\r\n5 4108\r\n.\r\n
    MailItem item = { 0 };
    std::vector<std::string> vecString; 
    std::vector<std::string> vecStringItem;
    MailHelper::splitString(m_strRecv, vecString, "\r\n");
    size_t count = vecString.size();
    std::cout << "mail list: " << std::endl;
    for (size_t i = 0; i < count; ++i)
    {
        const std::string& strItem = vecString[i];
        if (strItem[0] == '+') 
            continue;

        MailHelper::splitString(strItem, vecStringItem, " ");
        if (vecStringItem.size() == 2)
        {
            item.id = atoi(vecStringItem[0].c_str());
            item.size = atoi(vecStringItem[1].c_str());

            std::cout << "id: " << item.id << ", size: " << item.size << std::endl;
            mailList.push_back(item);
        }
        vecStringItem.clear();
    }

    return true;
}


bool Pop3Socket::request(bool toRecvBody)
{
    if (m_hSocket == INVALID_SOCKET)
        return false;

    if (m_strSend.empty()) 
        return true;

    m_strRecv.clear();
    
    int result = ::send(m_hSocket, m_strSend.c_str(), (int)m_strSend.length(), NULL);
    
    Sleep(300);

    if (result == SOCKET_ERROR)
        return false;

    if (toRecvBody)
    {
        if (!recvBody())
        {
            Pop3Socket::close();
            return false;
        }
    }
    else
    {
        if (!recvData())
        {
            Pop3Socket::close();
            return false;
        }
    }

    return true;
}

bool Pop3Socket::getHeader(int index)
{
    //m_dwDate = m_dwTime = 0;
    //m_strFrom.Empty();
    //m_strMessageID.Empty();
    //m_strTitle.Empty();
    //m_strSubject.Empty();
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    int nResult = 0;
    char szTmp[16] = { 0 };
    sprintf(szTmp, "TOP % d 1\r\n", index);
    m_strSend.clear();
    m_strSend.append(szTmp, strlen(szTmp));

    if (!request(true)) 
        return false;

    //邮件格式
    /*
        +OK 4108 octets
        Received: from sonic310-21.consmr.mail.gq1.yahoo.com (unknown [98.137.69.147])
	        by mx29 (Coremail) with SMTP id T8CowABHlztmml5erAoHAQ--.23443S3;
	        Wed, 04 Mar 2020 01:56:57 +0800 (CST)
        DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed; d=yahoo.com; s=s2048; t=1583258213; bh=ABL3sF+YL/syl+mwknwxiAlvKPRNYq4AYTujNrPA86g=; h=Date:From:Reply-To:Subject:References:From:Subject; b=OrAQTs0GJnA9yVA08gRolpsoe9E2PQhc3BLvK0msqlZkIYPYVLD1SHAHc7eI3imH4b+hggrFA6wUeiSqqq2du3tOokCU8ckq3LrbdI82EZ013M3KL6o2y+/wdPIj9Mo1TeGbmqtthYBOpGvEgwzsQMNnydkJdy5tDaW6IBT2Ux+IaP0K+jp71eYXcWjdR0mSyu3aMhLqc0z4l5HlZYpZRQG1hjxZOaCH/UjgBAdr98JecVvuRp4s5iGe6OIxc0p3xzRZBxTlLdgdHjmTKHQ00eTNCfFYai2rMxf4830lMYTwKI6O/iu3jUbTA2yjxx0LrYBFTiWzFetwAQupKLw3Qg==
        X-YMail-OSG: 7HG016cVM1nEI.fdz8BF9PN3tO6MvrppAOwu_jpQ09s4eVdYvLXavghvjDvWrRW
         B7PF6pZKuhiLjV7yCErxEmbWUKPLzX.WL4RJ9u4tnPC4NyVp30cLaoGZVIapWeFtqRpKlh31orVY
         WTsWE9FcDuHts5p2MPAd7Si52EZfyPuoffEIWrd481hx1IdSsRQN_V7mpfxihvReOIoQ5rCWuMzd
         oK5kXOho8iOwXlEVPzdTs33RD5rQmwbycPtLS7.FARNxWl9yO9Lrd25gDYa1hXvgG4aQptJQK5aD
         cHQpZUYqdiaNUaEoGoIDQR_HVndus53gTyzUzmJONpDo6wQM39O.pih7VGCrgLqB2_hHeJdPEUIk
         jCcwkqNn0cfDyc4QwBdQ65jcgm2cJDPFgoODhxDIqTqeFVbXFr2cXLand8vAqARi3tlnmsOUA7ZI
         DiSvhSx8eYGd4_frX1LfP.TpctO9Uuc3ZP6iP_K24F9HE9HNN9_swBUEPlBOB3jjSPSOdmiEMteF
         NWj1qOJ8i47BwMBILtx0dZheRRSxvfzSA.JmnUghFo80EgaGRgXYIEAzt8hpvxdZbtwrg0k0WPeF
         Y4LC2my.A9XcsnF58558bweJDaDHCJyLGFnE8__ZQI163vMPqY6QbU3OP0EJz2OE1rPBOrq9PUol
         TZjOEu6ghV1PG2HhX0Ydc.vvq5mloqbKusdzV5EgtpFZjLdp1_RQWuI1LG865Ig756HBaozMU6RG
         0FUMn86pvXRBbNMPD6ADwcw4rdw.Xqk5TRZkqJpSp6KX82OjAgFu0xxMiZnQ7LNemrsJ2UQK9Y2_
         nm8nrwOIX03Ol6Z2KspWUcPNkqPIZ6vGAr9FO9qqE_elB3K4hh04lq_KS5Tv_XoI3deD4r3J6RTb
         O9xp5O6cbe0Svy7FS7DosvJfK958_57Kk_6vk6wxxc3D8cx_k6P.yPbphTCLYFdfnbV5sRKNvUKT
         .apHpO8d0GUf29QtSc3dwBDrLEcRSpguJ3tMKBc2GZPCwUMOgf2b24zFZ49.D7MRQbZifaHsk4dJ
         L9jxS2qdN5pSjhZUjbLCUQ2YGcYgmNnTbfjAIaxqUWNSgpypIYNmi.lgG4bM_gW7sXH_Y3TULcsC
         .1GXTSjZUdUvvkr7BDnzUy3FGqv9Eyfb7GOwPzTXLzdurcd6eHx0ejCmC6gVJIwoIh9S0YSK659a
         K2usThSAyogrxqQ664fZr70CrLJehr5OZNLstPt3fiJhyUR1DXrlm6myQ9uSQ5vPTl0p2.DemDaY
         k84mtcZO0EEjKIzqeSvZ505Fex0u.66Mzu2lmr07WwMCE7wgqwOSWRnYNCz2rWcLmXA_TVDtdJ85
         bHZ79FY6Vs5pGJjp.7YgDnVqysBp95w--
        Received: from sonic.gate.mail.ne1.yahoo.com by sonic310.consmr.mail.gq1.yahoo.com with HTTP; Tue, 3 Mar 2020 17:56:53 +0000
        Date: Tue, 3 Mar 2020 17:56:49 +0000 (UTC)
        From: Peter Edward Copley <noodlelife@yahoo.com>
        Reply-To: Peter Edward Copley <pshun3592@gmail.com>
        Message-ID: <729348196.5391236.1583258209467@mail.yahoo.com>
        Subject: Re:Hello
        MIME-Version: 1.0
        Content-Type: multipart/alternative; 
	        boundary="----=_Part_5391235_1821490954.1583258209466"
        References: <729348196.5391236.1583258209467.ref@mail.yahoo.com>
        X-Mailer: WebService/1.1.15302 YMailNorrin Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.122 Safari/537.36
        X-CM-TRANSID:T8CowABHlztmml5erAoHAQ--.23443S3
        Authentication-Results: mx29; spf=pass smtp.mail=noodlelife@yahoo.com;
	         dkim=pass header.i=@yahoo.com
        X-Coremail-Antispam: 1Uf129KBjDUn29KB7ZKAUJUUUUU529EdanIXcx71UUUUU7v73
	        VFW2AGmfu7bjvjm3AaLaJ3UbIYCTnIWIevJa73UjIFyTuYvjxU-NtIUUUUU

        ------=_Part_5391235_1821490954.1583258209466
        .

    */
    std::vector<std::string> vecString;
    std::vector<std::string> vecStringItems;
    std::map<std::string, std::string> mapString;
    MailHelper::splitString(m_strRecv, vecString, "\r\n");
    MailHelper::analyzeTags(vecString, mapString);
    std::string strTemp;
    long lStart = 0, lEnd = 0;

    //邮件时间
    auto iter = mapString.find(EMAIL_DATE_TAG);	
    if (iter != mapString.end())
    {
        //取出格式"Tue, 3 Mar 2020 17:56:49 +0000 (UTC)"中的时间
        lStart = iter->second.find(",", 0);
        strTemp = iter->second.substr(lStart + 1);
        lStart = strTemp.find("+", 0);
        //去掉时区
        if (lStart != std::string::npos) 
            strTemp = strTemp.substr(0, lStart);

        TimeItem timeItem;
        if (MailHelper::parseTimeString(strTemp, timeItem))
        {
            m_pImpl->_MailList[index - 1].date = timeItem.year * 10000 + timeItem.month * 100 + timeItem.day;
            m_pImpl->_MailList[index - 1].time = timeItem.hour * 10000 + timeItem.minute * 100 + timeItem.second;
        }      
    }

    iter = mapString.find(EMAIL_SUBJECT_TAG);
    if (iter != mapString.end())
    {
        m_pImpl->_MailList[index - 1].subject = EncodeUtil::s2ws(std::string(EMAIL_SUBJECT_TAG + iter->second));
        MailHelper::analyzeString(iter->second, m_pImpl->_MailList[index - 1].title);
    }

    iter = mapString.find(EMAIL_FROM_TAG);
    if (iter != mapString.end())
    {
        lStart = iter->second.find('<');
        if (lStart != std::string::npos)
        {
            lEnd = iter->second.find('>', lStart + 1);
            if (lEnd != std::string::npos)
            {
                strTemp = iter->second.substr(lStart + 1, lEnd - lStart - 1);
                m_pImpl->_MailList[index - 1].from = strTemp;
            }
        }
    }

    iter = mapString.find(EMAIL_MESSAGE_ID_TAG);
    if (iter != mapString.end())
    {
        strTemp = iter->second;
        lStart = iter->second.find('<');
        if (lStart != std::string::npos)
        {
            lEnd = iter->second.find('>', lStart + 1);
            if (lEnd != std::string::npos)
                strTemp = iter->second.substr(lStart + 1, lEnd - lStart - 1);
        }
        m_pImpl->_MailList[index - 1].messageID = strTemp;
    }

    return true;
}

std::wstring Pop3Socket::getTitle(int index) const
{
    return m_pImpl->_MailList[index - 1].title;
}

std::string Pop3Socket::getFrom(int index) const
{ 
    return m_pImpl->_MailList[index - 1].from;
}

int64_t Pop3Socket::getDate(int index) const
{ 
    return m_pImpl->_MailList[index - 1].date;
}

int64_t	Pop3Socket::getTime(int index) const
{ 
    return m_pImpl->_MailList[index - 1].time;
}

std::string Pop3Socket::getMessageID(int index) const
{ 
    return m_pImpl->_MailList[index - 1].messageID;
}

std::wstring Pop3Socket::getSubject(int index) const
{ 
    return m_pImpl->_MailList[index - 1].subject;
}

bool Pop3Socket::getMail(int index)
{
    if (m_hSocket == INVALID_SOCKET) 
        return false;

    char szCmd[32] = { 0 };
    sprintf(szCmd, "RETR %d\r\n", index);
    m_strSend = szCmd;
    if (!request(true)) 
        return false;

    if (!verifyRecvData()) 
        return false;

    return MailHelper::analyzeBody(m_strRecv);
}