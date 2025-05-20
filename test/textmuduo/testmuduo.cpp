/*
muduo网络库给用户提供了两个主要的类
TcpServer:用于编写服务器程序
TcpClient:用于编写客户端程序

epoll +线程池

能够把网络I/O的代码和业务代码区分开
业务代码：用户的连接和断开，用户的可读写事件
*/


#include<muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;

//基于muduo网络库开发服务器程序
//1.组合TcpServer对象
//2.创建EventLoop时间循环对象的指针
//3.明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
//4.在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写的回调函数
//设置合适的服务器端线程数量，muduo库会自己分配I/O线程和worker线程
class ChatServer
{
public:
    ChatServer(muduo::net::EventLoop* loop,//事件循环
               const muduo::net::InetAddress& listenAddr,//IP+端口
               const string& nameArg) //服务器的名称
        : _server(loop, listenAddr, nameArg), _loop(loop) 
    {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(
            std::bind(&ChatServer::onConnection, this, std::placeholders::_1));//绑定器
        //给服务器注册用户读写事件的回调
        _server.setMessageCallback(
            std::bind(&ChatServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        //设置服务器端的线程数量
        _server.setThreadNum(4);//1个IO线程，3个工作线程


    }
    //开启事件循环
    void start(){
        _server.start(); 
    }
private:
//专门处理用户的连接创建和断开  epoll listenfd accept
    void onConnection(const muduo::net::TcpConnectionPtr &conn){
        if(conn->connected()){
            cout << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << "status online" << endl;
    }else{
            cout << conn->peerAddress().toIpPort() << " -> "
            << conn->localAddress().toIpPort() << "status offline" << endl;
            conn->shutdown(); //关闭连接
        }
    }
    //专门处理用户的读写事件
    void onMessage(const muduo::net::TcpConnectionPtr& conn,//连接
                    muduo::net::Buffer* buffer,//缓冲区
                    muduo::Timestamp time)//接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data: " << buf << "time:" << time.toString() <<endl;
        //给用户发送数据
        conn->send(buf);
        
    }
    muduo::net::TcpServer _server; //#1
    muduo::net::EventLoop *_loop; //#2

};


int main(){

    muduo::net::EventLoop loop;//epoll
    muduo::net::InetAddress addr("192.168.157.128",6000);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop(); //epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等
    return 0;
}