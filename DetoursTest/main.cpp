// CreateRemoteThread_Test.cpp : �������̨Ӧ�ó������ڵ㡣
#include "main.h"
#define IS_SHOW_WINDOW TRUE

int _tmain(int argc, _TCHAR* argv[])
{
    HWND hwnd = GetConsoleWindow();
    if (IS_SHOW_WINDOW) {
        ShowWindow(hwnd, SW_NORMAL);
    }
    else {
        ShowWindow(hwnd, SW_HIDE); // ���ؿ���̨����
    }

    HANDLE mutexHandle = CreateMutex(NULL, TRUE, "MyUniqueMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // ����������Ѵ��ڣ���ʾ�Ѿ��г���ʵ��������
        std::cout << "�����Ѿ������У������������ʵ����" << std::endl;
    }
    else {
        AutoStartPrepare(); // �Զ�����
        //InjectionFunction(argv); // ע��

        system("pause");
        return 0;

        // �ͷŻ�����
        ReleaseMutex(mutexHandle);
    }
    // �رջ��������
    CloseHandle(mutexHandle);
}