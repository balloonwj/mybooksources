#include <pthread.h>
#include <iostream>
#include <unistd.h>

//线程局部存储key
__thread int g_mydata = 99;

void* thread_function1(void* args)
{
	while (true)
	{
		g_mydata ++;
	}
	
	return NULL;
} 

void* thread_function2(void* args)
{
	while (true)
	{		
		std::cout << "g_mydata = " << g_mydata << ", ThreadID: " << pthread_self() << std::endl;
		sleep(1);
	}
	
	return NULL;
} 

int main()
{
	pthread_t threadIDs[2];	
	pthread_create(&threadIDs[0], NULL, thread_function1, NULL);
	pthread_create(&threadIDs[1], NULL, thread_function2, NULL);
	
	for(int i = 0; i < 2; ++i)
	{
		pthread_join(threadIDs[i], NULL);
	}
	
	return 0;
}