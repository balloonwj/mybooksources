/**
 *@desc: AsyncLogger.cpp
 *@author: zhangyl
 *@date: 2018.11.10
 */
#include "stdafx.h"
#include <thread>
#include <mutex>
#include <list>
#include <string>
#include <sstream>
#include <iostream>

std::mutex log_mutex;
std::list<std::string> cached_logs;
FILE* log_file = NULL;

bool init_log_file()
{
    //以追加内容的形式写入文件内容，如果文件不存在，则创建
    log_file = fopen("my.log", "a+");
    return log_file != NULL;
}

void uninit_log_file()
{
    if (log_file != NULL)
        fclose(log_file);
}

bool write_log_tofile(const std::string& line)
{
    if (log_file == NULL)
        return false;

    if (fwrite((void*)line.c_str(), 1, line.length(), log_file) != line.length())
        return false;

    //将日志立即冲刷到文件中去
    fflush(log_file);

    return true;
}

void log_producer()
{
    int index = 0;
    while (true)
    {
        ++ index;
        std::ostringstream os;
        os << "This is log, index: " << index << ", producer threadID: " << std::this_thread::get_id() << "\n";
        //使用花括号括起来为的是减小锁的粒度
        {
            std::lock_guard<std::mutex> lock(log_mutex);
            cached_logs.emplace_back(os.str());
        }
           
        std::chrono::milliseconds duration(100);
        std::this_thread::sleep_for(duration);
    }
}

void log_consumer()
{
    std::string line;
    while (true)
    {
        //使用花括号括起来为的是减小锁的粒度
        {
            std::lock_guard<std::mutex> lock(log_mutex);
            if (!cached_logs.empty())
            {
                line = cached_logs.front();
                cached_logs.pop_front();
            }
        }

        if (line.empty())
        {        
            std::chrono::milliseconds duration(1000);
            std::this_thread::sleep_for(duration);

            continue;
        }

        write_log_tofile(line);

        line.clear();
    }
}

int main(int argc, char* argv[])
{
    if (!init_log_file())
    {
        std::cout << "init log file error." << std::endl;
        return -1;
    }
    
    std::thread log_producer1(log_producer);
    std::thread log_producer2(log_producer);
    std::thread log_producer3(log_producer);

    std::thread log_consumer1(log_consumer);
    std::thread log_consumer2(log_consumer);
    std::thread log_consumer3(log_consumer);

    log_producer1.join();
    log_producer2.join();
    log_producer3.join();

    log_consumer1.join();
    log_consumer2.join();
    log_consumer3.join();

    uninit_log_file();
    
    return 0;
}

