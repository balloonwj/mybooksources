/*
==========================================================================

Purpose:

	* 这个类CIOCPModel是本代码的核心类，用于说明WinSock服务器端编程模型中的
	  完成端口(IOCP)的使用方法，并使用MFC对话框程序来调用这个类实现了基本的
	  服务器网络通信的功能。

	* 其中的PER_IO_DATA结构体是封装了用于每一个重叠操作的参数
	  PER_HANDLE_DATA 是封装了用于每一个Socket的参数，也就是用于每一个完成端口的参数

Notes:

	* 具体讲明了服务器端建立完成端口、建立工作者线程、投递Recv请求、投递Accept请求的方法，
	  所有的客户端连入的Socket都需要绑定到IOCP上，所有从客户端发来的数据，都会实时显示到
	  主界面中去。

==========================================================================
*/

#pragma once

// winsock 2 的头文件和库
#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

// 缓冲区长度 (1024*8)
// 之所以为什么设置8K，也是一个江湖上的经验值
// 如果确实客户端发来的每组数据都比较少，那么就设置得小一些，省内存
#define MAX_BUFFER_LEN        8192  
// 默认端口
#define DEFAULT_PORT          8080    
// 默认IP地址
#define DEFAULT_IP            _T("127.0.0.1")


//////////////////////////////////////////////////////////////////
// 在完成端口上投递的I/O操作的类型
typedef enum _OPERATION_TYPE
{
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;

//====================================================================================
//
//				单IO数据结构体定义(用于每一个重叠操作的参数)
//
//====================================================================================

typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET         m_sockAccept;                               // 这个网络操作所使用的Socket
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE m_OpType;                                   // 标识网络操作的类型(对应上面的枚举)

	// 初始化
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType = NULL_POSTED;
	}
	// 释放掉Socket
	~_PER_IO_CONTEXT()
	{
		if (m_sockAccept != INVALID_SOCKET)
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}
	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

} PER_IO_CONTEXT, * PPER_IO_CONTEXT;


//====================================================================================
//
//				单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)
//
//====================================================================================

typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET      m_Socket;                                  // 每一个客户端连接的Socket
	SOCKADDR_IN m_ClientAddr;                              // 客户端的地址
	CArray<_PER_IO_CONTEXT*> m_arrayIoContext;             // 客户端网络操作的上下文数据，
														   // 也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的

	// 初始化
	_PER_SOCKET_CONTEXT()
	{
		m_Socket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	}

	// 释放资源
	~_PER_SOCKET_CONTEXT()
	{
		if (m_Socket != INVALID_SOCKET)
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		// 释放掉所有的IO上下文数据
		for (int i = 0; i < m_arrayIoContext.GetCount(); i++)
		{
			delete m_arrayIoContext.GetAt(i);
		}
		m_arrayIoContext.RemoveAll();
	}

	// 获取一个新的IoContext
	_PER_IO_CONTEXT* GetNewIoContext()
	{
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;

		m_arrayIoContext.Add(p);

		return p;
	}

	// 从数组中移除一个指定的IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext)
	{
		ASSERT(pContext != NULL);

		for (int i = 0; i < m_arrayIoContext.GetCount(); i++)
		{
			if (pContext == m_arrayIoContext.GetAt(i))
			{
				delete pContext;
				pContext = NULL;
				m_arrayIoContext.RemoveAt(i);
				break;
			}
		}
	}

} PER_SOCKET_CONTEXT, * PPER_SOCKET_CONTEXT;




//====================================================================================
//
//				CIOCPModel类定义
//
//====================================================================================

// 工作者线程的线程参数
class CIOCPModel;
typedef struct _tagThreadParams_WORKER
{
	CIOCPModel* pIOCPModel;                                   // 类指针，用于调用类中的函数
	int         nThreadNo;                                    // 线程编号

} THREADPARAMS_WORKER, * PTHREADPARAM_WORKER;

// CIOCPModel类
class CIOCPModel
{
public:
	CIOCPModel(void);
	~CIOCPModel(void);

public:
	// 启动服务器
	bool Start();

	//	停止服务器
	void Stop();

	// 加载Socket库
	bool LoadSocketLib();

	// 卸载Socket库，彻底完事
	void UnloadSocketLib() { WSACleanup(); }

	// 获得本机的IP地址
	CString GetLocalIP();

	// 设置监听端口
	void SetPort(const int& nPort) { m_nPort = nPort; }

	// 设置主界面的指针，用于调用显示信息到界面中
	void SetMainDlg(CDialog* p) { m_pMain = p; }

private:
	// 初始化IOCP
	bool _InitializeIOCP();

	// 初始化Socket
	bool _InitializeListenSocket();

	// 最后释放资源
	void _Uninitialize();

	// 投递Accept请求
	bool _PostAccept(PER_IO_CONTEXT* pAcceptIoContext);

	// 投递接收数据请求
	bool _PostRecv(PER_IO_CONTEXT* pIoContext);

	// 在有客户端连入的时候，进行处理
	bool _DoAccept(PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext);

	// 在有接收的数据到达的时候，进行处理
	bool _DoRecv(PER_SOCKET_CONTEXT* pSocketContext, PER_IO_CONTEXT* pIoContext);

	// 将客户端的相关信息存储到数组中
	void _AddToContextList(PER_SOCKET_CONTEXT* pSocketContext);

	// 将客户端的信息从数组中移除
	void _RemoveContext(PER_SOCKET_CONTEXT* pSocketContext);

	// 清空客户端信息
	void _ClearContextList();

	// 将句柄绑定到完成端口中
	bool _AssociateWithIOCP(PER_SOCKET_CONTEXT* pContext);

	// 处理完成端口上的错误
	bool HandleError(PER_SOCKET_CONTEXT* pContext, const DWORD& dwErr);

	// 线程函数，为IOCP请求服务的工作者线程
	static DWORD WINAPI _WorkerThread(LPVOID lpParam);

	// 获得本机的处理器数量
	int _GetNoOfProcessors();

	// 判断客户端Socket是否已经断开
	bool _IsSocketAlive(SOCKET s);

	// 在主界面中显示信息
	void _ShowMessage(const CString szFormat, ...) const;

private:
	// 用来通知线程系统退出的事件，为了能够更好的退出线程
	HANDLE                       m_hShutdownEvent;              
	// 完成端口的句柄
	HANDLE                       m_hIOCompletionPort;           
	// 工作者线程的句柄指针
	HANDLE* m_phWorkerThreads;             
	// 生成的线程数量
	int		                     m_nThreads;                    
	// 服务器端的IP地址
	CString                      m_strIP;  
	// 服务器端的监听端口
	int                          m_nPort;                      
	// 主界面的界面指针，用于在主界面中显示消息
	CDialog* m_pMain;                       
	// 用于Worker线程同步的互斥量
	CRITICAL_SECTION             m_csContextList;               
	// 客户端Socket的Context信息
	CArray<PER_SOCKET_CONTEXT*>  m_arrayClientContext;                  
	// 用于监听的Socket的Context信息
	PER_SOCKET_CONTEXT*			 m_pListenContext;              
	// AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_ACCEPTEX                m_lpfnAcceptEx;                
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs;

};

