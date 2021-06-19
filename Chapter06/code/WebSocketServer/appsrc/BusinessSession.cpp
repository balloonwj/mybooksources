/***************************************************************
 * 版权所有 :
 * 文件名   : BusinessSession.h
 * 作者     : zhangyl
 * 版本     : 1.0.0.0
 * 创建日期 : 2019.03.29
 * 描述     :
 ***************************************************************/
#include "BusinessSession.h"

std::string BusinessSession::m_strWelcomeMsg = "Welcome to WebSocket Server, contact: balloonwj@qq.com";

BusinessSession::BusinessSession(std::shared_ptr<TcpConnection>& conn) : MyWebSocketSession(conn)
{
    
}

bool BusinessSession::onMessage(const std::string& strClientMsg)
{   
    //TODO: 收到的消息在这里处理，这里仅做一个消息回显
    send(strClientMsg);

    return true;
}

void BusinessSession::onConnect()
{ 
    //发送欢迎语
    sendWelcomeMsg();     
}

void BusinessSession::sendWelcomeMsg()
{
    send(BusinessSession::m_strWelcomeMsg);
}