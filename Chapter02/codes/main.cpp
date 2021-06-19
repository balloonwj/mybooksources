#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

long g = 0;

void* worker_thread_1(void* p)
{
	while (true)
	{
		g = 100;
		printf("worker_thread_1\n");
		usleep(300000);
	}

	return NULL;
}

void* worker_thread_2(void* p)
{
	while (true)
	{
		g = -100;
		printf("worker_thread_2\n");
		usleep(500000);

	}

	return NULL;
}

int main()
{
	pthread_t thread_id_1;
	pthread_create(&thread_id_1, NULL, worker_thread_1, NULL); 
	pthread_t thread_id_2;
	pthread_create(&thread_id_2, NULL, worker_thread_2, NULL);  

	while (true)
	{
		g = -1;
		printf("g=%d\n", g);
		g = -2;
		printf("g=%d\n", g);
		g = -3;
		printf("g=%d\n", g);
		g = -4;
		printf("g=%d\n", g);

		usleep(1000000);
	}

	return 0;
}