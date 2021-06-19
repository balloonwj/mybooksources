/** 
 * std::map::try_emplace用法演示
 * zhangyl 2019.10.06
 */

#include <iostream>
#include <map>
#include <memory>

class ChatDialog
{
//其他实现省略...
public:
    ChatDialog()
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
};

//用于管理所有聊天对话框的map，key是好友id，value是ChatDialog是聊天对话框智能指针
std::map<int64_t, std::unique_ptr<ChatDialog>> m_ChatDialogs;

//C++ 17版本2
void onDoubleClickFriendItem3(int64_t userid)
{   
    //结构化绑定和try_emplace都是 C++17语法
    auto spChatDialog = std::make_unique<ChatDialog>();
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid, std::move(spChatDialog));
    iter->second->activate();
}

int main()
{
    //测试用例
    //906106643 是userid
    onDoubleClickFriendItem3(906106643L);
    //906106644 是userid
    onDoubleClickFriendItem3(906106644L);
    //906106643 是userid
    onDoubleClickFriendItem3(906106643L);

    return 0;
}

