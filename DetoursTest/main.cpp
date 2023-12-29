// CreateRemoteThread_Test.cpp : 定义控制台应用程序的入口点。
#include "main.h"

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = GetConsoleWindow();
	//ShowWindow(hwnd, SW_HIDE); // 隐藏控制台窗口

	InjectionFunction(argv); // 注入

	return 0;
}