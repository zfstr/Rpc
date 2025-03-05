#pragma once
#include <unordered_map>
#include <string>
// 框架读取配置文件类
class MprpcConfig
{
public:
    void LoadConfigFile(const char *config);
    // 查询配置项信息
    std::string Load(const std::string &key);
private:
    std::unordered_map<std::string,std::string> m_configMap;
    void Trim(std::string &src_buf);
};