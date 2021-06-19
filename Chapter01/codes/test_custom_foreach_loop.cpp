#include <iostream>
#include <string>

template<typename T, size_t N>
class A
{
public:
    A()
    {
        for (size_t i = 0; i < N; ++i)
        {
            m_elements[i] = i;
        }
    }

    ~A()
    {

    }

    T* begin()
    {
        return m_elements + 0;
    }

    T* end()
    {
        return m_elements + N;
    }

private:
    T       m_elements[N];
};

int main()
{
    A<int, 10> a;
    for (auto iter : a)
    {
        std::cout << iter << std::endl;
    }
    return 0;
}

