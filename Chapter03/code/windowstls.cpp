#include <Windows.h>
#include <iostream>

__declspec(thread) int g_mydata = 1;

DWORD __stdcall WorkerThreadProc1(LPVOID lpThreadParameter)
{
    while (true)
    {
        ++g_mydata;
        //std::cout << "g_mydata = " << g_mydata << ", ThreadID = " << GetCurrentThreadId() << std::endl;
        Sleep(1000);
    }
    return 0;
}

DWORD __stdcall WorkerThreadProc2(LPVOID lpThreadParameter)
{
    while (true)
    {       
        std::cout << "g_mydata = " << g_mydata << ", ThreadID = " << GetCurrentThreadId() << std::endl;
        Sleep(1000);
    }
    return 0;
}

int main()
{
    HANDLE hWorkerThreads[2];
    hWorkerThreads[0] = CreateThread(NULL, 0, WorkerThreadProc1, NULL, 0, NULL);
    hWorkerThreads[1] = CreateThread(NULL, 0, WorkerThreadProc2, NULL, 0, NULL);
    
    CloseHandle(hWorkerThreads[0]);
    CloseHandle(hWorkerThreads[1]);

    while (true)
    {
        Sleep(1000);
    }
    
    return 0;
}

