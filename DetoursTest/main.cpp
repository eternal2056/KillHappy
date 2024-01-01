// CreateRemoteThread_Test.cpp : 定义控制台应用程序的入口点。
#include "main.h"
#define IS_SHOW_WINDOW TRUE

int _tmain(int argc, _TCHAR* argv[])
{
    HWND hwnd = GetConsoleWindow();
    if (IS_SHOW_WINDOW) {
        ShowWindow(hwnd, SW_NORMAL);
    }
    else {
        ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口
    }

    HANDLE mutexHandle = CreateMutex(NULL, TRUE, "MyUniqueMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // 如果互斥锁已存在，表示已经有程序实例在运行
        std::cout << "程序已经在运行，不能启动多个实例。" << std::endl;
    }
    else {
        AutoStartPrepare(); // 自动启动
        //InjectionFunction(argv); // 注入

        system("pause");
        return 0;

        // 释放互斥锁
        ReleaseMutex(mutexHandle);
    }
    // 关闭互斥锁句柄
    CloseHandle(mutexHandle);
}