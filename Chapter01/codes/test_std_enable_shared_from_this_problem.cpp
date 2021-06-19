// test_std_enable_shared_from_this.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <memory>

class A : public std::enable_shared_from_this<A>
{
public:
    A()
    {
        m_i = 9;
        //注意:
        //比较好的做法是在构造函数里面调用shared_from_this()给m_SelfPtr赋值
        //但是很遗憾不能这么做,如果写在构造函数里面程序会直接崩溃

        std::cout << "A constructor" << std::endl;
    }

    ~A()
    {
        m_i = 0;

        std::cout << "A destructor" << std::endl;
    }

    void func()
    {
        m_SelfPtr = shared_from_this();
    }

public:
    int                 m_i;
    std::shared_ptr<A>  m_SelfPtr;

};

int main()
{
    {
        std::shared_ptr<A> spa(new A());
        spa->func();
    }

    return 0;
}