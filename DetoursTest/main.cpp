// CreateRemoteThread_Test.cpp : �������̨Ӧ�ó������ڵ㡣
#include "main.h"



std::string getParentDirectory(const std::string& path) {
    // �ҵ����һ��·���ָ���'\'
    size_t lastSlash = path.find_last_of("\\");

    if (lastSlash != std::string::npos) {
        // ��ȡ���ַ�������ȡ�ϲ�Ŀ¼
        return path.substr(0, lastSlash);
    }
    else {
        // ����Ҳ���·���ָ���������ԭʼ·��
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
        ShowWindow(hwnd, SW_HIDE); // ���ؿ���̨����
    }

    HANDLE mutexHandle = CreateMutex(NULL, TRUE, "MyUniqueMutex");

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // ����������Ѵ��ڣ���ʾ�Ѿ��г���ʵ��������
        std::cout << "�����Ѿ������У������������ʵ����" << std::endl;
    }
    else {
        // ������ǰ��������Ȩ��
        EnbalePrivileges(GetCurrentProcess());
        while (true) {
            AutoStartPrepare(currentDirectoryParent); // �Զ�����
            InjectionFunction(isInject); // ע��
            if (isInject == "1") {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } else {
                break;
            }
        }
        // �ͷŻ�����
        ReleaseMutex(mutexHandle);
    }
    // �رջ��������
    CloseHandle(mutexHandle);
    system("pause");
    return 0;
}