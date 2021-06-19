/**
 *@desc: AsyncLogger.cpp，Windows版本
 *@author: zhangyl
 *@date: 2018.11.10
 */
#include "stdafx.h"
#include <windows.h>
#include <list>
#include <string>
#include <iostream>
#include <sstream>

std::list<std::string> cached_logs;
CRITICAL_SECTION g_cs;
HANDLE g_hSemaphore = NULL;
HANDLE g_hLogFile = INVALID_HANDLE_VALUE;

bool Init()
{
    InitializeCriticalSection(&g_cs);
    
    //假设资源数量上限是0xFFFFFFFF
    g_hSemaphore = CreateSemaphore(NULL, 0, 0xFFFFFFFF, NULL);
    
    //如果文件不存在，则创建
    g_hLogFile = CreateFile(_T("my.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (g_hLogFile == INVALID_HANDLE_VALUE)
        return false;

    return true;
}

void Uninit()
{
    DeleteCriticalSection(&g_cs);
    
    if (g_hSemaphore != NULL)
        CloseHandle(g_hSemaphore);
    
    if (g_hLogFile != INVALID_HANDLE_VALUE)
        CloseHandle(g_hLogFile);
}

bool WriteLogToFile(const std::string& line)
{
    if (g_hLogFile == INVALID_HANDLE_VALUE)
        return false;

    DWORD dwBytesWritten;
    //如果对于比较长的日志，应该分段写入，因为单次写入可能只能写入部分，这里为了演示方便，逻辑从简
    if (!WriteFile(g_hLogFile, line.c_str(), line.length(), &dwBytesWritten, NULL) || dwBytesWritten != line.length())
        return false;
    
    //将日志立即冲刷到文件中去
    FlushFileBuffers(g_hLogFile);

    return true;
}

DWORD CALLBACK LogProduceThreadProc(LPVOID lpThreadParameter)
{
    int index = 0;
    while (true)
    {
        ++ index;
        std::ostringstream os;
        os << "This is log, index: " << index << ", producer threadID: " << GetCurrentThreadId() << "\n";
      
        EnterCriticalSection(&g_cs);       
        cached_logs.emplace_back(os.str());
        LeaveCriticalSection(&g_cs);

        ReleaseSemaphore(g_hSemaphore, 1, NULL);
                   
        Sleep(100);
    }

    return 0;
}

DWORD CALLBACK LogConsumeThreadProc(LPVOID lpThreadParameter)
{
    std::string line;
    while (true)
    {     
        //无限等待
        WaitForSingleObject(g_hSemaphore, INFINITE);

        EnterCriticalSection(&g_cs);
        if (!cached_logs.empty())
        {
            line = cached_logs.front();
            cached_logs.pop_front();
        }     
        LeaveCriticalSection(&g_cs);          


        if (line.empty())
        {        
            Sleep(1000);

            continue;
        }

        WriteLogToFile(line);

        line.clear();
    }
}

int main(int argc, char* argv[])
{
    if (!Init())
    {
        std::cout << "init log file error." << std::endl;
        return -1;
    }
    
    HANDLE hProducers[3];
    for (int i = 0; i < sizeof(hProducers) / sizeof(hProducers[0]); ++i)
    {
        hProducers[i] = CreateThread(NULL, 0, LogProduceThreadProc, NULL, 0, NULL);
    }

    HANDLE hConsumers[3];
    for (int i = 0; i < sizeof(hConsumers) / sizeof(hConsumers[0]); ++i)
    {
        hConsumers[i] = CreateThread(NULL, 0, LogConsumeThreadProc, NULL, 0, NULL);
    }

    //等待消费者线程退出
    for (int i = 0; i < sizeof(hProducers) / sizeof(hProducers[0]); ++i)
    {
        WaitForSingleObject(hProducers[i], INFINITE);
    }

    //等待生产者线程退出
    for (int i = 0; i < sizeof(hConsumers) / sizeof(hConsumers[0]); ++i)
    {
        WaitForSingleObject(hConsumers[i], INFINITE);
    }

    Uninit();
    
    return 0;
}

