#include <iostream>
#include <string>
#include "../user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

// 主要是在用框架
class UserService : public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name:" << name << ", pwd:" << pwd << std::endl;
        return true;
    }
    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service: Register" << std::endl;
        std::cout << "id:" << id << "name:" << name << ", pwd:" << pwd << std::endl;
        return true;
    }

    // 重写基类虚函数 框架直接调用
    void Login(::google::protobuf::RpcController *controller,
               const ::fixbug::LoginRequest *request,
               ::fixbug::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        bool login_result = Login(name,pwd);

        // 响应回去
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调
        done->Run();
    }
    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        // 
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 本地业务
        bool login_result = Register(id,name,pwd);

        // 响应回去
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        // 执行回调
        done->Run();
    }

};

int main(int argc,char ** argv)
{
    // 使用框架的初始化操作
    MprpcApplication::Init(argc,argv);
    // 
    RpcProvider provider;
    provider.NotifyService(new UserService());
    // 
    provider.Run();
    return 0;
}