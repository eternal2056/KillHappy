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
	// ��ȡ���ַ��ַ����ĳ���
	int length = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, NULL, 0, NULL, NULL);

	// �����㹻�Ŀռ�洢խ�ַ��ַ���
	char* buffer = new char[length];

	// �����ַ��ַ���ת��Ϊխ�ַ��ַ���
	WideCharToMultiByte(CP_UTF8, 0, wideString, -1, buffer, length, NULL, NULL);

	// ����std::string����
	std::string result(buffer);

	// �ͷŷ�����ڴ�
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
	// ��������Բ�������Զ������
	// ...
	std::string processPath = ConvertWideStringToNarrow(lpApplicationName);
	if (processPath.find("Telegram.exe") != processPath.npos) {
		return FALSE;
	}
	WriteToLogFile(processPath);
	//LPCWSTR applicationPath = L"C:\\HOME_NGQ\\Telegram\\Telegram.exe";
	// ����ԭʼ����
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

	// ��ȡԭʼ wsasend �����ĵ�ַ
	TrueCreateProcessW = (LPFN_CREATEPROCESSW)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateProcessW");

	// �� wsasend �������� Hook�������滻Ϊ�Զ���� HookedWsaSend ����
	DetourAttach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);

	// ��� Hook ����
	DetourTransactionCommit();
}

void FreeCreateProcessW() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// �ָ�ԭʼ�� wsasend ����
	DetourDetach(&(PVOID&)TrueCreateProcessW, MyCreateProcessW);

	// ��� Hook ����
	DetourTransactionCommit();
}
