/**
 * 任务池模型，TaskPool.cpp
 * zhangyl 2019.02.14
 */
#include "TaskPool.h"

TaskPool::TaskPool() : m_bRunning(false)
{

}

TaskPool::~TaskPool()
{
    removeAllTasks();
}

void TaskPool::init(int threadNum/* = 5*/)
{
    if (threadNum <= 0)
        threadNum = 5;

    m_bRunning = true;

    for (int i = 0; i < threadNum; ++i)
    {
        std::shared_ptr<std::thread> spThread;
        spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this)));
        m_threads.push_back(spThread);
    }
}

void TaskPool::threadFunc()
{
    std::shared_ptr<Task> spTask;
    while (true)
    {
        std::unique_lock<std::mutex> guard(m_mutexList);
        while (m_taskList.empty())
        {                 
            if (!m_bRunning)
                break;
            
            //如果获得了互斥锁，但是条件不合适的话，pthread_cond_wait会释放锁，不往下执行。
            //当发生变化后，条件合适，pthread_cond_wait将直接获得锁。
            m_cv.wait(guard);
        }

        if (!m_bRunning)
            break;

        spTask = m_taskList.front();
        m_taskList.pop_front();

        if (spTask == NULL)
            continue;

        spTask->doIt();
        spTask.reset();
    }

    std::cout << "exit thread, threadID: " << std::this_thread::get_id() << std::endl;
}

void TaskPool::stop()
{
    m_bRunning = false;
    m_cv.notify_all();

    //等待所有线程退出
    for (auto& iter : m_threads)
    {
        if (iter->joinable())
            iter->join();
    }
}

void TaskPool::addTask(Task* task)
{
    std::shared_ptr<Task> spTask;
    spTask.reset(task);

    {
        std::lock_guard<std::mutex> guard(m_mutexList);       
        //m_taskList.push_back(std::make_shared<Task>(task));
        m_taskList.push_back(spTask);
        std::cout << "add a Task." << std::endl;
    }
    
    m_cv.notify_one();
}

void TaskPool::removeAllTasks()
{
    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        for (auto& iter : m_taskList)
        {
            iter.reset();
        }
        m_taskList.clear();
    }
}