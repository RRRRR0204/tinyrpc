#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include "mprpccontroller.h"

#include <string>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>

// 所有通过stub代理对象调用的rpc方法，都在调用这里，统一做rpc调用方的数据序列化和网络发送
/*
    header_size + header_str(service_name method_name args_size) + args_str
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();    // service_name
    std::string method_name = method->name(); // method_name

    // 获取参数序列化后的字符串长度
    int args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    // 组织要发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4)); // header_size
    send_rpc_str += rpc_header_str;                               // rpc_header
    send_rpc_str += args_str;                                     // args

    // 打印调试信息
    std::cout << "===========================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===========================================" << std::endl;

    // 使用tcp编程，完成rpc方法的远程调用
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == client_fd)
    {
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error, errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 读取配置文件rpcserver的信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (-1 == connect(client_fd, (struct sockaddr *)&server_addr, sizeof server_addr))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error, errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 发送rpc请求
    if (-1 == send(client_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error, errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 接受rpc请求的响应值
    char recv_buf[1024] = {};
    int recv_size = 0;
    if (-1 == (recv_size = recv(client_fd, recv_buf, 1024, 0)))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error, errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc调用的相应数据
    // std::string response_str(recv_buf, 0, recv_size);   // 有bug，string构造的时候如果遇到\0，后面的数据没法保存
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        close(client_fd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error, response_str:%s", recv_buf);
        controller->SetFailed(errtxt);
        return;
    }

    close(client_fd);
}