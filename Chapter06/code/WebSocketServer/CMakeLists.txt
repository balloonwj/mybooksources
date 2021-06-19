## 
## 编译的时候，使用cmake .
## zhangyl 2019.08.30
## QQ群：578019391
##

cmake_minimum_required(VERSION 2.6)

project (WEB_SOCKET_SERVER)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O0 -Wno-unused-variable -pthread")


include_directories(
	/usr/include/uuid/
)

link_directories(
	/usr/lib64/
    ${PROJECT_SOURCE_DIR}/lib
)

set(net_srcs 
base/AsyncLog.cpp
base/ConfigFileReader.cpp
base/Platform.cpp
base/Timestamp.cpp

net/Acceptor.cpp
net/Buffer.cpp
net/Channel.cpp
net/Connector.cpp
net/EpollPoller.cpp
net/EventLoop.cpp
net/EventLoopThread.cpp
net/EventLoopThreadPool.cpp
net/InetAddress.cpp
net/Poller.cpp
net/PollPoller.cpp
net/ProtocolStream.cpp
net/SelectPoller.cpp
net/Sockets.cpp
net/TcpClient.cpp
net/TcpConnection.cpp
net/TcpServer.cpp
net/Timer.cpp
net/TimerQueue.cpp
)

set(utils_srcs
utils/DaemonRun.cpp
utils/StringUtil.cpp
)

set(zlib_srcs
zlib1.2.11/zutil.c
zlib1.2.11/uncompr.c
zlib1.2.11/trees.c
zlib1.2.11/inftrees.c
zlib1.2.11/inflate.c
zlib1.2.11/inffast.c
zlib1.2.11/infback.c
zlib1.2.11/gzwrite.c
zlib1.2.11/gzread.c
zlib1.2.11/gzlib.c
zlib1.2.11/gzclose.c
zlib1.2.11/deflate.c
zlib1.2.11/crc32.c
zlib1.2.11/compress.c
zlib1.2.11/adler32.c
zlib1.2.11/ZlibUtil.cpp
)

set(websocketsrc
websocketsrc/MyWebSocketServer.cpp
websocketsrc/MyWebSocketSession.cpp
)

set(app_srcs
appsrc/main.cpp
appsrc/BusinessSession.cpp
)

add_executable(websocketserver ${net_srcs} ${utils_srcs} ${zlib_srcs} ${websocketsrc} ${app_srcs})
