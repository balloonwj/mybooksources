/**
 * WebSocketServer类，MyWebSocketServer.h
 * zhangyl 2019.08.28
 */
#ifndef __MY_WEBSOCKET_SERVER_H__
#define __MY_WEBSOCKET_SERVER_H__

#include "MyWebSocketServer.h"

#include <memory>
#include <mutex>
#include <thread>
#include <list>

#include "../net/TcpServer.h"

using namespace net;

class BusinessSession;

class MyWebSocketServer final
{
public:
    MyWebSocketServer();
    ~MyWebSocketServer() = default;
    MyWebSocketServer(const MyWebSocketServer& rhs) = delete;
    MyWebSocketServer& operator =(const MyWebSocketServer& rhs) = delete;

public:
    bool init(const char* ip, short port, EventLoop* loop);
    void uninit();

    //新连接到来调用或连接断开，所以需要通过conn->connected()来判断，一般只在主loop里面调用
    void onConnection(std::shared_ptr<TcpConnection> conn);
    //连接断开
    void onClose(const std::shared_ptr<TcpConnection>& conn);

private:
    std::shared_ptr<TcpServer>                             m_server; 

    std::list<std::shared_ptr<BusinessSession>>            m_listSessions;
    std::mutex                                             m_mutexForSession;      //多线程之间保护m_sessions

    //开放的前置侦听ip地址
    std::string                                            m_strWsHost;
    //开放的前置端口号
    int                                                    m_wsPort;
};

#endif //!__MY_WEBSOCKET_SERVER_H__