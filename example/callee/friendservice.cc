#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

#include <iostream>
#include <string>
#include <vector>

class FriendService : public rzl::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendList(uint32_t userid)
    {
        std::cout << "do GetFriendList service! userid:" << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("axy1");
        vec.push_back("axy2");
        vec.push_back("axy3");
        return vec;
    }

    // 重写基类方法
    void GetFriendList(::google::protobuf::RpcController *controller,
                       const ::rzl::GetFriendListRequest *request,
                       ::rzl::GetFriendListResponse *response,
                       ::google::protobuf::Closure *done)
    {
        uint32_t userid = request->userid();
        std::vector<std::string> friendList = GetFriendList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (std::string &name : friendList)
        {
            std::string *p = response->add_friends();
            *p = name;
        }
        done->Run();
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把UserService发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点，进程进入阻塞状态，等待远程的rpc请求调用
    provider.Run();

    return 0;
}