// test_unique_ptr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>

int main()
{
    //创建10个int类型的堆对象
    //形式1
    std::unique_ptr<int[]> sp1(new int[10]);
    
    //形式2
    std::unique_ptr<int[]> sp2;
    sp2.reset(new int[10]);
    //形式3
    std::unique_ptr<int[]> sp3(std::make_unique<int[]>(10));

    for (int i = 0; i < 10; ++i)
    {
        sp1[i] = i;
        sp2[i] = i;
        sp3[i] = i;
    }

    for (int i = 0; i < 10; ++i)
    {
        std::cout << sp1[i] << ", " << sp2[i] << ", " << sp3[i] << std::endl;
    }

    return 0;
}
