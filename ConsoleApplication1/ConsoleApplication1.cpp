//#include <iostream>
//#include <chrono>
//#include <thread>

//// �������ڼ���Ƿ��Ѿ������ض���ʱ���
//bool isTimeReached(std::chrono::system_clock::time_point targetTime) {
//	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
//	return currentTime >= targetTime;
//}
//
//int main() {
//	// ��ȡ��ǰʱ���
//	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
//
//	// ����Ŀ��ʱ��㣨��������Ϊ��ǰʱ������5�룩
//	std::chrono::seconds timeOffset(5); // ����5���ʱ����
//	std::chrono::system_clock::time_point targetTime = currentTime + timeOffset;
//
//	std::cout << "�ȴ�����Ŀ��ʱ��..." << std::endl;
//
//	// ����Ƿ��Ѿ�����Ŀ��ʱ���
//	while (!isTimeReached(targetTime)) {
//		// �����������������񣬻���ֻ�Ǽ򵥵صȴ�
//		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ÿ100������һ��
//	}
//
//	std::cout << "�Ѿ�����Ŀ��ʱ��㣡" << std::endl;
//
//	return 0;
//}
#include <Windows.h>
#include "MinHook.h"

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

typedef int (WINAPI* MESSAGEBOXW)(HWND, LPCWSTR, LPCWSTR, UINT);

// Pointer for calling original MessageBoxW. 
MESSAGEBOXW fpMessageBoxW = NULL;

// Detour function which overrides MessageBoxW.
int WINAPI DetourMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    return fpMessageBoxW(hWnd, L"Hooked!", lpCaption, uType);
}

int main()
{
    // Initialize MinHook.
    if (MH_Initialize() != MH_OK)
    {
        return 1;
    }

    // Create a hook for MessageBoxW, in disabled state.
    if (MH_CreateHook(&MessageBoxW, &DetourMessageBoxW,
        reinterpret_cast<LPVOID*>(&fpMessageBoxW)) != MH_OK)
    {
        return 1;
    }

    // or you can use the new helper function like this.
    //if (MH_CreateHookApiEx(
    //    L"user32", "MessageBoxW", &DetourMessageBoxW, &fpMessageBoxW) != MH_OK)
    //{
    //    return 1;
    //}

    // Enable the hook for MessageBoxW.
    if (MH_EnableHook(&MessageBoxW) != MH_OK)
    {
        return 1;
    }

    // Expected to tell "Hooked!".
    MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

    // Disable the hook for MessageBoxW.
    if (MH_DisableHook(&MessageBoxW) != MH_OK)
    {
        return 1;
    }

    // Expected to tell "Not hooked...".
    MessageBoxW(NULL, L"Not hooked...", L"MinHook Sample", MB_OK);

    // Uninitialize MinHook.
    if (MH_Uninitialize() != MH_OK)
    {
        return 1;
    }

    return 0;
}
