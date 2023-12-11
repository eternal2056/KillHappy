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
#include <psapi.h>

void ListModules(DWORD processID) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL) {
		return;
	}

	HMODULE hModules[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hModules[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
			}
		}
	}

	CloseHandle(hProcess);
}
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
	std::string isInject = argv[1];

	// 远线程注入 DLL
#ifndef _WIN64
	BOOL bRet = CreateRemoteThreadInjectDll(9568, L"C:\\D_Files\\Project_Driver\\ISBN9787115499240\\x64\\Debug\\RemoteThreadInjection_Dll.dll");
#else 
	std::vector<std::string> processNameList = {
		"chrome.exe",
		"explorer.exe",
		"Taskmgr.exe",
	};
	printf(" [%s] Eject Dll OK.\n", isInject);
	if (isInject == "1") {
		for (auto& processName : processNameList) {
			std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
			for (auto& processId : allProcessId) {
				BOOL bRet = CreateRemoteThreadInjectDll(processId, "D:\\Download\\Code\\WindowsDriver\\KillHappy\\x64\\Debug\\ManagementDll.dll");
				if (bRet) printf("[%s] [%d] Inject Dll OK.\n", processName.data(), processId);
				else printf("[%s] [%d] Inject Dll Error.\n", processName.data(), processId);
			}
		}
	}
	else {
		for (auto& processName : processNameList) {
			std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
			for (auto& processId : allProcessId) {
				BOOL bRet = EjectDll(processId, "D:\\Download\\Code\\WindowsDriver\\KillHappy\\x64\\Debug\\ManagementDll.dll");
				if (bRet) printf("[%s] [%d] Eject Dll OK.\n", processName.data(), processId);
				else printf("[%s] [%d] Eject Dll Error.\n", processName.data(), processId);
			}
		}
	}
	system("pause");
#endif
	return 0;
}

