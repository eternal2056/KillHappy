#include "InjectDll.h"


void ShowError(const wchar_t* pszText)
{
	TCHAR szErr[MAX_PATH] = { 0 };
	wsprintf(szErr, "%s Error[%d]\n", pszText, ::GetLastError());
	::MessageBox(NULL, szErr, "ERROR", MB_OK);
}


// ʹ�� CreateRemoteThread ʵ��Զ�߳�ע��
BOOL CreateRemoteThreadInjectDll(DWORD dwProcessId)
{
	HANDLE hProcess = NULL;
	SIZE_T dwSize = 0;
	LPVOID pDllAddr = NULL;
	FARPROC pFuncProcAddr = NULL;

	// ��ע����̣���ȡ���̾��
	hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	if (NULL == hProcess)
	{
		ShowError(L"OpenProcess");
		return FALSE;
	}
	// ��ע������������ڴ�
	dwSize = 1 + ::lstrlen("C:\\D_Files\\Project_Driver\\KillHappy\\x64\\Debug\\DetoursTestDll.dll");
	pDllAddr = ::VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	if (NULL == pDllAddr)
	{
		ShowError(L"VirtualAllocEx");
		return FALSE;
	}
	// ��������ڴ���д������
	if (FALSE == ::WriteProcessMemory(hProcess, pDllAddr, "C:\\D_Files\\Project_Driver\\KillHappy\\x64\\Debug\\DetoursTestDll.dll", dwSize, NULL))
	{
		ShowError(L"WriteProcessMemory");
		return FALSE;
	}
	// ��ȡLoadLibraryA������ַ
	pFuncProcAddr = GetProcAddress(::GetModuleHandle("kernel32.dll"), "LoadLibraryA");
	if (pFuncProcAddr == NULL)
	{
		ShowError(L"GetProcAddress_LoadLibraryA");
		return FALSE;
	}

	// ʹ�� CreateRemoteThread ����Զ�߳�, ʵ�� DLL ע��
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
	//// �رվ��
	//printf("thread exitcode = 0x%x\n", exitCode);
	//printf("errcode = %d\n", GetLastError());
	::CloseHandle(hProcess);

	return TRUE;
}