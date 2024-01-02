#include "RemoteThreadInjection.h"

std::string getCurrentDirectoryPath() {
	const DWORD bufferSize = MAX_PATH;
	char buffer[bufferSize];

	// 获取当前工作目录
	DWORD length = GetCurrentDirectory(bufferSize, buffer);
	//std::cout << "当前用户路径：" << buffer << std::endl;
	if (length == 0) {
		// 获取失败
		return "";
	}

	// 将当前工作目录输出到标准输出
	std::string str = buffer;

	return str;
}
std::string getUserPath() {
	char* homePath;
	size_t len;

	// 获取 HOME 环境变量
	if (_dupenv_s(&homePath, &len, "USERPROFILE") == 0 && homePath != nullptr) {
		// 输出当前用户路径
		std::cout << "当前用户路径：" << homePath << std::endl;
	}
	else {
		std::cerr << "无法获取当前用户路径。" << std::endl;
	}

	std::string str = homePath;
	free(homePath);
	return str;
}


BOOL isExistsModules(DWORD processID, std::string moduleName) {
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
				if (dllPath.find(moduleName) != dllPath.npos) {
					return true;
					printf(" [%s][%d] Inject Dll OK.\n", dllPath.data(), 1);
				}
			}
		}
	}
	CloseHandle(hProcess);
	return false;
}
std::vector<DWORD> GetChromeProcessIds(const char* processName) {
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

void InjectionFunction(std::string isInject) {
	//std::string currentDirectory = getUserPath() + "\\AppData\\Local\\Temp\\";
	std::string currentDirectory = getCurrentDirectoryPath() + "\\";
	printf((currentDirectory + "\n").data());

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
	std::vector<std::string> moduleNameList = {
		//"msvcp140_1d",
		//"msvcp140_2d",
		//"msvcp140d",
		//"msvcp140d_atomic_wait",
		//"msvcp140d_codecvt_ids",
		//"ucrtbased",
		//"vcruntime140_1d",
		//"vcruntime140_threadsd",
		//"vcruntime140d",
		"ManagementDll",
	};
	int i = 0;

	if (isInject == "1") {
		printf(" [%s][%d] Inject Dll OK.\n", isInject.data(), i++);
		for (auto& processName : processNameList) {
			std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
			for (auto& processId : allProcessId) {
				if (IsProcessRunning(processId) == true) {
					for (auto& moduleName : moduleNameList) {
						if (isExistsModules(processId, moduleName) == false) {
							BOOL bRet = CreateRemoteThreadInjectDll(processId, (currentDirectory + moduleName + ".dll").data());
							if (bRet) printf("[%s] [%d] Inject Dll OK.\n", processName.data(), processId);
							else printf("[%s] [%d] Inject Dll Error.\n", processName.data(), processId);
						}
					}
				}
			}
		}
	}
	else {
		for (auto& processName : processNameList) {
			std::vector<DWORD> allProcessId = GetChromeProcessIds(processName.data());
			for (auto& processId : allProcessId) {
				for (auto& moduleName : moduleNameList) {
					if (isExistsModules(processId, moduleName) == true) {
						//BOOL bRet = EjectDll(processId, (currentDirectory + "ManagementDll.dll").data());
						BOOL bRet = EjectDll(processId, (moduleName + ".dll").data());
						if (bRet) printf("[%s] [%d] Eject Dll OK.\n", processName.data(), processId);
						else printf("[%s] [%d] Eject Dll Error.\n", processName.data(), processId);
					}
				}

			}
		}
	}
	//system("pause");
#endif
}
