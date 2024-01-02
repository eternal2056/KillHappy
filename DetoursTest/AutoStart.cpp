#include "AutoStart.h"

class RegistryHelper {
public:
    static bool WriteRegistryValue(HKEY rootKey, const char* subKey, const char* valueName, const char* valueData) {
        HKEY hKey;
        DWORD dwDisposition;
        TCHAR pBuf[1024]; // ��Ϣ��������С
        
        LONG regError = RegCreateKeyExA(rootKey, subKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr);
        if (regError != ERROR_SUCCESS) {
            std::cerr << "�޷��򿪻򴴽�ע�����" << std::endl;
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, regError, 0, pBuf, 1024, NULL);//
            return false;
        }

        if (RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(valueData), strlen(valueData) + 1) != ERROR_SUCCESS) {
            std::cerr << "�޷�д��ע���ֵ" << std::endl;
            RegCloseKey(hKey);
            return false;
        }

        std::cout << "�ɹ�д��ע���ֵ" << std::endl;
        RegCloseKey(hKey);
        return true;
    }

    static bool ReadRegistryValue(HKEY rootKey, const char* subKey, const char* valueName, char* buffer, DWORD bufferSize) {
        HKEY hKey;
        if (RegOpenKeyExA(rootKey, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            std::cerr << "�޷���ע�����" << std::endl;
            return false;
        }

        if (RegQueryValueExA(hKey, valueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &bufferSize) != ERROR_SUCCESS) {
            std::cerr << "�޷���ȡע���ֵ" << std::endl;
            RegCloseKey(hKey);
            return false;
        }
        for (int i = 0; i < 1024; i++) {
            std::cout << std::to_string(buffer[i]) << std::endl;
        }
        std::cout << "��ȡ����ֵΪ: " << buffer << std::endl;
        RegCloseKey(hKey);
        return true;
    }
};

//void CreateShortCut() {
//
//
//    CoInitialize(NULL);
//
//    IShellLink* psl;
//    CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
//
//    // ���ÿ�ݷ�ʽ��·��
//    psl->SetPath("D:\\Download\\Test\\DetoursTest.exe");
//
//    IPersistFile* ppf;
//    psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
//
//    // �����ݷ�ʽ
//    ppf->Save(L"D:\\Download\\Test\\DetoursTest.lnk", TRUE);
//
//    ppf->Release();
//    psl->Release();
//
//    CoUninitialize();
//
//    // ���ÿ�ݷ�ʽ��Ŀ��·��
//    LPCSTR targetPath = "D:\\Download\\Test\\DetoursTest.exe";
//    // ���ÿ�ݷ�ʽ�ı���·��
//    LPCSTR shortcutPath = "D:\\Download\\Test\\DetoursTest.lnk";
//
//    // ʹ��ShellExecute������ݷ�ʽ
//    HINSTANCE result = ShellExecuteA(NULL, "create", shortcutPath, targetPath, "", SW_SHOWNORMAL);
//
//    if ((int)result > 32) {
//        std::cout << "��ݷ�ʽ�����ɹ�" << std::endl;
//    }
//    else {
//        std::cerr << "������ݷ�ʽʧ��" << std::endl;
//    }
//}

void AutoStartPrepare(std::string directory) {
    // ��ȡע���ֵ
    //char buffer[1024];
    //DWORD bufferSize = sizeof(buffer);
    //RegistryHelper::ReadRegistryValue(HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run\sfweff", "FontSize", buffer, bufferSize);

    std::string exePath = "\"" + directory + "\\1.exe\" 1 " + std::to_string(IS_SHOW_WINDOW); // 1-ע�� 0-�γ� | 1-��ʾ 0-����
    const char *buffer = exePath.data();

    DWORD bufferSize = sizeof(buffer);
    RegistryHelper::WriteRegistryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "MyTest", buffer);
    RegistryHelper::WriteRegistryValue(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", "MyTest", buffer);
    //CreateShortCut();
}