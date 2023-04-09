#pragma once

#include <unordered_map>
#include <string>

// 读取配置文件类，rpcserver_ip, rpcserver_port, zookeeper_ip, zookeeper_port
class MprpcConfig
{
public:
    // 解析加载配置文件
    void LoadConfigFile(const char *config_file);

    // 插叙配置信息
    std::string Load(const std::string &key);

    // 去掉字符串前后的空格
    void Trim(std::string &src_buf);

private:
    std::unordered_map<std::string, std::string> m_configMap;
};