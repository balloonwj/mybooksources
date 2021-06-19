/**
 * 验证select时间参数设置为NULL，select_client_tvnull.cpp
 * zhangyl 2018.12.25
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <string.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT     3000

int main(int argc, char* argv[])
{
    //创建一个socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        std::cout << "create client socket error." << std::endl;
        return -1;
    }

    //连接服务器
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

	int ret;
	while (true)
	{
		fd_set readset;
		FD_ZERO(&readset);
		//将侦听socket加入到待检测的可读事件中去
		FD_SET(clientfd, &readset);	
		//timeval tm;
		//tm.tv_sec = 0;
		//tm.tv_usec = 0;	
		
		//暂且只检测可读事件，不检测可写和异常事件
		ret = select(clientfd + 1, &readset, NULL, NULL, NULL);
		if (ret == -1)
		{
			//除了被信号中断的情形，其他情况都是出错
			if (errno != EINTR)
				break;
		}
		else if (ret == 0)
		{
			//select函数超时
			std::cout << "no event in specific time interval." << std::endl;
			continue;
		} 
		else
		{
			if (FD_ISSET(clientfd, &readset))
			{
				//检测到可读事件
				char recvbuf[32];
				memset(recvbuf, 0, sizeof(recvbuf));
				//假设对端发数据的时候不超过31个字符。
				int n = recv(clientfd, recvbuf, 32, 0);
				if (n < 0)
				{
					//除了被信号中断的情形，其他情况都是出错
					if (errno != EINTR)
						break;
				}
				else if (n == 0)
				{
					//对端关闭了连接
					break;
				}
				else
				{
					std::cout << "recv data: " << recvbuf << std::endl;
				}
			}
			else 
			{
				std::cout << "other socket event." << std::endl;
			}
		}
	}		
	
	//关闭socket
	close(clientfd);

    return 0;
}