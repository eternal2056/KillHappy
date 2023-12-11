// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
//#include "CreateProcessW.cpp"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		HookWsaSend();
		HookCreateProcessW();
		HookZwQuerySystemInformation();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		//DetourTransactionBegin();
		//DetourUpdateThread(GetCurrentThread());

		//// 恢复原始的 wsasend 函数
		//DetourDetach(&(PVOID&)RealWsaSend, HookedWsaSend);

		//// 完成 Hook 事务
		//DetourTransactionCommit();
	}

	return TRUE;
}