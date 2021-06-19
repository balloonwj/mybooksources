#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <memory>

std::mutex 				mymutex;
std::condition_variable mycv;
bool success = false;

void thread_func()
{
    {
        std::unique_lock<std::mutex> lock(mymutex);
        success = true;
        mycv.notify_all();
    }

    //实际的线程执行的工作代码放在下面
    //这里为了模拟方便，简单地写个死循环
    while (true)
    {

    }
}

int main()
{
    std::vector<std::shared_ptr<std::thread>> threads;
    
    for (int i = 0; i < 5; ++i)
    {
        std::shared_ptr<std::thread> spthread;
        spthread.reset(new std::thread(thread_func));

        //使用花括号减小锁的粒度
        {
            std::unique_lock<std::mutex> lock(mymutex);
            while (!success)
            {
                mycv.wait(lock);
            }
        }

        std::cout << "start thread successfully， index: " << i << std::endl;

        threads.push_back(spthread);
    }

    for (auto& iter : threads)
    {
        iter->join();
    }

    return 0;
}