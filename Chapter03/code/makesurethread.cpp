#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>

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
	std::thread t(thread_func);
	
	//使用花括号减小锁的粒度
	{
		std::unique_lock<std::mutex> lock(mymutex);
		while (!success)
		{
			mycv.wait(lock);
		}
	}
	
	std::cout << "start thread successfully." << std::endl; 
	
	t.join();

	return 0;
}