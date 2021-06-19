#include <winsock2.h>
#include <stdio.h>

//链接Windows的socket库
#pragma comment(lib, "ws2_32.lib")

class ServerSocket
{
public:
    ServerSocket()
    {
        m_bInit = false;
        m_ListenSocket = -1;
    }

    ~ServerSocket()
    {
        if (m_ListenSocket != -1)
            ::closesocket(m_ListenSocket);

        if (m_bInit)
            ::WSACleanup();
    }

    bool DoInit()
    {
        //初始化socket库
        WORD wVersionRequested = MAKEWORD(2, 2);
        WSADATA wsaData;
        int err = WSAStartup(wVersionRequested, &wsaData);
        if (err != 0)
            return false;

        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
            return false;
            
        m_bInit = true;

        //创建用于监听的套接字
        m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_ListenSocket == -1)
            return false;

        return true;
    }

    bool DoBind(const char* ip, short port = 6000)
    {
        SOCKADDR_IN addrSrv;
        addrSrv.sin_addr.S_un.S_addr = inet_addr(ip);
        addrSrv.sin_family = AF_INET;
        addrSrv.sin_port = htons(port);
        if (::bind(m_ListenSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
            return false;

        return true;
    }

    bool DoListen(int backlog = 15)
    {
        if (listen(m_ListenSocket, backlog) == -1)
            return false;

        return true;
    }

    bool DoAccept()
    {
        SOCKADDR_IN addrClient;
        int len = sizeof(SOCKADDR);
        char msg[] = "HelloWorld";
        while (true)
        {
            //等待客户请求到来，如果有客户端连接，则接受连接
            SOCKET sockClient = accept(m_ListenSocket, (SOCKADDR*)&addrClient, &len);
            if (sockClient == -1)
                break;

            //给客户端发送”HelloWorld“消息
            send(sockClient, msg, strlen(msg), 0);
            closesocket(sockClient);
        }// end inner-while-loop

        return false;
    }

private:
    bool    m_bInit;
    SOCKET  m_ListenSocket;
};

int main(int argc, char* argv[])
{
    ServerSocket serverSocket;
    if (!serverSocket.DoInit())
        return false;

    if (!serverSocket.DoBind("0.0.0.0", 6000))
        return false;

    if (!serverSocket.DoListen(15))
        return false;

    if (!serverSocket.DoAccept())
        return false;

    return 0;
}


// void SomeFunction()
// {
//     得到某把锁；
//     if (条件1)
//     {
//         if (条件2)
//         {
//             某些操作1
//             释放锁;
//             return;
//         }
//         else (条件3)
//         {
//             某些操作2
//             释放锁;
//             return;
//         }
//     }

//     if (条件3)
//     {
//         某些操作3
//         释放锁;
//         return;
//     }

//     某些操作4
//     释放锁;
// }

// class SomeLockGuard
// {
// public:
//     SomeLockGuard()
//     {
//         //加锁
//         m_lock.lock();
//     }

//     ~SomeLockGuard()
//     {
//         //解锁
//         m_lock.unlock();
//     }

// private:
//     SomeLock  m_lock;
// };

// void SomeFunction()
// {
//     SomeLockGuard lockWrapper;
//     if (条件1)
//     {
//         if (条件2)
//         {
//             某些操作1
//             return;
//         }
//         else (条件3)
//         {
//             某些操作2
//             return;
//         }
//     }

//     if (条件3)
//     {
//         某些操作3
//         return;
//     }

//     某些操作4
// }