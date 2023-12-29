#include "AutoStart.h"

class RegistryHelper {
public:
    static bool WriteRegistryValue(HKEY rootKey, const char* subKey, const char* valueName, const char* valueData) {
        HKEY hKey;
        if (RegCreateKeyExA(rootKey, subKey, 0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) {
            std::cerr << "无法打开或创建注册表项" << std::endl;
            return false;
        }

        if (RegSetValueExA(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(valueData), strlen(valueData) + 1) != ERROR_SUCCESS) {
            std::cerr << "无法写入注册表值" << std::endl;
            RegCloseKey(hKey);
            return false;
        }

        std::cout << "成功写入注册表值" << std::endl;
        RegCloseKey(hKey);
        return true;
    }

    static bool ReadRegistryValue(HKEY rootKey, const char* subKey, const char* valueName, char* buffer, DWORD bufferSize) {
        HKEY hKey;
        if (RegOpenKeyExA(rootKey, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
            std::cerr << "无法打开注册表项" << std::endl;
            return false;
        }

        if (RegQueryValueExA(hKey, valueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(buffer), &bufferSize) != ERROR_SUCCESS) {
            std::cerr << "无法读取注册表值" << std::endl;
            RegCloseKey(hKey);
            return false;
        }
        for (int i = 0; i < 1024; i++) {
            std::cout << std::to_string(buffer[i]) << std::endl;
        }
        std::cout << "读取到的值为: " << buffer << std::endl;
        RegCloseKey(hKey);
        return true;
    }
};

void AutoStartPrepare() {
    // 读取注册表值
    char buffer[1024];
    DWORD bufferSize = sizeof(buffer);
    RegistryHelper::ReadRegistryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\Microsoft\Windows\CurrentVersion\Run", "FontSize", buffer, bufferSize);
}