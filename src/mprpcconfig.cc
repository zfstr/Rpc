#include "mprpcconfig.h"
#include <iostream>
#include <string>
#include "logger.h"
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file,"r");
    if(nullptr == pf)
    {
        // std::cout << config_file << "is not exist" << std::endl;
        LOG_ERROR("%s is not exist",config_file)
        exit(EXIT_FAILURE);
    }
    // 注释项
    // 正确配置项
    // 去开头空格
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf,512,pf);
        // 去前面空格
        std::string read_buf(buf);
        Trim(read_buf);
        if(read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }
        int idx = read_buf.find('=');
        if(idx == -1)
        {
            // 配置不合法
            continue;
        }
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        int endidx = read_buf.find('\n',idx);
        value = read_buf.substr(idx + 1, endidx - idx - 1);
        Trim(value);
        m_configMap.insert({key,value});
    }
    
}


void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if(idx!=-1)
    {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    // 去后面空格
    idx = src_buf.find_last_not_of(' ');
    if(idx!=-1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}

    // 查询配置项信息
std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}