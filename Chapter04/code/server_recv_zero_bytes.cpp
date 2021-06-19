/**
 * 验证recv函数接受0字节的行为，server端，server_recv_zero_bytes.cpp
 * zhangyl 2018.12.17
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <vector>

int main(int argc, char* argv[])
{
    //1.创建一个侦听socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error." << std::endl;
        return -1;
    }

    //2.初始化服务器地址
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);
    if (bind(listenfd, (struct sockaddr *)&bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socket error." << std::endl;
		close(listenfd);
        return -1;
    }

	//3.启动侦听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
		close(listenfd);
        return -1;
    }
	
	int clientfd; 
	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	//4. 接受客户端连接
	clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientaddrlen);
	if (clientfd != -1)
	{         	
		while (true)
		{
			char recvBuf[32] = {0};
			//5. 从客户端接受数据,客户端没有数据来的时候会recv函数会阻塞
			int ret = recv(clientfd, recvBuf, 32, 0);
			if (ret > 0) 
			{
				std::cout << "recv data from client, data: " << recvBuf << std::endl;				
			} 
			else if (ret == 0)
			{
				//“假设recv返回值为0时是收到了0个字节”
				std::cout << "recv 0 byte data." << std::endl;
				continue;
			} 
			else
			{
				//出错
				std::cout << "recv data error." << std::endl;
				break;
			}
		}				
	}

	
	//关闭客户端socket
	close(clientfd);
	//7.关闭侦听socket
	close(listenfd);

    return 0;
}