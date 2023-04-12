#pragma once

#include <string>
#include <functional>
#include <unordered_map>

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/Timestamp.h>

#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>

// 框架提供的专门负责发布rpc服务的网络对象类，RpcServer
class RpcProvider
{
public:
    // 框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    // EventLoop
    muduo::net::EventLoop m_eventLoop;

    // service服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *m_service;                                                    // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> m_methodMap; // 保存服务方法
    };
    // 存储注册成功的服务对象和其服务
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;

    // 新连接回调
    void onConnection(const muduo::net::TcpConnectionPtr &);
    // 已建立连接的读写事件回调
    void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);

    // Closuce的回调操作，用于序列化rpc的相应和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);
};