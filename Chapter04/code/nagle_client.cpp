/**
 * 验证nagle算法
 * zhangyl 2018.12.13
 */
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

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
        return -1;
    }
	
	int count = 1000;
	int ret;
	//3. 连续向服务器发送1000次数据
	while (count > 0)
	{
		ret = send(clientfd, SEND_DATA, strlen(SEND_DATA), 0);
		if (ret != strlen(SEND_DATA))
		{
			std::cout << "send data error." << std::endl;
			break;
		}

		count --;
	}
	
	if (count == 0)
		std::cout << "send data successfully" << std::endl;
	
	//4. 从客户端收取数据
	char recvBuf[32] = {0};
	ret = recv(clientfd, recvBuf, 32, 0);
	if (ret > 0) 
	{
		std::cout << "recv data successfully, data: " << recvBuf << std::endl;
	} 
	else 
	{
		std::cout << "recv data error, data: " << recvBuf << std::endl;
	}
	
	//5. 关闭socket
	close(clientfd);

    return 0;
}