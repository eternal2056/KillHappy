// CreateRemoteThread_Test.cpp : �������̨Ӧ�ó������ڵ㡣
#include "main.h"

int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = GetConsoleWindow();
	//ShowWindow(hwnd, SW_HIDE); // ���ؿ���̨����

	InjectionFunction(argv); // ע��

	return 0;
}