#include <iostream>
#include "mprpcapplication.h"
#include "../user.pb.h"
#include "mprpcchannel.h"
int main(int argc,char **argv)
{
    MprpcApplication::Init(argc,argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;

    stub.Login(nullptr,&request,&response,nullptr);
    // 一次rpc调用结束

    if(0 == response.result().errcode())
    {
        std::cout << "rpc login response: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    fixbug::RegisterRequest request_r;
    request_r.set_id(1);
    request_r.set_name("zhang san");
    request_r.set_pwd("123456");

    fixbug::RegisterResponse response_r;

    stub.Register(nullptr,&request_r,&response_r,nullptr);
    // 一次rpc调用结束

    if(0 == response_r.result().errcode())
    {
        std::cout << "rpc Register response: " << response_r.success() << std::endl;
    }
    else
    {
        std::cout << "rpc Register response error: " << response_r.result().errmsg() << std::endl;
    }


    return 0;
}