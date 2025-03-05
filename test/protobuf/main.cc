#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;
using namespace std;
int main1()
{
    LoginRequest req;
    req.set_name("张三");
    req.set_pwd("123456");
    string send_str;
    if(req.SerializeToString(&send_str))
    {
        cout << send_str << endl;
    }
    
    LoginRequest reqB;
    if(reqB.ParseFromString(send_str))
    {
        cout << reqB.name() << endl;
        cout << reqB.pwd() << endl;
    }

    return 0;
}

int main()
{
    // LoginResponse rsp;
    // ResultCode *rc = rsp.mutable_result();
    // rc->set_errcode(1);
    // rc->set_errmsg("登录处理失败");
    GetFriendListsResponse rsp;
    // 对象操作
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(0);
    rc->set_errmsg("请求成功");
    
    
    // 列表操作
    User* user1 = rsp.add_friend_list();
    user1->set_name("张三");
    user1->set_age(18);
    user1->set_sex(User::MAN);

    User* user2 = rsp.add_friend_list();
    user2->set_name("李四");
    user2->set_age(23);
    user2->set_sex(User::WOMAN);

    cout << rsp.friend_list_size() << endl;

    cout << rsp.result().errcode() << endl;
    cout << rsp.result().errmsg() << endl;

    cout << rsp.friend_list()[0].name() << endl;
    cout << rsp.friend_list()[0].age() << endl;
    cout << rsp.friend_list()[0].sex() << endl;


    cout << rsp.friend_list()[1].name() << endl;
    cout << rsp.friend_list()[1].age() << endl;
    cout << rsp.friend_list()[1].sex() << endl;



    return 0;
}
