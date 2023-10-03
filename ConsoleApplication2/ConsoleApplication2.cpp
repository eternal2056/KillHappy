#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <chrono>

int main()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    while (true) {
        // ����һ�����̿���
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        if (hSnapshot == INVALID_HANDLE_VALUE) {
            std::cerr << "�޷��������̿���" << std::endl;
            return 1;
        }

        // ���ýṹ����Ϣ
        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        // ��ʼ���������б�
        if (Process32First(hSnapshot, &pe32)) {
            do {
                std::wcout << "����ID " << pe32.th32ProcessID << "\t��������: " << pe32.szExeFile << std::endl;
                // ������Ҫ�Ķ��ֽ��ַ�����С
                int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, nullptr, 0, nullptr, nullptr);
                // ����һ�����������洢���ֽ��ַ���
                char* buffer = new char[bufferSize];
                // ��WCHAR�ַ���ת��Ϊ���ֽ��ַ���
                WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, buffer, bufferSize, nullptr, nullptr);
                std::string processName = buffer;
                if (processName == "ConsoleApplication2.exe") {
                    HANDLE hToken;
                    TOKEN_PRIVILEGES tkp;

                    // Get a token for this process. 

                    if (!OpenProcessToken(GetCurrentProcess(),
                        TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
                        return(FALSE);

                    // Get the LUID for the shutdown privilege. 

                    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                        &tkp.Privileges[0].Luid);

                    tkp.PrivilegeCount = 1;  // one privilege to set    
                    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

                    // Get the shutdown privilege for this process. 

                    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                        (PTOKEN_PRIVILEGES)NULL, 0);

                    if (GetLastError() != ERROR_SUCCESS)
                        return FALSE;

                    // ������Ϣ���������û����
                    int result = MessageBox(NULL, TEXT("ȷ��Ҫ�رռ������"), TEXT("�ػ�ȷ��"), MB_ICONQUESTION | MB_YESNO);

                    if (result == IDYES) {

                        // �û������"��"��ť��ִ�йػ���������Ҫ����ԱȨ�ޣ�1
                        if (ExitWindowsEx(EWX_SHUTDOWN, SHTDN_REASON_MAJOR_OPERATINGSYSTEM | SHTDN_REASON_MINOR_UPGRADE | SHTDN_REASON_FLAG_PLANNED)) {
                            // �ػ������ɹ�
                            return 0;
                        }
                        else {
                            // �ػ�����ʧ��
                            return 1;
                        }
                    }
                    else {

                        // �û������"��"��ť��ر���Ϣ�򣬲�ִ�йػ�����
                        return 0;
                    }
                }
            } while (Process32Next(hSnapshot, &pe32));
        }

        // �رս��̿��վ��
        CloseHandle(hSnapshot);
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
