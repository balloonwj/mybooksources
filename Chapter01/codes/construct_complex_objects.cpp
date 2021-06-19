// construct_complex_objects.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <initializer_list>
#include <vector>
#include <sstream>

//简单地模拟 json 支持的几种数据类型
enum class jsonType
{
    jsonTypeNull,
    jsonTypeInt,
    jsonTypeLong,
    jsonTypeDouble,
    jsonTypeBool,
    jsonTypeString,
    jsonTypeArray,
    jsonTypeObject
};

struct jsonNode
{
    jsonNode(const char* key, const char* value) : 
        m_type(jsonType::jsonTypeString),
        m_key(key),
        m_value(value)
    { 
        std::cout << "jsonNode contructor1 called." << std::endl;
    }

    jsonNode(const char* key, double value) :
        m_type(jsonType::jsonTypeString),
        m_key(key),
        m_value(std::to_string(value))
    {
        std::cout << "jsonNode contructor2 called." << std::endl;
    }

    jsonType    m_type;
    std::string m_key;
    std::string m_value;
};

class json
{
public:
    static json& array(std::initializer_list<jsonNode> nodes)
    {            
        m_json.m_nodes.clear();
        m_json.m_nodes.insert(m_json.m_nodes.end(), nodes.begin(), nodes.end());

        std::cout << "json::array() called." << std::endl;

        return m_json;
    }

    json()
    {

    }

    ~json()
    {

    }

    std::string toString()
    {
        std::ostringstream os;
        size_t size = m_nodes.size();
        for (size_t i = 0; i < size; ++i)
        {
            switch (m_nodes[i].m_type)
            {
            //根据类型，组装成一个json字符串，代码省略...
            case jsonType::jsonTypeDouble:
                break;
            }
        }
    }

private:
    std::vector<jsonNode> m_nodes;

    static json           m_json;
};

json json::m_json;

int main()
{
    json array_not_object = json::array({ {"currency", "USD"}, {"value", 42.99} });

    return 0;
}

