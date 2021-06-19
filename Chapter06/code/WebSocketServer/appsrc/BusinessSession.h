/***************************************************************
 * 版权所有 :
 * 文件名   : BusinessSession.h
 * 作者     : zhangyl
 * 版本     : 1.0.0.0
 * 创建日期 : 2019.03.29
 * 描述     :
 ***************************************************************/

#ifndef __BUSINESS_SESSION_H__
#define __BUSINESS_SESSION_H__

#include <string>

#include "../websocketsrc/MyWebSocketSession.h"

/** 
 * 使用方法：创建你自己的BusinessSession类，在这里专注于业务处理
 * BussinessSession类专注于业务处理，WebSocketSession类专注于网络通信本身
 */
class BusinessSession : public MyWebSocketSession
{
public:
    BusinessSession(std::shared_ptr<TcpConnection>& conn);
    virtual ~BusinessSession() = default;

public:
    void onConnect() override;
    bool onMessage(const std::string& strClientMsg) override;
 
private:
    void sendWelcomeMsg();

private:
    static std::string          m_strWelcomeMsg;        //欢迎信息
};

#endif //!__BUSINESS_SESSION_H__
