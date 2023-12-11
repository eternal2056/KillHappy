#include "InjectDll.h"


void ShowError(const wchar_t* pszText)
{
	TCHAR szErr[MAX_PATH] = { 0 };
	wsprintf(szErr, "%s Error[%d]\n", pszText, ::GetLastError());
	::MessageBox(NULL, szErr, "ERROR", MB_OK);
}


// 使用 CreateRemoteThread 实现远线程注入
BOOL CreateRemoteThreadInjectDll(DWORD dwProcessId, const char* dllPath)
{
	HANDLE hProcess = NULL;
	SIZE_T dwSize = 0;
	LPVOID pDllAddr = NULL;
	FARPROC pFuncProcAddr = NULL;

	// 打开注入进程，获取进程句柄
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		ShowError(L"OpenProcess");
		return FALSE;
	}
	// 在注入进程中申请内存
	dwSize = 1 + ::lstrlen(dllPath);
	pDllAddr = ::VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pDllAddr)
	{
		ShowError(L"VirtualAllocEx");
		return FALSE;
	}
	// 向申请的内存中写入数据
	if (FALSE == ::WriteProcessMemory(hProcess, pDllAddr, dllPath, dwSize, NULL))
	{
		ShowError(L"WriteProcessMemory");
		return FALSE;
	}
	// 获取LoadLibraryA函数地址
	pFuncProcAddr = GetProcAddress(::GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (pFuncProcAddr == NULL)
	{
		ShowError(L"GetProcAddress_LoadLibraryA");
		return FALSE;
	}

	// 使用 CreateRemoteThread 创建远线程, 实现 DLL 注入
	HANDLE hRemoteThread = ::CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFuncProcAddr, pDllAddr, 0, NULL);
	if (NULL == hRemoteThread)
	{
		ShowError(L"CreateRemoteThread");
		return FALSE;
	}
	//WaitForSingleObject(hRemoteThread, -1);
	//DWORD exitCode = 0;
	//if (!GetExitCodeThread(hRemoteThread, &exitCode))
	//	ShowError(L"GetExitCodeThread error!");
	//// 关闭句柄
	//printf("thread exitcode = 0x%x\n", exitCode);
	//printf("errcode = %d\n", GetLastError());
	::CloseHandle(hProcess);

	return TRUE;
}


BOOL EjectDll(DWORD dwPID, const char* dllPath)
{
	BOOL bMore = FALSE, bFound = FALSE;
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hModule = NULL;
	MODULEENTRY32 me = { sizeof(me) };
	LPTHREAD_START_ROUTINE pThreadProc;

	// dwPID = notepad 进程ID
	// 使用TH32CS_SNAPMODULE参数，获取加载到notepad进程的DLL名称
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

	bMore = Module32First(hSnapshot, &me);
	for (; bMore; bMore = Module32Next(hSnapshot, &me))
	{
		if (!_tcsicmp((LPCTSTR)me.szModule, dllPath) ||
			!_tcsicmp((LPCTSTR)me.szExePath, dllPath))
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		CloseHandle(hSnapshot);
		return FALSE;
	}

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		return FALSE;
	}

	hModule = GetModuleHandle("kernel32.dll");
	// 获取FreeLibrary函数加载地址，并使用CreateRemoteThread进行调用
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(::GetModuleHandle("kernel32.dll"), "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0,
		pThreadProc, me.modBaseAddr,
		0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	CloseHandle(hSnapshot);

	return TRUE;
}