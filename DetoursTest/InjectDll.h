#ifndef _INJECT_DLL_H_
#define _INJECT_DLL_H_


#include <Windows.h>
#include <stdio.h>
#include "tlhelp32.h"
#include "tchar.h"

// ʹ�� CreateRemoteThread ʵ��Զ�߳�ע��
BOOL CreateRemoteThreadInjectDll(DWORD dwProcessId, const char * dllPath);
BOOL EjectDll(DWORD dwPID, const char* dllPath);

#endif