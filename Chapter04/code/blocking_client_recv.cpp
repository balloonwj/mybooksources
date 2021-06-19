/**
 * 验证阻塞模式下recv函数的行为，client端，blocking_client_recv.cpp
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
	
	char recvbuf[32] = {0};
    //3.直接调用recv函数，程序会阻塞在recv函数调用处
	int ret = recv(clientfd, recvbuf, 32, 0);
	if (ret > 0) 
	{
		std::cout << "recv successfully." << std::endl;
	} 
	else 
	{
		std::cout << "recv data error." << std::endl;
	}
	
	//4. 关闭socket
	close(clientfd);

    return 0;
}