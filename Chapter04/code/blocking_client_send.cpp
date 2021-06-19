/**
 * 验证阻塞模式下send函数的行为，client端
 * zhangyl 2018.12.17
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
		close(clientfd);
        return -1;
    }

	//3. 不断向服务器发送数据，或者出错退出
	int count = 0;
	while (true)
	{
		int ret = send(clientfd, SEND_DATA, strlen(SEND_DATA), 0);
		if (ret != strlen(SEND_DATA))
		{
			std::cout << "send data error." << std::endl;
			break;
		} 
		else
		{
			count ++;
			std::cout << "send data successfully, count = " << count << std::endl;
		}
	}
	
	//5. 关闭socket
	close(clientfd);

    return 0;
}