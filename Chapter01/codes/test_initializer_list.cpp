#include <iostream>
#include <initializer_list>
#include <vector>

class A
{
public:
    A(std::initializer_list<int> integers)
    {
        m_vecIntegers.insert(m_vecIntegers.end(), integers.begin(), integers.end());
    }

    ~A()
    {

    }

    void append(std::initializer_list<int> integers)
    {
        m_vecIntegers.insert(m_vecIntegers.end(), integers.begin(), integers.end());
    }

    void print()
    {
        size_t size = m_vecIntegers.size();
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << m_vecIntegers[i] << std::endl;
        }
    }

private:
    std::vector<int> m_vecIntegers;
};

int main()
{
    A a{ 1, 2, 3 };
    a.print();

    std::cout << "After appending..." << std::endl;

    a.append({ 4, 5, 6 });
    a.print();

    return 0;
}

