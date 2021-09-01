/**
 * std::shared_mutex与std::mutex的性能对比
 * zhangyl 2016.11.10
 */

 //读线程数量
#define READER_THREAD_COUNT  8
//最大循环次数
#define LOOP_COUNT           5000000

#include <iostream>
#include <mutex>  
#include <shared_mutex>
#include <thread>

class shared_mutex_counter
{
public:
	shared_mutex_counter() = default;
	~shared_mutex_counter() = default;

	//使用std::shared_mutex，同一时刻多个读线程可以同时访问value_值
	unsigned int get() const
	{
		//注意：这里使用std::shared_lock
		std::shared_lock<std::shared_mutex> lock(mutex_);
		return value_;
	}

	//使用std::shared_mutex，同一个时刻仅有一个写线程可以修改value_值
	void increment()
	{
		//注意：这里使用std::unique_lock
		std::unique_lock<std::shared_mutex> lock(mutex_);
		value_++;
	}

	//使用std::shared_mutex，同一个时刻仅有一个写线程可以重置value_值
	void reset()
	{
		//注意：这里使用std::unique_lock
		std::unique_lock<std::shared_mutex> lock(mutex_);
		value_ = 0;
	}

private:
	mutable std::shared_mutex   mutex_;
	//value_是多个线程的共享资源
	unsigned int                value_ = 0;
};

class mutex_counter
{
public:
	mutex_counter() = default;
	~mutex_counter() = default;

	//使用std::mutex，同一时刻仅有一个线程可以访问value_的值
	unsigned int get() const
	{
		std::unique_lock<std::mutex> lk(mutex_);
		return value_;
	}

	//使用std::mutex，同一时刻仅有一个线程可以修改value_的值
	void increment()
	{
		std::unique_lock<std::mutex> lk(mutex_);
		value_++;
	}

private:
	mutable std::mutex      mutex_;
	//value_是多个线程的共享资源
	unsigned int            value_ = 0;
};

//测试std::shared_mutex
void test_shared_mutex()
{
	shared_mutex_counter counter;
	int temp;

	//写线程函数
	auto writer = [&counter]() {
		for (int i = 0; i < LOOP_COUNT; i++)
		{
			counter.increment();
		}
	};

	//读线程函数
	auto reader = [&counter, &temp]() {
		for (int i = 0; i < LOOP_COUNT; i++)
		{
			temp = counter.get();
		}
	};

	//存放读线程对象指针的数组
	std::thread** tarray = new std::thread * [READER_THREAD_COUNT];

	//记录起始时间
	clock_t start = clock();

	//创建READER_THREAD_COUNT个读线程
	for (int i = 0; i < READER_THREAD_COUNT; i++)
	{
		tarray[i] = new std::thread(reader);
	}

	//创建一个写线程
	std::thread tw(writer);

	for (int i = 0; i < READER_THREAD_COUNT; i++)
	{
		tarray[i]->join();
	}
	tw.join();

	//记录起始时间
	clock_t end = clock();
	printf("[test_shared_mutex]\n");
	printf("thread count: %d\n", READER_THREAD_COUNT);
	printf("result: %d cost: %dms temp: %d \n", counter.get(), end - start, temp);
}

//测试std::mutex
void test_mutex()
{
	mutex_counter counter;

	int temp;

	//写线程函数
	auto writer = [&counter]() {
		for (int i = 0; i < LOOP_COUNT; i++)
		{
			counter.increment();
		}
	};

	//读线程函数
	auto reader = [&counter, &temp]() {
		for (int i = 0; i < LOOP_COUNT; i++)
		{
			temp = counter.get();
		}
	};

	//存放读线程对象指针的数组
	std::thread** tarray = new std::thread * [READER_THREAD_COUNT];

	//记录起始时间
	clock_t start = clock();

	//创建READER_THREAD_COUNT个读线程
	for (int i = 0; i < READER_THREAD_COUNT; i++)
	{
		tarray[i] = new std::thread(reader);
	}

	//创建一个写线程
	std::thread tw(writer);

	for (int i = 0; i < READER_THREAD_COUNT; i++)
	{
		tarray[i]->join();
	}
	tw.join();

	//记录结束时间
	clock_t end = clock();
	printf("[test_mutex]\n");
	printf("thread count:%d\n", READER_THREAD_COUNT);
	printf("result:%d cost:%dms temp:%d \n", counter.get(), end - start, temp);
}

int main()
{
	//为了排除测试程序的无关因素，测试时只开启一个  
	test_mutex();
	//test_shared_mutex();
	return 0;
}