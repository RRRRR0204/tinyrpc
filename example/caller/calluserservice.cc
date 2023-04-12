#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

#include <iostream>

int main(int argc, char **argv)
{
    // 调用框架的初始化函数
    MprpcApplication::Init(argc, argv);

    // 调用远程发布的rpc方法Login
    rzl::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    rzl::LoginRequest request1;
    request1.set_name("zhang san");
    request1.set_pwd("123456");
    // rpc方法的响应
    rzl::LoginResponse response1;
    // 发起rpc方法的调用 同步的rpc调用过程
    stub.Login(nullptr, &request1, &response1, nullptr); // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读结果
    if (0 == response1.result().errcode())
    {
        std::cout << "rpc login response success:" << response1.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error:" << response1.result().errmsg() << std::endl;
    }

    rzl::RegisterRequest request2;
    request2.set_id(1117);
    request2.set_name("rzl");
    request2.set_pwd("0000");
    rzl::RegisterResponse response2;

    stub.Register(nullptr, &request2, &response2, nullptr);
    if (0 == response2.result().errcode())
    {
        std::cout << "rpc register response success:" << response2.success() << std::endl;
    }
    else
    {
        std::cout << "rpc register response error:" << response2.result().errmsg() << std::endl;
    }

    return 0;
}