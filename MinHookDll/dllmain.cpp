// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

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
    return fpMessageBoxW(hWnd, L"Hooked1!", lpCaption, uType);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize MinHook.
        MessageBox(NULL, L"This Is From Dll!\nInject Success1!", L"OK", MB_OK);
        MH_Initialize();

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
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        MH_DisableHook(&OpenProcess);
        MH_Uninitialize();
        break;
    }
    return TRUE;
}

