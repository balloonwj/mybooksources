/**
 * 验证epoll LT与ET模式在处理写事件上的区别，epoll_server_write_event_et.cpp
 * zhangyl 2019.04.01
 */
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<poll.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<errno.h>
#include<iostream>

int main()
{
    //创建一个监听socket
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << "create listen socket error" << std::endl;
        return -1;
    }

    //设置重用IP地址和端口号
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&on, sizeof(on));

    //将监听socker设置为非阻塞的
    int oldSocketFlag = fcntl(listenfd, F_GETFL, 0);
    int newSocketFlag = oldSocketFlag | O_NONBLOCK;
    if (fcntl(listenfd, F_SETFL, newSocketFlag) == -1)
    {
        close(listenfd);
        std::cout << "set listenfd to nonblock error" << std::endl;
        return -1;
    }

    //初始化服务器地址
    struct sockaddr_in bindaddr;
    bindaddr.sin_family = AF_INET;
    bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bindaddr.sin_port = htons(3000);

    if (bind(listenfd, (struct sockaddr*) & bindaddr, sizeof(bindaddr)) == -1)
    {
        std::cout << "bind listen socker error." << std::endl;
        close(listenfd);
        return -1;
    }

    //启动监听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        std::cout << "listen error." << std::endl;
        close(listenfd);
        return -1;
    }


    //创建epollfd
    int epollfd = epoll_create(1);
    if (epollfd == -1)
    {
        std::cout << "create epollfd error." << std::endl;
        close(listenfd);
        return -1;
    }

    epoll_event listen_fd_event;
    listen_fd_event.data.fd = listenfd;
    listen_fd_event.events = EPOLLIN;
    //若取消注释这一行，则使用ET模式
    //listen_fd_event.events |= EPOLLET;

    //将监听sokcet绑定到epollfd上
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &listen_fd_event) == -1)
    {
        std::cout << "epoll_ctl error" << std::endl;
        close(listenfd);
        return -1;
    }

    int n;
    while (true)
    {
        epoll_event epoll_events[1024];
        n = epoll_wait(epollfd, epoll_events, 1024, 1000);
        if (n < 0)
        {
            //被信号中断
            if (errno == EINTR)
                continue;

            //出错，退出
            break;
        }
        else if (n == 0)
        {
            //超时，继续
            continue;
        }
        for (size_t i = 0; i < n; ++i)
        {
            //有读事件
            if (epoll_events[i].events & EPOLLIN)
            {
                if (epoll_events[i].data.fd == listenfd)
                {
                    //侦听socket，接受新连接
                    struct sockaddr_in clientaddr;
                    socklen_t clientaddrlen = sizeof(clientaddr);
                    int clientfd = accept(listenfd, (struct sockaddr*) & clientaddr, &clientaddrlen);
                    if (clientfd != -1)
                    {
                        int oldSocketFlag = fcntl(clientfd, F_GETFL, 0);
                        int newSocketFlag = oldSocketFlag | O_NONBLOCK;
                        if (fcntl(clientfd, F_SETFL, newSocketFlag) == -1)
                        {
                            close(clientfd);
                            std::cout << "set clientfd to nonblocking error." << std::endl;
                        }
                        else
                        {
                            epoll_event client_fd_event;
                            client_fd_event.data.fd = clientfd;
                            //同时侦听新来连接socket的读和写时间
                            client_fd_event.events = EPOLLIN | EPOLLOUT;
                            //若取消注释这一行，则使用ET模式
                            client_fd_event.events |= EPOLLET; 
                            if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_fd_event) != -1)
                            {
                                std::cout << "new client accepted,clientfd: " << clientfd << std::endl;
                            }
                            else
                            {
                                std::cout << "add client fd to epollfd error" << std::endl;
                                close(clientfd);
                            }
                        }
                    }
                }
                else
                {
                    std::cout << "client fd: " << epoll_events[i].data.fd << " recv data." << std::endl;
                    //普通clientfd
                    char recvbuf[1024] = { 0 };
                    //每次只收一个字节
                    int m = recv(epoll_events[i].data.fd, recvbuf, 1024, 0);
                    if (m == 0)
                    {
                        //对端关闭了连接，从epollfd上移除clientfd
                        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                        {
                            std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd << std::endl;
                        }
                        close(epoll_events[i].data.fd);
                    }
                    else if (m < 0)
                    {
                        //出错
                        if (errno != EWOULDBLOCK && errno != EINTR)
                        {
                            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, epoll_events[i].data.fd, NULL) != -1)
                            {
                                std::cout << "client disconnected,clientfd:" << epoll_events[i].data.fd << std::endl;
                            }
                            close(epoll_events[i].data.fd);
                        }
                    }
                    else
                    {
                        //正常收到数据
                        std::cout << "recv from client:" << epoll_events[i].data.fd << ", " << recvbuf << std::endl;
                       
                        epoll_event client_fd_event;
                        client_fd_event.data.fd = epoll_events[i].data.fd;
                        //再次给clientfd注册检测可写事件
                        client_fd_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                        if (epoll_ctl(epollfd, EPOLL_CTL_MOD, epoll_events[i].data.fd, &client_fd_event) != -1)
                        {
                            std::cout << "epoll_ctl successfully, mode: EPOLL_CTL_MOD, clientfd:" << epoll_events[i].data.fd << std::endl;
                        }

                    }
                }
            }
            else if (epoll_events[i].events & EPOLLOUT)
            {
                //只处理客户端fd的写事件
                if (epoll_events[i].data.fd != listenfd)
                {
                    //打印结果
                    std::cout << "EPOLLOUT triggered,clientfd: " << epoll_events[i].data.fd << std::endl;
                }
            }
            else if (epoll_events[i].events & EPOLLERR)
            {
                //TODO 暂不处理
            }
        }
    }

    close(listenfd);
    return 0;
}
