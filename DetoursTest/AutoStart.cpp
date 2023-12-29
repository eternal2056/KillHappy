#include "AutoStart.h"

class RegistryHelper {
public:
    static bool WriteRegistryValue(HKEY rootKey, const char* subKey, const char* valueName, const char* valueData) {
        HKEY hKey;
        if (RegCreateKeyExA(rootKey, subKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
            std::cerr << "�޷��򿪻򴴽�ע�����" << std::endl;
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

void AutoStartPrepare() {
    // ��ȡע���ֵ
    char buffer[1024];
    DWORD bufferSize = sizeof(buffer);
    RegistryHelper::ReadRegistryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\Microsoft\Windows\CurrentVersion\Run", "FontSize", buffer, bufferSize);
}