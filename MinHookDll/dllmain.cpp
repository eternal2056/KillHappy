// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <Windows.h>
#include <WinSock2.h>
#include "MinHook.h"
#include <WS2tcpip.h>
#include <fstream>

#pragma comment(lib, "ws2_32.lib")
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif


typedef int(WINAPI* OriginalSendTo)(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);

OriginalSendTo s_sendto1 = NULL;


typedef int (WINAPI* WSASend_t)(
    _In_  SOCKET                             s,
    _In_  LPWSABUF                           lpBuffers,
    _In_  DWORD                              dwBufferCount,
    _Out_ LPDWORD                            lpNumberOfBytesSent,
    _In_  DWORD                              dwFlags,
    _In_  LPWSAOVERLAPPED                    lpOverlapped,
    _In_  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
    );
// Function pointer for the original WSASend
WSASend_t oWSASend = nullptr;
void WriteToLogFile(const wchar_t* message)
{
    std::wofstream logfile("D:\\LogFile.txt", std::ios::app); // 打开文件，追加写入
    if (logfile.is_open())
    {
        logfile << message << std::endl; // 写入信息
        logfile.close(); // 关闭文件
    }
}

#include <locale>
#include <codecvt>

void GetPeerInfo(SOCKET s)
{
    sockaddr_in peerAddr;
    int addrLen = sizeof(peerAddr);

    if (getpeername(s, reinterpret_cast<sockaddr*>(&peerAddr), &addrLen) == 0)
    {
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peerAddr.sin_addr, ip, INET_ADDRSTRLEN);

        std::string str(ip);
        // 使用 std::wstring_convert 进行转换
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wideStr = converter.from_bytes(str);
        WriteToLogFile(wideStr.data());

        //std::cout << "Peer IP: " << ip << ", Port: " << ntohs(peerAddr.sin_port) << std::endl;
    }
    else
    {
        WriteToLogFile(L"Failed to get peer information.");
    }
}
// Custom implementation of WSASend
int WINAPI MyWSASend(
    _In_  SOCKET                             s,
    _In_  LPWSABUF                           lpBuffers,
    _In_  DWORD                              dwBufferCount,
    _Out_ LPDWORD                            lpNumberOfBytesSent,
    _In_  DWORD                              dwFlags,
    _In_  LPWSAOVERLAPPED                    lpOverlapped,
    _In_  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
) {
    // Your custom implementation goes here
    //MessageBoxW(NULL, L"MyWSASend", L"MinHook Sample", MB_OK);
    WriteToLogFile(L"MyWSASend");
    OutputDebugString(L"WSASend Hooked!\n");
    GetPeerInfo(s);
    // Call the original WSASend

    return oWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
}

// 定义 sendto 的原始函数指针类型

// 定义钩子后的回调函数
int WINAPI HookedSendTo(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
    MessageBoxW(NULL, L"HookedSendTo", L"MinHook Sample", MB_OK);
    OriginalSendTo originalSendTo = s_sendto1;
    return originalSendTo(s, buf, len, flags, to, tolen);
}

BOOL HookWSASend()
{
    // Create a hook for MessageBoxW, in disabled state.
    if (MH_CreateHook(&WSASend, &MyWSASend, reinterpret_cast<LPVOID*>(&oWSASend)) != MH_OK) {
        return 1;
    }

    // Enable the hook
    if (MH_EnableHook(&WSASend) != MH_OK) {
        return 1;
    }
}
#include <winhttp.h>
// 定义原始函数指针类型
typedef HINTERNET(WINAPI* WinHttpOpenRequest_t)(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName,
    LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR* ppwszAcceptTypes, DWORD dwFlags);

// 原始函数指针
WinHttpOpenRequest_t originalWinHttpOpenRequest = nullptr;

// 自定义的 hook 函数
HINTERNET WINAPI hookedWinHttpOpenRequest(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName,
    LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR* ppwszAcceptTypes, DWORD dwFlags) {
    // 在这里执行你的自定义逻辑
    WriteToLogFile(L"hookedWinHttpOpenRequest");

    // 调用原始函数
    return originalWinHttpOpenRequest(hConnect, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);
}

BOOL HookWinHttpOpenRequest()
{
    // 创建钩取点
    if (MH_CreateHookApi(L"winhttp.dll", "WinHttpOpenRequest", &hookedWinHttpOpenRequest, reinterpret_cast<LPVOID*>(&originalWinHttpOpenRequest)) != MH_OK) {
        return -1;
    }

    // 启用钩取
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
        return -1;
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    int i = 1;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize MinHook.
        MH_Initialize();
        MessageBox(NULL, L"This Is From Dll!\nInject Success!", L"OK", MB_OK);
        switch (i) {
        case 1:
            HookWSASend();
            break;
        case 2:
            HookWinHttpOpenRequest();
            break;
        case 3:
            HookWSASend();
            break;
        default:
            HookWSASend();
        }

        MessageBoxW(NULL, L"hooked!", L"MinHook Sample", MB_OK);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

