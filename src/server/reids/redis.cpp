#include "redis.hpp"
#include <iostream>
using namespace std;

Redis::Redis() : _publisherContext(nullptr), _subscriberContext(nullptr)
{
}

Redis::~Redis()
{
    if (_publisherContext != nullptr)
    {
        redisFree(_publisherContext);
    }

    if (_subscriberContext != nullptr)
    {
        redisFree(_subscriberContext);
    }
}
// 连接redis
bool Redis::connect()
{
    // 负责publish发布消息的上下文连接
    _publisherContext = redisConnect("192.168.157.128", 6379);
    if (nullptr == _publisherContext)
    {
        cout << "connect redis failed!" << endl;
        return false;
    }
    // 负责subscribe订阅消息的上下文连接
    _subscriberContext = redisConnect("192.168.157.128", 6379);
    if (nullptr == _subscriberContext)
    {
        cout << "connect redis failed!" << endl;
        return false;
    }
    //在单独的线程内部执行订阅消息的操作
    thread t([&](){
        observer_channel_message();
    });
    t.detach();
    cout << "connect redis success!" << endl;
    return true;
}

// 向redis指定通道channel发布消息
bool Redis::publish(int channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(_publisherContext, "PUBLISH %d %s", channel, message.c_str());
    if(nullptr ==reply)
    {
        cerr << "publish message failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
// 向redis指定的通道subscribe订阅消息
bool Redis::subscribe(int channel)
{
    if(REDIS_ERR == redisAppendCommand(this->_subscriberContext,"SUBSCRIBE %d", channel))
    {
        cerr << "subscribe message failed!" << endl;
        return false;
    }
    //redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕
    int done =0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->_subscriberContext, &done))
        {
            cerr << "subscribe message failed!" << endl;
            return false;
        }
    }

    return true;
}
// 向redis指定的通道unsubscribe取消订阅消息
bool Redis::unsubscribe(int channel)
{
    if(REDIS_ERR == redisAppendCommand(this->_subscriberContext,"UNSUBSCRIBE %d", channel))
    {
        cerr << "unsubscribe message failed!" << endl;
        return false;
    }
    //redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕
    int done =0;
    while(!done)
    {
        if(REDIS_ERR == redisBufferWrite(this->_subscriberContext, &done))
        {
            cerr << "unsubscribe message failed!" << endl;
            return false;
        }
    }

    return true;
}

// 在独立线程中接收订阅通道中的消息
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while(REDIS_OK == redisGetReply(this->_subscriberContext, (void **)&reply))
    {
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            //给业务层上报通道上发生的消息
            _notify_meaasge_handler(atoi(reply->element[1]->str), reply->element[2]->str);
        }
        freeReplyObject(reply);
    }
    cerr << "observer channel message quit!" << endl;
}
// 初始化向业务层上报通道消息的回调函数
void Redis::init_notify_handler(function<void(int, string)> fn)
{
    this->_notify_meaasge_handler = fn;
}
