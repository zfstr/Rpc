#pragma once
#include "google/protobuf/service.h"
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <string>
#include <functional>
#include <google/protobuf/descriptor.h>
#include <unordered_map>
// #include <muduo/base/Timestamp.h>
class RpcProvider
{
public:
    // 这里是框架给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);
    void Run();

private:

    // 组合了TcpServer
    // std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr;
    muduo::net::EventLoop m_eventLoop;
    
    // 服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;  // 保存服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    // 新连接的回调
    void OnConnection(const muduo::net::TcpConnectionPtr&);
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*,  muduo::Timestamp);
    // Closure 回调操作
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);
};