#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

//extern int h_errno;

bool connect_to_server(const char* server, short port)
{
    int hSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (hSocket == -1)
        return false;

    struct sockaddr_in addrSrv = { 0 };
    struct hostent* pHostent = NULL;
    //unsigned int addr = 0;

    //如果传入的参数 server 的值是 somesite.com 这种域名域名形式则 if 条件成立，
	//接着调用 gethostbyname 解析域名为 4 字节的 ip 地址（整型）
	if (addrSrv.sin_addr.s_addr = inet_addr(server) == INADDR_NONE)
    {       
		pHostent = gethostbyname(server);
        if (pHostent == NULL)      
            return false;
        
        //当存在多个域名时，我们只取第一个
        addrSrv.sin_addr.s_addr = *((unsigned long*)pHostent->h_addr_list[0]);
    }

    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(port);
    int ret = connect(hSocket, (struct sockaddr*)&addrSrv, sizeof(addrSrv));
    if (ret == -1)
        return false;

    return true;
}

int main()
{
	if (connect_to_server("baidu.com", 80))
		printf("connect successfully.\n");
	else
		printf("connect error.\n");
	
	return 0;
}