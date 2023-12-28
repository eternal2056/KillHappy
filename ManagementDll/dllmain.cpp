// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
//#include "CreateProcessW.cpp"
extern "C" __declspec(dllexport) void UnloadDll()
{
	// ִ��DLLж��ʱ��Ҫ���е�������
	// ...

	// �ͷ���Դ���رվ����
	// ...

	// �˳�DLL
	FreeLibraryAndExitThread(GetModuleHandle("ManagementDll.dll"), 0);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	extern std::string currentDirectory;
	//MessageBox(NULL, "DLL_PROCESS_DETACH", "OK", MB_OK);
	//currentDirectory = getUserPath() + "\\AppData\\Local\\Temp\\";
	currentDirectory = getModulePath(hModule);
	UNREFERENCED_PARAMETER(lpReserved);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		HookWsaSend();
		HookCreateProcessW();
		HookZwQuerySystemInformation();
		HookRecv();
	}
	else if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		//MessageBox(NULL, "DLL_PROCESS_DETACH", "OK", MB_OK);
		FreeWsaSend();
		FreeCreateProcessW();
		FreeZwQuerySystemInformation();
		FreeRecv();
	}

	return TRUE;
}