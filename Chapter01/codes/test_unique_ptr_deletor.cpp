#include <iostream>
#include <memory>

class Socket
{
public:
    Socket()
    {

    }

    ~Socket()
    {

    }

    //关闭资源句柄
    void close()
    {

    }
};

int main()
{
    auto deletor = [](Socket* pSocket) {
        //关闭句柄
        pSocket->close();
        //TODO: 你甚至可以在这里打印一行日志...
        delete pSocket;
    };

    //std::unique_ptr<Socket, void(*)(Socket * pSocket)> spSocket(new Socket(), deletor);
    std::unique_ptr<Socket, decltype(deletor)> spSocket(new Socket(), deletor);

    return 0;
}