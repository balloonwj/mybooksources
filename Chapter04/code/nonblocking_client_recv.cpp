/**
 * 验证非阻塞模式下recv函数的行为，client端，nonblocking_client_recv.cpp
 * zhangyl 2018.12.17
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT     3000
#define SEND_DATA       "helloworld"

int main(int argc, char* argv[])
{
    //1.创建一个socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        std::cout << "create client socket error." << std::endl;
        return -1;
    }

    //2.连接服务器
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serveraddr.sin_port = htons(SERVER_PORT);
    if (connect(clientfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        std::cout << "connect socket error." << std::endl;
		close(clientfd);
        return -1;
    }
	
	//连接成功以后，我们再将 clientfd 设置成非阻塞模式，
	//不能在创建时就设置，这样会影响到 connect 函数的行为
	int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
	int newSocketFlag = oldSocketFlag | O_NONBLOCK;
	if (fcntl(clientfd, F_SETFL,  newSocketFlag) == -1)
	{
		close(clientfd);
		std::cout << "set socket to nonblock error." << std::endl;
		return -1;
	}
	
	while (true)
	{
		char recvbuf[32] = {0};
		//由于clientfd被设置成了非阻塞模式，所以无论是否有数据，recv函数都不会阻塞程序
		int ret = recv(clientfd, recvbuf, 32, 0);
		if (ret > 0) 
		{
			//收到了数据
			std::cout << "recv successfully." << std::endl;
		} 
		else if (ret == 0)
		{
			//对端关闭了连接
			std::cout << "peer close the socket." << std::endl;	
			break;
		} 
		else if (ret == -1) 
		{
			if (errno == EWOULDBLOCK)
			{
				std::cout << "There is no data available now." << std::endl;
			} 
			else if (errno == EINTR) 
			{
				//如果被信号中断了，则继续重试recv函数
				std::cout << "recv data interrupted by signal." << std::endl;				
			} else
			{
				//真的出错了
				break;
			}
		}
	}
	
	//3. 关闭socket
	close(clientfd);

    return 0;
}