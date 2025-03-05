#include <iostream>
#include "mprpcapplication.h"
#include "../friend.pb.h"
int main(int argc,char **argv)
{
    MprpcApplication::Init(argc,argv);

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendsListRequest request;
    request.set_useid(1);

    fixbug::GetFriendsListResponse response;

    MprpcController controller;


    stub.GetFriendsList(&controller,&request,&response,nullptr);
    // 一次rpc调用结束
    if(controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if(0 == response.result().errcode())
        {
            std::cout << "rpc GetFriendsList response success: " << std::endl;
            for (int i = 0; i < response.friends_size();i++)
            {
                std::cout << response.friends()[i] << std::endl;
            }
            
        }
        else
        {
            std::cout << "rpc GetFriendsList response error: " << response.result().errmsg() << std::endl;
        }
    }
    
    


    return 0;
}