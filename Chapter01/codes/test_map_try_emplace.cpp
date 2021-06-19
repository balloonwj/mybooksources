// test_map_try_emplace.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>

class ChatDialog
{
//其他实现省略...
public:
    void activate()
    {
        //实现省略
    }
};

//用于管理所有聊天对话框的map，key是好友id，ChatDialog是聊天对话框指针
std::map<int64_t, ChatDialog*> m_ChatDialogs;

//普通版本
void onDoubleClickFriendItem(int64_t userid)
{
    auto targetChatDialog = m_ChatDialogs.find(userid);
    //好友对话框不存在，则创建之，并激活
    if (targetChatDialog == m_ChatDialogs.end())
    {
        ChatDialog* pChatDialog = new ChatDialog();
        m_ChatDialogs.insert(std::pair<int64_t, ChatDialog*>(userid, pChatDialog));
        pChatDialog->activate();
    }
    //好友对话框存在，直接激活
    else
    {
        targetChatDialog->second->activate();
    }
}

//C++ 17版本1
void onDoubleClickFriendItem2(int64_t userid)
{   
    //结构化绑定和try_emplace都是 C++17语法
    auto [iter, inserted] = m_ChatDialogs.try_emplace(userid);
    if (inserted)
        iter->second = new ChatDialog();   

    iter->second->activate();
}

int main()
{
    //测试用例
    //906106643 是userid
    onDoubleClickFriendItem2(906106643L);
    //906106644 是userid
    onDoubleClickFriendItem2(906106644L);
    //906106643 是userid
    onDoubleClickFriendItem2(906106643L);

    return 0;
}

