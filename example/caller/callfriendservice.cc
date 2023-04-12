#include "mprpcapplication.h"
#include "friend.pb.h"
#include "mprpcchannel.h"

#include <iostream>

int main(int argc, char **argv)
{
    // 调用框架的初始化函数
    MprpcApplication::Init(argc, argv);

    // 调用远程发布的rpc方法Login
    rzl::FriendServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    rzl::GetFriendListRequest request1;
    request1.set_userid(1117);
    // rpc方法的响应
    rzl::GetFriendListResponse response1;
    // 发起rpc方法的调用 同步的rpc调用过程
    stub.GetFriendList(nullptr, &request1, &response1, nullptr); // RpcChannel->RpcChannel::callMethod 集中来做所有rpc方法调用的参数序列化和网络发送

    // 一次rpc调用完成，读结果
    if (0 == response1.result().errcode())
    {
        std::cout << "rpc GetFriendList response success!" << std::endl;
        int size = response1.friends_size();
        for (int i = 0; i < size; ++i)
        {
            std::cout << "index:" << (i + 1) << " name:" << response1.friends(i) << std::endl;
        }
    }
    else
    {
        std::cout << "rpc GetFriendList response error:" << response1.result().errmsg() << std::endl;
    }

    return 0;
}