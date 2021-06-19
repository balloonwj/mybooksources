#include "stdafx.h"
//#include <winsock2.h>
//#include <stdio.h>
//
////链接Windows的socket库
//#pragma comment(lib, "ws2_32.lib")
//
//int main(int argc, char* argv[])
//{
//    //加载套接字库
//    WORD wVersionRequested = MAKEWORD(2, 2);
//    WSADATA wsaData;
//    int err = WSAStartup(wVersionRequested, &wsaData);
//    if (err != 0)
//        return 1;
//
//    SOCKET sockSrv = -1;
//    do 
//    {
//        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
//            break;
//
//        //创建用于监听的套接字        
//        sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//        if (sockSrv == -1)
//            break;
//
//        SOCKADDR_IN addrSrv;
//        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//        addrSrv.sin_family = AF_INET;
//        addrSrv.sin_port = htons(6000);
//        //绑定套接字，在6000端口上监听
//        if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
//            break;
//
//        //将套接字设为监听模式，准备接受客户请求
//        if (listen(sockSrv, 15) == -1)
//            break;
//
//        SOCKADDR_IN addrClient;
//        int len = sizeof(SOCKADDR);
//        char msg[] = "HelloWorld";
//        while (true)
//        {
//            //等待客户请求到来，如果有客户端连接，则接收连接
//            SOCKET sockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
//            //给客户端发送HelloWorld发送数据
//            send(sockClient, msg, strlen(msg), 0);
//            closesocket(sockClient);
//        }// end inner-while-loop
//    } while (0); //end outer-while-loop
//    
//
//    if (sockSrv != -1)
//        closesocket(sockSrv);
//    
//    WSACleanup();
//
//    return 0;
//}
//
////char* p = new char[1024];
////
////if (操作1不成功)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (操作2不成功)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////if (操作3不成功)
////{
////    delete[] p;
////    p = NULL;
////    return;
////}
////
////delete[] p;
////p = NULL;
//
//
//#include "stdafx.h"
//#include <winsock2.h>
//#include <stdio.h>
//
////链接Windows的socket库
//#pragma comment(lib, "ws2_32.lib")
//
//int main(int argc, char* argv[])
//{
//    //加载套接字库
//    WORD wVersionRequested = MAKEWORD(1, 1);
//    WSADATA wsaData;
//    int err = WSAStartup(wVersionRequested, &wsaData);
//    if (err != 0)
//        return 1;
//
//    SOCKET sockSrv = -1;
//    do
//    {
//        if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
//            break;
//
//        //创建用于监听的套接字        
//        sockSrv = socket(AF_INET, SOCK_STREAM, 0);
//        if (sockSrv == -1)
//            break;
//
//        SOCKADDR_IN addrSrv;
//        addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//        addrSrv.sin_family = AF_INET;
//        addrSrv.sin_port = htons(6000);
//        //绑定套接字，在6000端口上监听
//        if (bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == -1)
//            break;
//
//        //将套接字设为监听模式，准备接受客户请求
//        if (listen(sockSrv, 15) == -1)
//            break;
//
//        SOCKADDR_IN addrClient;
//        int len = sizeof(SOCKADDR);
//        char msg[] = "HelloWorld";
//        while (true)
//        {
//            //等待客户请求到来，如果有客户端连接，则接收连接
//            SOCKET sockClient = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
//            //给客户端发送HelloWorld发送数据
//            send(sockClient, msg, strlen(msg), 0);
//            closesocket(sockClient);
//        }// end inner-while-loop
//    } while (0); //end outer-while-loop
//
//
//    if (sockSrv != -1)
//        closesocket(sockSrv);
//
//    WSACleanup();
//
//    return 0;
//}
//
//char* p = NULL;
//do 
//{
//   p = new char[1024];
//   if (操作1不成功)
//       break;
//
//   if (操作2不成功)
//       break;
//
//   if (操作3不成功)
//       break;
//} while (0);
//
//delete[] p;
//p = NULL;