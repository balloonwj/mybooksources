/** 
 * std::map::try_emplace用法演示
 * zhangyl 2019.10.06
 */

#include <iostream>
#include <map>

class ChatDialog
{
//其他实现省略...
public:
    ChatDialog(int64_t userid) : m_userid(userid)
    {
        std::cout << "ChatDialog constructor" << std::endl;
    }

    ~ChatDialog()
    {
        std::cout << "ChatDialog destructor" << std::endl;
    }

    void activate()
    {
        //实现省略
    }

private:
    int64_t     m_userid;
};

//用于管理所有聊天对话框的map，key是好友id，value是ChatDialog是聊天对话框智能指针
std::map<int64_t, ChatDialog>   m_ChatDialogs;

//C++ 17版本3
void onDoubleClickFriendItem3(int64_t userid)
{   
    //结构化绑定和try_emplace都是 C++17语法    
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid, userid);   
    iter->second.activate();
}

//int main()
//{
//    //测试用例
//    //906106643 是userid
//    onDoubleClickFriendItem3(906106643L);
//    //906106644 是userid
//    onDoubleClickFriendItem3(906106644L);
//    //906106643 是userid
//    onDoubleClickFriendItem3(906106643L);
//
//    return 0;
//}

int main()
{
    std::map<std::string, int> mapUsersAge{ { "Alex", 45 }, { "John", 25 } };
    mapUsersAge.insert_or_assign("Tom", 26);
    mapUsersAge.insert_or_assign("Alex", 27);

    for (const auto& [userName, userAge] : mapUsersAge)
    {
        std::cout << "userName: " << userName << ", userAge: " << userAge << std::endl;
    }
}

