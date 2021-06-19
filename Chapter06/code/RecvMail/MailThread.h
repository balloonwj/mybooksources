#pragma once
#include <deque>
#include "Pop3Socket.h"

class MailThread
{
public:
	MailThread(void);
	virtual ~MailThread(void);

	void setServerInfo(LPCTSTR lpUrl,long lPort,LPCTSTR lpUser,LPCTSTR lpPassword);

	inline long getCheckMailCount() const { return m_lCheckMailCount; }
	inline long getFinishCount() const { return m_lFinishCount; }
	__int64 getTotalRecvSize() const { return m_Pop3Socket.getTotalRecvSize(); }
	inline void enableDispatch(BOOL bEnable) { m_bDispatched=bEnable;}

protected:
	virtual DWORD execute();

	CString	        m_szUrl;
	long	        m_lPort;
	CString	        m_strUser;
	CString         m_strPassword;

	long	        m_lCheckMailCount;	//一共扫描的邮件个数
	long	        m_lFinishCount;		//扫描完成次数

	bool	        m_bDispatched;		//是否转发

	Pop3Socket	    m_Pop3Socket;
};
