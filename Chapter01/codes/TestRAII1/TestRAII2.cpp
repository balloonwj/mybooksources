// #include "stdafx.h"
// #include <winsock2.h>
// #include <stdio.h>

// //链接Windows的socket库
// #pragma comment(lib, "ws2_32.lib")

// int main(int argc, char* argv[])
// {
//     //由于goto语句不能跳过变量声明，
//     //所以提前定义下文需要用到的变量
//     SOCKET sockSrv;
//     SOCKADDR_IN addrSrv;
//     SOCKADDR_IN addrClient;
//     int len = sizeof(SOCKADDR);
//     char msg[] = "HelloWorld";
    
//     //初始化socket库
//     WORD wVersionRequested = MAKEWORD(2, 2);
//     WSADATA wsaData;
//     int err = WSAStartup(wVersionRequested, &wsaData);
//     if (err != 0)
//         return 1;

//     if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
//     {
//         goto cleanup2;
//         return 1;
//     }

//     //创建用于监听的套接字
//     sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockSrv == -1)
//     {
//         goto cleanup2;
//         return 1;
//     }

//     addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//     addrSrv.sin_family = AF_INET;
//     addrSrv.sin_port = htons(6000);
//     //绑定套接字，监听6000端口
//     if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
//     {
//         goto cleanup1;
//         return 1;
//     }

//     //启动监听，准备接受客户请求
//     if (listen(sockSrv, 15) == -1)
//     {
//         goto cleanup1;
//         return 1;
//     }

//     while (true)
//     {
//         //等待客户请求到来，如果有客户端连接，则接受连接
//         SOCKET sockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
//         if (sockClient == -1)
//             break;

//         //给客户端发送”HelloWorld“消息
//         send(sockClient, msg, strlen(msg), 0);
//         closesocket(sockClient);
//     }// end while-loop

// cleanup1:
//     closesocket(sockSrv);

// cleanup2:
//     WSACleanup();

//     return 0;
// }

//
////char* p = new char[1024];
////
////if (²Ù×÷1²»³É¹¦)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (²Ù×÷2²»³É¹¦)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (²Ù×÷3²»³É¹¦)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////delete[] p;
////p = NULL;