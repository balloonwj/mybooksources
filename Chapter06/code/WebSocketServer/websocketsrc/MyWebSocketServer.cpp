/**
 * MyWebSocketServer类，MyMyWebSocketServer.cpp
 * zhangyl 2019.08.28
 */

#include "MyWebSocketServer.h"

#include <string>

#include "../base/AsyncLog.h"
#include "../base/Singleton.h"
#include "../net/InetAddress.h"
#include "../net/TcpServer.h"
#include "../net/EventLoop.h"
#include "../net/EventLoopThread.h"
#include "../net/EventLoopThreadPool.h"
#include "../base/Platform.h"
#include "../appsrc/BusinessSession.h"

#include "MyWebSocketSession.h"

MyWebSocketServer::MyWebSocketServer()
{

}

bool MyWebSocketServer::init(const char* ip, short port, EventLoop* loop)
{
    InetAddress addr(ip, port);
    m_server.reset(new TcpServer(loop, addr, "MY-WEBSOCKET-SERVER", TcpServer::kReusePort));
    m_server->setConnectionCallback(std::bind(&MyWebSocketServer::onConnection, this, std::placeholders::_1));
    //启动侦听
    unsigned int threadCount = std::thread::hardware_concurrency() + 2;
    m_server->start(threadCount);

    return true;
}

void MyWebSocketServer::uninit()
{
    if (m_server)
        m_server->stop();
}

//新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
void MyWebSocketServer::onConnection(std::shared_ptr<TcpConnection> conn)
{
    if (conn->connected())
    {
        std::shared_ptr<BusinessSession> spSession(new BusinessSession(conn));
        conn->setMessageCallback(std::bind(&MyWebSocketSession::onRead, spSession.get(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        {           
            std::lock_guard<std::mutex> guard(m_mutexForSession);
            m_listSessions.push_back(spSession);
        }    
    }
    else
    {
        onClose(conn);
    }
}

//连接断开
void MyWebSocketServer::onClose(const std::shared_ptr<TcpConnection>& conn)
{
    //TODO: 这样的代码逻辑太混乱，需要优化
    std::unique_lock<std::mutex> guard(m_mutexForSession);
    for (auto iter = m_listSessions.begin(); iter != m_listSessions.end(); ++iter)
    {
        //通过比对connection对象找到对应的session
        if ((*iter)->getConnectionPtr() == conn)
        {         
            (*iter)->onDisconnect();
            LOGI("client: [%s] disconnected, session: 0x%0x", (*iter)->getClientInfo(), (int64_t)((*iter).get()));

            m_listSessions.erase(iter);
            return;
        }
    }


    LOGE("Unable to find session, conn = 0x%llx\n", (int64_t)conn->getLoop());
}