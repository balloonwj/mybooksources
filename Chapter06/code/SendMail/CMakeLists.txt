cmake_minimum_required(VERSION 2.6)

project (SEND_MAIL_DEMO)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g -Wall -O0 -Wno-unused-variable -pthread")

set(srcs 
Base64Util.cpp
MailMonitor.cpp
Platform.cpp
SmtpSocket.cpp
main.cpp
)

add_executable(alertmailsystem ${srcs})
TARGET_LINK_LIBRARIES(alertmailsystem)
