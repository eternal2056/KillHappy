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

		//// �ָ�ԭʼ�� wsasend ����
		//DetourDetach(&(PVOID&)RealWsaSend, HookedWsaSend);

		//// ��� Hook ����
		//DetourTransactionCommit();
	}

	return TRUE;
}