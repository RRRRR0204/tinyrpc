#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的基础类，负责一些初始化操作
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);

    static MprpcApplication &GetInstance();

    static MprpcConfig &GetConfig();

private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;

    static MprpcConfig m_config;
};