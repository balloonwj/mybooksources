// test_std_enable_shared_from_this.cpp
#include <iostream>
#include <memory>

class A : public std::enable_shared_from_this<A>
{
public:
    A()
    {
        std::cout << "A constructor" << std::endl;
    }

    ~A()
    {
        std::cout << "A destructor" << std::endl;
    }

    std::shared_ptr<A> getSelf()
    {
        return shared_from_this();
    }
};


int main()
{
    std::shared_ptr<A> sp1(new A());

    std::shared_ptr<A> sp2 = sp1->getSelf();

    std::cout << "use count: " << sp1.use_count() << std::endl;

    return 0;
}
