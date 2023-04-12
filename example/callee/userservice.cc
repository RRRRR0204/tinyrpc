#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

#include <iostream>
#include <string>

/**
 * UserService是一个本地服务，提供两个进程的方法，Login和GetFriendLists
 */
class UserService : public rzl::UserServiceRpc // 使用在rpc服务发布端（服务提供者）
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id: " << id << " name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

    /**
     * 重写基类UserServiceRpc的虚函数，提供给框架调用
     * 1. caller ==> Login(LoginRequest) ==> muduo ==> callee
     * 2. callee ==> Login(LoginRequest) ==> 交到下面重写的这个Login方法上
     */
    void Login(::google::protobuf::RpcController *controller,
               const ::rzl::LoginRequest *request,
               ::rzl::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 框架给业务传入了请求参数request，应用获取相应数据做本地业务即可
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        bool login_result = Login(name, pwd);

        // 把相应写入（错误码，错误消息，返回值）
        rzl::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调操作，相应对象的序列化和网络发送（都是由框架完成）
        done->Run();
    }

    void Register(::google::protobuf::RpcController *controller,
                  const ::rzl::RegisterRequest *request,
                  ::rzl::RegisterResponse *response,
                  ::google::protobuf::Closure *done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_result = Register(id, name, pwd);

        rzl::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        done->Run();        
    }
};

int main(int argc, char **argv)
{
    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // provider是一个rpc网络服务对象，把UserService发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());

    // 启动一个rpc服务发布节点，进程进入阻塞状态，等待远程的rpc请求调用
    provider.Run();

    return 0;
}