/**
 * 发送邮件类，SmtpSocket.h
 * zhangyl 2019.05.11
 */

#pragma once

#include <string>
#include <vector>
#include "Platform.h"

class SmtpSocket final
{
public:
    static bool sendMail(const std::string& server, short port, const std::string& from, const std::string& fromPassword,
                         const std::vector<std::string>& to, const std::string& subject, const std::string& mailData);

public:
	SmtpSocket(void);
	~SmtpSocket(void);
	
	bool isConnected() const { return m_hSocket; }
	bool connect(const char* pszUrl, short port = 25);
	bool logon(const char* pszUser, const char* pszPassword);
	bool setMailFrom(const char* pszFrom);
	bool setMailTo(const std::vector<std::string>& sendTo);
	bool send(const std::string& subject, const std::string& mailData);

	void closeConnection();
	void quit();	//退出

private:
	/*
	功能:
		验证从服务器返回的前三位代码和传递进来的参数是否一样

	备注:
		211 帮助返回系统状态
		214 帮助信息
		220 服务准备就绪
		221 关闭连接
		235 用户验证成功
		250 请求操作就绪
		251 用户不在本地，转寄到其他路径
		334 等待用户输入验证信息
		354 开始邮件输入
		421 服务不可用
		450 操作未执行，邮箱忙
		451 操作中止，本地错误
		452 操作未执行，存储空间不足
		500 命令不可识别或语言错误
		501 参数语法错误
		502 命令不支技
		503 命令顺序错误
		504 命令参数不支持
		550 操作未执行，邮箱不可用
		551 非本地用户
		552 中止存储空间不足
		553 操作未执行，邮箱名不正确
		554 传输失败
	*/
	bool checkResponse(const char* recvCode);

private:	
	bool		                m_bConnected;
	SOCKET		                m_hSocket;
    std::string                 m_strUser;
    std::string                 m_strPassword;
    std::string                 m_strFrom;
    std::vector<std::string>    m_strTo;
};