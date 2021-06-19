/**
 *@desc: AsyncLogger.cpp，linux版本
 *@author: zhangyl
 *@date: 2018.11.10
 */
#include <unistd.h>
#include <list>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <iostream>
#include <sstream>


std::list<std::string> cached_logs;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t           log_semphore;
FILE* plogfile = NULL;

bool init()
{
    pthread_mutex_init(&log_mutex, NULL);
    //初始信号量资源数目是0
    sem_init(&log_semphore, 0, 0);
 
    //如果文件不存在，则创建
    plogfile = fopen("my.log", "a++");

    return plogfile != NULL;
}

void uninit()
{
    pthread_mutex_destroy(&log_mutex);
    sem_destroy(&log_semphore);
       
    if (plogfile != NULL)
        fclose(plogfile);
}

bool write_log_to_file(const std::string& line)
{
    if (plogfile == NULL)
        return false;

    //如果对于比较长的日志，应该分段写入，因为单次写入可能只能写入部分，这里为了演示方便，逻辑从简
    if (fwrite((void*)line.c_str(), 1, line.length(), plogfile) != line.length())
        return false;
    
    //将日志立即冲刷到文件中去
    fflush(plogfile);

    return true;
}

void* producer_thread_proc(void* arg)
{
    int index = 0;
    while (true)
    {
        ++ index;
        std::ostringstream os;
        os << "This is log, index: " << index << ", producer threadID: " << pthread_self() << "\n";
      
        pthread_mutex_lock(&log_mutex);
        cached_logs.push_back(os.str());
        pthread_mutex_unlock(&log_mutex);

        sem_post(&log_semphore);
                   
        usleep(100000);
    }
}

void* consumer_thread_proc(void* arg)
{
    std::string line;
    while (true)
    {     
        //无限等待
        sem_wait(&log_semphore);

        pthread_mutex_lock(&log_mutex);
        if (!cached_logs.empty())
        {
            line = cached_logs.front();
            cached_logs.pop_front();
        }     
        pthread_mutex_unlock(&log_mutex);

        if (line.empty())
        {        
            sleep(1);

            continue;
        }

        write_log_to_file(line);

        line.clear();
    }
}

int main(int argc, char* argv[])
{
    if (!init())
    {
        std::cout << "init log file error." << std::endl;
        return -1;
    }
    
    pthread_t producer_thread_id[3];
    for (int i = 0; i < sizeof(producer_thread_id) / sizeof(producer_thread_id[0]); ++i)
    {
        pthread_create(&producer_thread_id[i], NULL, producer_thread_proc, NULL);
    }

    pthread_t consumer_thread_id[3];
    for (int i = 0; i < sizeof(consumer_thread_id) / sizeof(consumer_thread_id[0]); ++i)
    {
        pthread_create(&consumer_thread_id[i], NULL, consumer_thread_proc, NULL);
    }

    //等待消费者线程退出
    for (int i = 0; i < sizeof(producer_thread_id) / sizeof(producer_thread_id[0]); ++i)
    {
        pthread_join(producer_thread_id[i], NULL);
    }

    //等待生产者线程退出
    for (int i = 0; i < sizeof(consumer_thread_id) / sizeof(consumer_thread_id[0]); ++i)
    {
        pthread_join(consumer_thread_id[i], NULL);
    }

    uninit();
    
    return 0;
}

