#ifndef _INJECT_DLL_H_
#define _INJECT_DLL_H_


#include <Windows.h>
#include <stdio.h>
#include "tlhelp32.h"
#include "tchar.h"

// 使用 CreateRemoteThread 实现远线程注入
BOOL CreateRemoteThreadInjectDll(DWORD dwProcessId, const char * dllPath);
BOOL EjectDll(DWORD dwPID, const char* dllPath);

#endif