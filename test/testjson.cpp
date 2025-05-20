#include "json.hpp"
using json =  nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
using namespace std;

string func1(){
    json js;
    //添加数组
    js["id"] = {1, 2, 3, 4, 5};
    //添加key-value
    js["name"] = "zhangsan";
    //添加对象
    //js["msg"]["zhang san"] = "hello world";
    //js["msg"]["li si"] = "hello china";

    js["msg"] = {{"zhang san", "hello world"},
                 {"li si", "hello china"}};
    string sendBuf = js.dump();
    return sendBuf;
}


void func2(){
    json js;
    js["id"] = {1, 2, 3, 4, 5};
    js["name"] = "zhangsan";
    js["msg"]["zhang san"] = "hello world";
    js["msg"]["li si"] = "hello china";

    string sendBuf = js.dump(); //json数据对象序列化为json字符串
    cout<<"js = "<<sendBuf.c_str()<<endl;
}

//json序列化示例
void func3(){
    json js;
    //直接序列化一个vector容器
    vector<int > vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);

    js["list"] = vec;
    //直接序列化一个map容器
    map<int,string> m;
    m.insert({1,"黄山"});
    m.insert({2,"天安门"});
    m.insert({3,"长城"});

    js["path"] = m;

    cout<<js;

}
int main(){
    string recvBuf = func1();

    //数据的反序列化 json字符串反序列化为数据对象
    json jsbuf = json::parse(recvBuf);
    cout<<jsbuf["msg"];
    return 0;
}