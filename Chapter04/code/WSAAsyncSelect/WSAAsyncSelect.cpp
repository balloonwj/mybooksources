// WSAAsyncSelect.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <winsock2.h>
#include "WSAAsyncSelect.h"

#pragma comment(lib, "ws2_32.lib")

//socket 消息
#define WM_SOCKET   WM_USER + 1

//当前在线用户数量
int    g_nCount = 0;

SOCKET              InitSocket();
ATOM MyRegisterClass(HINSTANCE hInstance);
HWND InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT OnSocketEvent(HWND hWnd, WPARAM wParam, LPARAM lParam);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    SOCKET hListenSocket = InitSocket();
    if (hListenSocket == INVALID_SOCKET)
        return 1;

	MSG msg;
	MyRegisterClass(hInstance);

    HWND hwnd = InitInstance(hInstance, nCmdShow);
    if (hwnd == NULL)
        return 1;

    //利用 WSAAsyncSelect 将侦听 socket 与 hwnd 绑定在一起
    if (WSAAsyncSelect(hListenSocket, hwnd, WM_SOCKET, FD_ACCEPT) == SOCKET_ERROR)
        return 1;

	while (GetMessage(&msg, NULL, 0, 0))
	{		
        TranslateMessage(&msg);
        DispatchMessage(&msg);	
	}

    closesocket(hListenSocket);
    WSACleanup();

	return (int) msg.wParam;
}

SOCKET InitSocket()
{
    //1. 初始化套接字库
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    int nError = WSAStartup(wVersionRequested, &wsaData);
    if (nError != 0)
        return INVALID_SOCKET;

    if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
    {
        WSACleanup();
        return INVALID_SOCKET;
    }

    //2. 创建用于监听的套接字
    SOCKET hListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(6000);

    //3. 绑定套接字
    if (bind(hListenSocket, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        closesocket(hListenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    //4. 将套接字设为监听模式，准备接受客户请求
    if (listen(hListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        closesocket(hListenSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return hListenSocket;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WSAASYNCSELECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= _T("DemoWindowCls");
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{  
   HWND hWnd = CreateWindow(_T("DemoWindowCls"), _T("DemoWindow"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd)
      return NULL;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

    switch (uMsg)
	{
    case WM_SOCKET:
        return OnSocketEvent(hWnd, wParam, lParam);


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

LRESULT OnSocketEvent(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    SOCKET s = (SOCKET)wParam;
    int nEventType = WSAGETSELECTEVENT(lParam);
    int nErrorCode = WSAGETSELECTERROR(lParam);
    if (nErrorCode != 0)
        return 1;

    switch (nEventType)
    {
    case FD_ACCEPT:
    {
        //调用accept函数处理接受连接事件;
        SOCKADDR_IN addrClient;
        int len = sizeof(SOCKADDR);
        //等待客户请求到来
        SOCKET hSockClient = accept(s, (SOCKADDR*)&addrClient, &len);
        if (hSockClient != SOCKET_ERROR)
        {
            //产生的客户端socket，监听其 FD_READ/FD_CLOSE 事件
            if (WSAAsyncSelect(hSockClient, hWnd, WM_SOCKET, FD_READ | FD_CLOSE) == SOCKET_ERROR)
            {
                closesocket(hSockClient);
                return 1;
            }

            g_nCount++;  
            TCHAR szLogMsg[64];
            wsprintf(szLogMsg, _T("a client connected, socket: %d, current: %d\n"), (int)hSockClient, g_nCount);
            OutputDebugString(szLogMsg);
        }
    }
    break;

    case FD_READ:
    {
        char szBuf[64] = { 0 };
        int n = recv(s, szBuf, 64, 0);
        if (n > 0)
        {
            OutputDebugStringA(szBuf);
        }
        else if (n <= 0)
        {
            g_nCount--;
            TCHAR szLogMsg[64];
            wsprintf(szLogMsg, _T("a client disconnected, socket: %d, current: %d\n"), (int)s, g_nCount);
            OutputDebugString(szLogMsg);
            closesocket(s);
        }
    }
    break;

    case FD_CLOSE:
    {
        g_nCount--;
        TCHAR szLogMsg[64];
        wsprintf(szLogMsg, _T("a client disconnected, socket: %d, current: %d\n"), (int)s, g_nCount);
        OutputDebugString(szLogMsg);
        closesocket(s);
    }
    break;

    }// end switch

    return 0;
}
