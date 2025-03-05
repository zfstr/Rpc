#include "rpcprovider.h"
#include <string>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include <iostream>
#include "logger.h"
#include "zookeeperutil.h"

// 这里是框架给外部使用的，可以发布rpc方法的函数接口
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    service_info.m_service = service;
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    // 获取服务的名字
    std::string service_name = pserviceDesc->name();
    // 获取服务的方法数量
    int methodCnt = pserviceDesc->method_count();

    // std::cout << "service_name: " << service_name << std::endl;
    LOG_INFO("service_name: %s",service_name.c_str());
    for(int i = 0;i < methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor * pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        // std::cout << "method_name: " << method_name << std::endl;
        LOG_INFO("method_name: %s",method_name.c_str());
        service_info.m_methodMap.insert({method_name, pmethodDesc});
    }
    m_serviceMap.insert({service_name, service_info});
    
}
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    muduo::net::InetAddress address(ip,port);
    // 创建muduo库的TcpServer对象
    muduo::net::TcpServer tcpserver(&m_eventLoop, address,"RpcProvider");
    // 绑定连接回调和消息读写回调 muduo库可以分离网络代码和业务代码
    tcpserver.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    tcpserver.setMessageCallback(std::bind(&RpcProvider::OnMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    // 设置muduo库的线程数量
    tcpserver.setThreadNum(4);

    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : m_serviceMap) 
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    std::cout << "RpcProvider start service at ip: " << ip << " port: " << port << std::endl;
    // 启动网络服务
    tcpserver.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
    {
        // 连接断开
        conn->shutdown();
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer * buffer, muduo::Timestamp timeout)
{
    // buffer->retrieveAllAsString() 应该包含servicename methodname args
    std::string recv_buf = buffer->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy((char *)&header_size, 4, 0);
    //根据header_size读取数据的原始字符流
    std::string rpc_header_str = recv_buf.substr(4,header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    if(rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        // 数据头反序列化失败
        // std::cout << "rpc_header_str: " << rpc_header_str << " parse error!" << std::endl;
        LOG_ERROR("rpc_header_str: %s parse error!",rpc_header_str.c_str());
        return;
    }
    std::string args_str = recv_buf.substr(header_size + 4, args_size);
    // std::cout << "=============================================" << std::endl;
    // std::cout << "header_size: "<< header_size << std::endl;
    // std::cout << "rpc_header_str: "<< rpc_header_str << std::endl;
    // std::cout << "service_name: "<< service_name << std::endl;
    // std::cout << "method_name: "<< method_name << std::endl;
    // std::cout << "args_str: "<< args_str << std::endl;
    // std::cout << "=============================================" << std::endl;

    auto it = m_serviceMap.find(service_name);
    if(it == m_serviceMap.end())
    {
        // std::cout << service_name << " is not exist!" << std::endl;
        LOG_ERROR("%s is not exist!",service_name.c_str());
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if(mit == it->second.m_methodMap.end())
    {
        // std::cout << service_name << " : "<< method_name << " is not exist!" << std::endl;
        LOG_ERROR("%s : %s is not exist!",service_name.c_str(),method_name.c_str());
        return;
    }

    google::protobuf::Service *service = it->second.m_service;  // 这个就是new 的Service对象
    const google::protobuf::MethodDescriptor *method = mit->second; 
    // 生成rpc方法调用的请求和响应参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
       
        LOG_ERROR("request parse error, content:  %s ",args_str.c_str());
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    // Closure 
    google::protobuf::Closure *done = 
    google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);

    // 在当前节点调用方法请求
    service->CallMethod(method,nullptr,request,response,done);
}

// Closure 回调操作
void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn , google::protobuf::Message* message)
{
    std::string response_str;
    if(message->SerializeToString(&response_str))
    {
        // 序列化成功后
        conn->send(response_str);
    }
    else
    {
        
        LOG_ERROR("serialize response_str error");
    }
    conn->shutdown();
}