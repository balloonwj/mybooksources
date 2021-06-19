/**
 *  邮件报警demo
 *  zhangyl 2020.04.09
 **/

#include <iostream>
#include <stdlib.h>
#include "Platform.h"
#include "MailMonitor.h"

//Winsock网络库初始化
#ifdef WIN32
NetworkInitializer windowsNetworkInitializer;
#endif


#ifndef WIN32
void prog_exit(int signo)
{
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;

    //停止邮件发送服务
    MailMonitor::getInstance().uninit();
}

#endif

const std::string servername = "MailAlertSysem";
const std::string mailserver = "smtp.163.com";
const short mailport = 25;
const std::string mailuser = "testformybook@163.com";
const std::string mailpassword = "2019hhxxttxs";
const std::string mailto = "balloonwj@qq.com;analogous_love@qq.com";

int main(int argc, char* argv[])
{
#ifndef WIN32
    //设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGTERM, prog_exit);
#endif
  
    
    bool bInitSuccess = MailMonitor::getInstance().initMonitorMailInfo(servername, mailserver, mailport, mailuser, mailpassword, mailto);
    if (bInitSuccess)
        MailMonitor::getInstance().run();

    const std::string subject = "Alert Mail";
    const std::string content = "This is an alert mail from " + mailuser;
    MailMonitor::getInstance().alert(subject, content);

    //等待报警邮件线程退出
    MailMonitor::getInstance().wait();

    return 0;
}