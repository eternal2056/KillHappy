// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include <windows.h>
#include "include\detours.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include <winhttp.h>
#include <Windows.h>
#include <winternl.h>
#include <cstdio>
#include <TlHelp32.h>

void WriteToLogFile(std::string message);
std::vector<std::string> forbidAddressList = {};
void readAndPrintFile(const std::string& filePath) {
	// ���ļ�
	std::ifstream inputFile(filePath);

	// ����ļ��Ƿ�ɹ���
	if (!inputFile.is_open()) {
		readAndPrintFile("Error opening file: ");
		return;
	}

	// ���ж�ȡ�ļ�����
	std::string line;
	while (std::getline(inputFile, line)) {
		// ����ÿһ�е�����
		forbidAddressList.push_back(line);
	}

	// �ر��ļ�
	inputFile.close();
}
void WriteToLogFile(std::string message)
{
	std::ofstream logfile("D:\\LogFile.txt", std::ios::app); // ���ļ���׷��д��
	if (logfile.is_open())
	{
		logfile << message << std::endl; // д����Ϣ
		logfile.close(); // �ر��ļ�
	}
}
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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
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