// test_auto_ptr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>

int main()
{
    //测试拷贝构造
    std::auto_ptr<int> sp1(new int(8));
    std::auto_ptr<int> sp2(sp1);
    if (sp1.get() != NULL)
    {
        std::cout << "sp1 is not empty." << std::endl;
    }
    else
    {
        std::cout << "sp1 is empty." << std::endl;
    }

    if (sp2.get() != NULL)
    {
        std::cout << "sp2 is not empty." << std::endl;
    }
    else
    {
        std::cout << "sp2 is empty." << std::endl;
    }

    //测试 operator= 构造
    std::auto_ptr<int> sp3(new int(8));
    std::auto_ptr<int> sp4;
    sp4 = sp3;
    if (sp3.get() != NULL)
    {
        std::cout << "sp3 is not empty." << std::endl;
    }
    else
    {
        std::cout << "sp3 is empty." << std::endl;
    }

    if (sp4.get() != NULL)
    {
        std::cout << "sp4 is not empty." << std::endl;
    }
    else
    {
        std::cout << "sp4 is empty." << std::endl;
    }

    return 0;
}
