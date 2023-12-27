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
#include <chrono>
#include <thread>

std::string getCurrentDirectoryPath() {
	const DWORD bufferSize = MAX_PATH;
	char buffer[bufferSize];

	// 获取当前工作目录
	DWORD length = GetCurrentDirectory(bufferSize, buffer);

	if (length == 0) {
		// 获取失败
		return "";
	}

	// 将当前工作目录输出到标准输出
	std::string str = buffer;

	return str;
}

BOOL isExistsModules(DWORD processID) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess == NULL) {
		return true;
	}

	HMODULE hModules[1024];
	DWORD cbNeeded;

	if (EnumProcessModules(hProcess, hModules, sizeof(hModules), &cbNeeded)) {
		for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
			TCHAR szModName[MAX_PATH];

			// Get the full path to the module's file.
			if (GetModuleFileNameEx(hProcess, hModules[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
				std::string dllPath = szModName;
				if (dllPath.find("ManagementDll") != dllPath.npos) {
					return true;
					printf(" [%s][%d] Inject Dll OK.\n", dllPath.data(), 1);
				}
			}
		}
	}
	CloseHandle(hProcess);
	return false;
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
bool IsProcessRunning(DWORD processId) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);
	if (hProcess == NULL) {
		// Unable to open the process
		return false;
	}

	DWORD exitCode;
	if (GetExitCodeProcess(hProcess, &exitCode) && exitCode == STILL_ACTIVE) {
		// Process is running
		CloseHandle(hProcess);
		return true;
	}

	// Process has exited
	CloseHandle(hProcess);
	return false;
}
int _tmain(int argc, _TCHAR* argv[])
{
	HWND hwnd = GetConsoleWindow();
	// 隐藏控制台窗口
	ShowWindow(hwnd, SW_HIDE);
	// 提升当前进程令牌权限
	EnbalePrivileges(GetCurrentProcess());
	std::string isInject = argv[1];
	//std::string isInject = "0";

	// 远线程注入 DLL
#ifndef _WIN64
	BOOL bRet = CreateRemoteThreadInjectDll(9568, L"C:\\D_Files\\Project_Driver\\ISBN9787115499240\\x64\\Debug\\RemoteThreadInjection_Dll.dll");
#else 
	std::vector<std::string> processNameList = {
		"chrome.exe",
		"explorer.exe",
		"Taskmgr.exe",
	};
	int i = 0;
	
	if (isInject == "1") {
		while (true) {
			printf(" [%s][%d] Inject Dll OK.\n", isInject.data(), i++);
			for (auto& processName : processNameList) {
				std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
				for (auto& processId : allProcessId) {
					if (IsProcessRunning(processId) == true) {
						if (isExistsModules(processId) == false) {
							BOOL bRet = CreateRemoteThreadInjectDll(processId, "D:\\Download\\Code\\WindowsDriver\\KillHappy\\x64\\Debug\\ManagementDll.dll");
							if (bRet) printf("[%s] [%d] Inject Dll OK.\n", processName.data(), processId);
							else printf("[%s] [%d] Inject Dll Error.\n", processName.data(), processId);
						}
					}
				}
			}
			// 休眠1秒
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}
	else {
		for (auto& processName : processNameList) {
			std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
			for (auto& processId : allProcessId) {
				if (isExistsModules(processId) == true) {
					BOOL bRet = EjectDll(processId, "D:\\Download\\Code\\WindowsDriver\\KillHappy\\x64\\Debug\\ManagementDll.dll");
					if (bRet) printf("[%s] [%d] Eject Dll OK.\n", processName.data(), processId);
					else printf("[%s] [%d] Eject Dll Error.\n", processName.data(), processId);
				}
			}
		}
	}
	//system("pause");
#endif
	return 0;
}

