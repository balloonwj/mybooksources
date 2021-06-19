/** 
 * 演示 Windows 条件变量的使用
 * zhangyl 20191111
 */

#include <Windows.h>
#include <iostream>
#include <list>

class Task
{
public:
    Task(int taskID)
    {
        this->taskID = taskID;
    }

    void doTask()
    {
        std::cout << "handle a task, taskID: " << taskID << ", threadID: " << GetCurrentThreadId() << std::endl;
    }

private:
    int taskID;
};

CRITICAL_SECTION    myCriticalSection;
CONDITION_VARIABLE  myConditionVar;
std::list<Task*>    tasks;

DWORD WINAPI consumerThread(LPVOID param)
{
    Task* pTask = NULL;
    while (true)
    {
        //进入临界区
        EnterCriticalSection(&myCriticalSection);
        while (tasks.empty())
        {
            //如果SleepConditionVariableCS挂起，挂起前会离开临界区，不往下执行。
            //当发生变化后，条件合适，SleepConditionVariableCS将直接进入临界区。
            SleepConditionVariableCS(&myConditionVar, &myCriticalSection, INFINITE);
        }

        pTask = tasks.front();
        tasks.pop_front();

        //SleepConditionVariableCS被唤醒后进入临界区，
        //为了让其他线程有机会操作tasks，这里需要再次离开临界区
        LeaveCriticalSection(&myCriticalSection);

        if (pTask == NULL)
            continue;

        pTask->doTask();
        delete pTask;
        pTask = NULL;
    }

    return 0;
}

DWORD WINAPI producerThread(LPVOID param)
{
    int taskID = 0;
    Task* pTask = NULL;

    while (true)
    {
        pTask = new Task(taskID);

        //进入临界区
        EnterCriticalSection(&myCriticalSection);
        tasks.push_back(pTask);
        std::cout << "produce a task, taskID: " << taskID << ", threadID: " << GetCurrentThreadId() << std::endl;

        LeaveCriticalSection(&myCriticalSection);

        WakeConditionVariable(&myConditionVar);

        taskID++;

        //休眠1秒
        Sleep(1000);
    }

    return 0;
}

int main()
{
    InitializeCriticalSection(&myCriticalSection);
    InitializeConditionVariable(&myConditionVar);

    //创建5个消费者线程
    HANDLE consumerThreadHandles[5];
    for (int i = 0; i < 5; ++i)
    {
        consumerThreadHandles[i] = CreateThread(NULL, 0, consumerThread, NULL, 0, NULL);
    }

    //创建一个生产者线程
    HANDLE producerThreadHandle = CreateThread(NULL, 0, producerThread, NULL, 0, NULL);

    //等待生产者线程退出
    WaitForSingleObject(producerThreadHandle, INFINITE);

    //等待消费者线程退出
    for (int i = 0; i < 5; ++i)
    {
        WaitForSingleObject(consumerThreadHandles[i], INFINITE);
    }

    DeleteCriticalSection(&myCriticalSection);

    return 0;
}