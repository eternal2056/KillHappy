// CreateRemoteThread_Test.cpp : 定义控制台应用程序的入口点。
//
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include "InjectDll.h"
#include "AdjustTokenPrivilegesTest.h"
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>


std::vector<DWORD> GetChromeProcessIds(const char * processName) {
    std::vector<DWORD> chromeProcessIds;

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (_stricmp(processEntry.szExeFile, processName) == 0) {
                chromeProcessIds.push_back(processEntry.th32ProcessID);
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return chromeProcessIds;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// 提升当前进程令牌权限
	EnbalePrivileges(GetCurrentProcess());

	// 远线程注入 DLL
#ifndef _WIN64
	BOOL bRet = CreateRemoteThreadInjectDll(9568, L"C:\\D_Files\\Project_Driver\\ISBN9787115499240\\x64\\Debug\\RemoteThreadInjection_Dll.dll");
#else 
    std::vector<DWORD> allProcessId = GetChromeProcessIds("chrome.exe");
    for (auto& processId : allProcessId) {
        BOOL bRet = CreateRemoteThreadInjectDll(processId, "D:\\Download\\Code\\WindowsDriver\\KillHappy\\x64\\Debug\\DetoursTestDll.dll");
        if (bRet) printf("[%d] Inject Dll OK.\n", processId);
        else printf("[%d] Inject Dll Error.\n", processId);
    }
#endif

	system("pause");
	return 0;
}

