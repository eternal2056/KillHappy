#include "pch.h"

typedef BOOL(WINAPI* LPFN_CREATEPROCESSW)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);

LPFN_CREATEPROCESSW TrueCreateProcessW;

std::string ConvertWideStringToNarrow(LPCWSTR wideString)
{
	// 获取宽字符字符串的长度
	int length = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);

	// 分配足够的空间存储窄字符字符串
	char* buffer = new char[length];

	// 将宽字符字符串转换为窄字符字符串
	WideCharToMultiByte(CP_UTF8, 0, wideString, -1, buffer, length, NULL, NULL);

	// 创建std::string对象
	std::string result(buffer);

	// 释放分配的内存
	delete[] buffer;

	return result;
}

BOOL WINAPI MyCreateProcessW(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
)
{
	// 在这里可以插入你的自定义代码
	// ...
	std::string processPath = ConvertWideStringToNarrow(lpApplicationName);
	if (processPath.find("Telegram.exe") != processPath.npos) {
		return FALSE;
	}
	WriteToLogFile(processPath);
	//LPCWSTR applicationPath = L"C:\\HOME_NGQ\\Telegram\\Telegram.exe";
	// 调用原始函数
	return TrueCreateProcessW(
		lpApplicationName, lpCommandLine, lpProcessAttributes,
		lpThreadAttributes, bInheritHandles, dwCreationFlags,
		lpEnvironment, lpCurrentDirectory, lpStartupInfo,
		lpProcessInformation
	);
}

void HookCreateProcessW() {
	//MessageBox(NULL, "This Is From Dll!\nInject Success!", "OK", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 获取原始 wsasend 函数的地址
	TrueCreateProcessW = (LPFN_CREATEPROCESSW)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateProcessW");

	// 对 wsasend 函数进行 Hook，将其替换为自定义的 HookedWsaSend 函数
	DetourAttach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);

	// 完成 Hook 事务
	DetourTransactionCommit();
}

void FreeCreateProcessW() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 恢复原始的 wsasend 函数
	DetourDetach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);

	// 完成 Hook 事务
	DetourTransactionCommit();
}
