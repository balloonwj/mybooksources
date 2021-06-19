#include "StdAfx.h"
#include "MailThread.h"
#include "LogTrace.h"

using namespace Log;

MailThread::MailThread(void)
    :m_lCheckMailCount(0), m_lFinishCount(0), m_bDispatched(TRUE)
{
}

MailThread::~MailThread(void)
{
}

DWORD MailThread::execute()
{
    m_lStatus = THREAD_STATUS_FREE;
    m_EventCancel.ResetEvent();

    CStringA strUser, strUrl, strPassword;
    m_lStatus = THREAD_STATUS_CONNECT_DB;
    m_lStatus = THREAD_STATUS_FREE;
    MailAnalysis	mail;
    long lWaitTime = 1;
    long lMailCount = 0, i = 0, lMailIndex = 0;
    long lTick = 0;
    bool bExit = false;
    while (!bExit && !m_EventCancel.Lock(lWaitTime))
    {
        lWaitTime = 1000 * 30;
        if (!m_Pop3Socket.isConnected())
        {
            m_lStatus = THREAD_STATUS_LOGON;
            strUrl = m_szUrl;
            if (!m_Pop3Socket.connect(strUrl, m_lPort))
            {
                m_Pop3Socket.close();
                m_lStatus = THREAD_STATUS_FREE;
                continue;
            }

            strUser = m_strUser;
            strPassword = m_strPassword;
            if (!m_Pop3Socket.logon(strUser, strPassword))
            {
                m_Pop3Socket.close();
                m_lStatus = THREAD_STATUS_FREE;
                continue;
            }
        }

        m_lStatus = THREAD_STATUS_GET_LIST;
        if (!m_Pop3Socket.getList())
        {
            m_Pop3Socket.close();
            m_lStatus = THREAD_STATUS_FREE;
            continue;
        }

        lMailCount = m_Pop3Socket.getMailCount();
        if (lMailCount <= 0) m_lStatus = THREAD_STATUS_EMPTY_EMAIL;

        MailLogNormal(_T("[MailThread::Execute] need scan email (%d)."), lMailCount);

        //开始检索邮件
        for (lTick = 0; i < lMailCount; ++i, ++lTick)
        {
            if (i == lMailCount - 1)
                ++ m_lFinishCount;

            if (lTick % 20 == 0)
            {
                if (m_EventCancel.Lock(1))
                {
                    bExit = TRUE;
                    break;
                }
            }

            m_lStatus = THREAD_STATUS_CHECK_SUBJECT;
            lMailIndex = m_Pop3Socket.getMainIndex(lMailCount - i - 1);	//从最近的邮件开始收
            if (!m_Pop3Socket.getHeader(lMailIndex))
            {
                if (!m_Pop3Socket.isConnected()) break;
                MailLogWarning(_T("[MailThread] Failed to get Email header, [Email Index=%d], program will try next time."), lMailIndex);
                continue;
            }

            ++m_lCheckMailCount;
            //是否将邮件存入表trav_research中
            BOOL bStoreIntoDB = FALSE;
            do
            {
                //if (!MailFilter.FilterMailHeader(m_Pop3Socket.GetTitle(), m_Pop3Socket.GetFrom(), m_Pop3Socket.GetMessageID(), m_Pop3Socket.GetDate(), m_Pop3Socket.GetTime()))
                //    break;

                ::OutputDebugString(m_Pop3Socket.getTitle());
                ::OutputDebugString(_T("\t"));
                ::OutputDebugString(m_Pop3Socket.getFrom());
                ::OutputDebugString(_T("\t"));
                ::OutputDebugString(m_Pop3Socket.getMessageID());
                ::OutputDebugString(_T("\t"));
                //::OutputDebugString(m_Pop3Socket.GetDate());
                ::OutputDebugString(_T("\r\n"));
                

                m_lStatus = THREAD_STATUS_DOWNLOAD_ID;
                if (!m_Pop3Socket.getMail(lMailIndex, mail))
                    break;

                //if (!MailFilter.FilterContent(mail))
                //    break;

                bStoreIntoDB = TRUE;

            } while (0);

            //将所有调研邮件写入数据库中
            m_lStatus = THREAD_STATUS_INTO_DB;
            //MailFilter.WriteDBComplete(m_Pop3Socket.getMessageID(), m_Pop3Socket.getTitle(), m_Pop3Socket.getFrom(), m_Pop3Socket.getDate(), m_Pop3Socket.getTime());
            //if (bStoreIntoDB)
            //    MailFilter.WriteDB();
        }

        MailLogNormal(_T("[MailThread::Execute] start scan finish.(%d/%d)"), i, lMailCount);

        if (i == lMailCount)
        {
            i = 0;
            m_Pop3Socket.quit();
            m_Pop3Socket.close();
            //5分钟扫一次
            lWaitTime = 1000 * 60 * 5;	
        }

        m_lStatus = THREAD_STATUS_FREE;
    }

    m_dwThreadId = 0;
    if (m_hThread)
    {
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }

    m_lStatus = THREAD_STATUS_STOP;
    MailLogNormal(_T("[MailThread::Execute] Exit"));

    return TRUE;
}

void MailThread::setServerInfo(LPCTSTR lpUrl, long lPort, LPCTSTR lpUser, LPCTSTR lpPassword)
{
    m_szUrl = lpUrl;
    m_lPort = lPort;
    m_strUser = lpUser;
    m_strPassword = lpPassword;
}
