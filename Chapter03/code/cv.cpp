#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <list>
#include <semaphore.h>
#include <iostream>

class Task
{
public:
	Task(int taskID)
	{
		this->taskID = taskID;
	}
	
	void doTask()
	{
		std::cout << "handle a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl; 
	}
	
private:
	int taskID;
};

pthread_mutex_t  mymutex;
std::list<Task*> tasks;
pthread_cond_t   mycv;

void* consumer_thread(void* param)
{	
	Task* pTask = NULL;
	while (true)
	{
		pthread_mutex_lock(&mymutex);
		while (tasks.empty())
		{				
			//如果获得了互斥锁，但是条件不合适的话，pthread_cond_wait会释放锁，不往下执行。
			//当发生变化后，条件合适，pthread_cond_wait将直接获得锁。
			pthread_cond_wait(&mycv, &mymutex);
		}
		
		pTask = tasks.front();
		tasks.pop_front();

		pthread_mutex_unlock(&mymutex);
		
		if (pTask == NULL)
			continue;

		pTask->doTask();
		delete pTask;
		pTask = NULL;		
	}
	
	return NULL;
}

void* producer_thread(void* param)
{
	int taskID = 0;
	Task* pTask = NULL;
	
	while (true)
	{
		pTask = new Task(taskID);
			
		pthread_mutex_lock(&mymutex);
		tasks.push_back(pTask);
		std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl; 
		
		pthread_mutex_unlock(&mymutex);
		
		//释放条件信号，通知消费者线程
		pthread_cond_signal(&mycv);
		
		taskID ++;

		//休眠1秒
		sleep(1);
	}
	
	return NULL;
}

int main()
{
	pthread_mutex_init(&mymutex, NULL);
	pthread_cond_init(&mycv, NULL);

	//创建5个消费者线程
	pthread_t consumerThreadID[5];
	for (int i = 0; i < 5; ++i)
	{
		pthread_create(&consumerThreadID[i], NULL, consumer_thread, NULL);
	}
	
	//创建一个生产者线程
	pthread_t producerThreadID;
	pthread_create(&producerThreadID, NULL, producer_thread, NULL);

	pthread_join(producerThreadID, NULL);
	
	for (int i = 0; i < 5; ++i)
	{
		pthread_join(consumerThreadID[i], NULL);
	}
	
	pthread_cond_destroy(&mycv);
	pthread_mutex_destroy(&mymutex);

	return 0;
}