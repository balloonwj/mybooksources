//test_destroy_locked_mutex.cpp
#include <pthread.h>
#include <stdio.h>
#include <errno.h>

int main()
{
	pthread_mutex_t mymutex;
	pthread_mutex_init(&mymutex, NULL);
	int ret = pthread_mutex_lock(&mymutex);
	
	//尝试对被锁定的mutex对象进行销毁
	ret = pthread_mutex_destroy(&mymutex);
	if (ret != 0)
	{
		if (ret == EBUSY)
			printf("EBUSY\n");
		printf("Failed to destroy mutex.\n");
	}

	ret = pthread_mutex_unlock(&mymutex);
	//尝试对已经解锁的mutex对象进行销毁
	ret = pthread_mutex_destroy(&mymutex);
	if (ret == 0)
		printf("Succeeded to destroy mutex.\n");

	return 0;
}