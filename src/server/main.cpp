#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>
#include <iostream>

using namespace std;
//处理服务器ctrl+c结束后，重置user的状态信息
void  resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}
int main()
{
    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress addr("192.168.157.128",6000);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop(); //epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等
    return 0;
}