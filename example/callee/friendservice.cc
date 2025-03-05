#include <iostream>
#include <string>
#include "../friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include <vector>
#include "logger.h"
class FriendService : public fixbug::FriendServiceRpc
{
    std::vector<std::string> GetFriendsList(uint32_t userid)
    {
        std::cout << "do GetFriendsList service, userid: " << userid << std::endl;
        std::vector<std::string> vec;
        vec.push_back("gao yao");
        vec.push_back("yi xiao chuan");
        vec.push_back("ying zheng");
        return vec;
    }
    void GetFriendsList(::google::protobuf::RpcController* controller,
                         const ::fixbug::GetFriendsListRequest* request,
                         ::fixbug::GetFriendsListResponse* response,
                         ::google::protobuf::Closure* done)
    {
        uint32_t userid = request->useid();

        std::vector<std::string> friendslist = GetFriendsList(userid);

        fixbug::ResultCode * result = response->mutable_result();
        result->set_errcode(0);
        result->set_errmsg("");
        for(std::string &name : friendslist)
        {
            std::string *p = response->add_friends();
            *p = name;
        }

        done->Run();
    }
};



int main(int argc,char ** argv)
{
    // 使用框架的初始化操作
    LOG_INFO("first log message!");
    LOG_ERROR("%s:%s:%d",__FILE__,__FUNCTION__,__LINE__);
    MprpcApplication::Init(argc,argv);
    // 
    RpcProvider provider;
    provider.NotifyService(new FriendService());
    // 
    provider.Run();
    return 0;
}