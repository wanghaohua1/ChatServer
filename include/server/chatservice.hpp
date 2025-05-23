#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
using namespace std;
using namespace muduo;
using namespace muduo::net;
#include <mutex>
#include "json.hpp"
#include "offlinemessagemodel.hpp"
using json = nlohmann::json;
#include "UserModel.hpp"
#include "friendModel.hpp"
#include "groupmodel.hpp"
#include "redis.hpp"

//表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;
// 聊天服务器业务类
class ChatService
{
public:
//获取单例对象的接口函数
    static ChatService* instance();
    // 处理登陆业务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp);
    // 处理注册业务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp);

    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp);
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp);
    //获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    //创建群组
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp);

    //加入群组
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp);

    //群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp);

    void handleRedisSubscribeMessage(int userid,string msg);
    //服务器异常，业务重置方法
    void reset();

private:
    ChatService();
    //存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap;
    ///存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;
    //定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;
    //数据操作类对象
    UserModel _userModel;
    offlineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;
    Redis _redis;
};

#endif