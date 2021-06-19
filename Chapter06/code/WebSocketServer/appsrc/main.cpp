/**
 *  聊天服务程序入口函数
 *  zhangyl 2017.03.09
 **/

#include <iostream>
#include <stdlib.h>
#include "../base/Platform.h"
#include "../base/Singleton.h"
#include "../base/AsyncLog.h"
#include "../net/EventLoop.h"
#include "../net/EventLoopThreadPool.h"
#include "../utils/DaemonRun.h"
#include "../websocketsrc/MyWebSocketServer.h"


using namespace net;

//Winsock网络库初始化
#ifdef WIN32
NetworkInitializer windowsNetworkInitializer;
#endif

EventLoop g_mainLoop;

#ifndef WIN32
void prog_exit(int signo)
{
    std::cout << "program recv signal [" << signo << "] to exit." << std::endl;


    g_mainLoop.quit();

    LOGC("app finish......");

    CAsyncLog::Uninit();
}
#endif

int main(int argc, char* argv[])
{
#ifndef WIN32
    //设置信号处理
    signal(SIGCHLD, SIG_DFL);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, prog_exit);
    signal(SIGTERM, prog_exit);

    int ch;
    bool bdaemon = false;
    while ((ch = getopt(argc, argv, "dv")) != -1)
    {
        switch (ch)
        {
        case 'd':
            bdaemon = true;
            break;
        }
    }

    if (bdaemon)
        daemon_run();

#endif

    CAsyncLog::Init();

    Singleton<MyWebSocketServer>::Instance().init("0.0.0.0", 9988, &g_mainLoop);

    LOGC("websocket server initialization complete.");

    g_mainLoop.loop();

    return 0;
}