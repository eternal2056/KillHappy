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

void WriteToLogFile(std::string message);
// ��������
std::vector<std::string> getIPsByDomain(const char* domain) {
	std::vector<std::string> ipAddresses;

	// Declare and initialize variables
	WSADATA wsaData;
	int iResult;
	INT iRetval;
	struct addrinfo hints;
	DWORD dwRetval;

	int i = 1;

	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;

	struct sockaddr_in* sockaddr_ipv4;
	//    struct sockaddr_in6 *sockaddr_ipv6;
	LPSOCKADDR sockaddr_ip;

	char ipstringbuffer[46];
	DWORD ipbufferlength = 46;

	// Validate the parameters
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return ipAddresses;
	}


	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	dwRetval = getaddrinfo(domain, 0, &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		WSACleanup();
	}
	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		char* addr1;
		std::string addrString;
		switch (ptr->ai_family) {
		case AF_INET:
			sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
			printf("IPv4 address %s\n",
				inet_ntoa(sockaddr_ipv4->sin_addr));
			addr1 = inet_ntoa(sockaddr_ipv4->sin_addr);
			addrString = addr1;
			ipAddresses.push_back(addrString);
			break;
		case AF_INET6:
			sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
			ipbufferlength = 46;
			iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
				ipstringbuffer, &ipbufferlength);
			addrString = ipstringbuffer;
			ipAddresses.push_back(addrString);
			if (iRetval)
				printf("WSAAddressToString failed with %u\n", WSAGetLastError());
			else
				printf("IPv6 address %s\n", ipstringbuffer);
			break;
		}
	}

	freeaddrinfo(result);
	WSACleanup();
	return ipAddresses;
}

typedef int(WINAPI* OriginalSendTo)(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen);

OriginalSendTo s_sendto1 = NULL;

void WriteToLogFile(std::string message)
{
	std::ofstream logfile("D:\\LogFile.txt", std::ios::app); // ���ļ���׷��д��
	if (logfile.is_open())
	{
		logfile << message << std::endl; // д����Ϣ
		logfile.close(); // �ر��ļ�
	}
}


bool isMatched = false;
void GetPeerInfo(SOCKET s, LPWSABUF  lpBuffers, DWORD dwBufferCount)
{
	const char* urlName = "www.douyu.com";
	sockaddr_in peerAddr;
	sockaddr_in6 peerAddr_v6;
	int addrLen = sizeof(peerAddr);
	int addrv6Len = sizeof(peerAddr_v6);
	std::string allAddressString;
	std::vector<std::string> addressList;
	addressList = getIPsByDomain(urlName);
	if (getpeername(s, reinterpret_cast<sockaddr*>(&peerAddr), &addrLen) == 0)
	{
		//if (reinterpret_cast<sockaddr*>(&peerAddr)->sa_family == AF_INET) {
		//	WriteToLogFile("AF_INET");
		//}
		//else if (reinterpret_cast<sockaddr*>(&peerAddr)->sa_family == AF_INET6) {
		//	WriteToLogFile("AF_INET6");
		//}

		std::vector<std::string> bufferContents;

		//WriteToLogFile("dwBufferCount" + std::to_string(dwBufferCount));
		// �������������飬��ÿ�������������ݴ洢�� vector ��
		for (DWORD i = 0; i < dwBufferCount; ++i) {
			// ���� lpBuffers[i].buf ��һ����null��β���ַ�����
			std::string testString = "";
			char* myUnsignedCharPointer = new char[lpBuffers[i].len + 1];
			int k = 0;
			for (int j = 0; j < lpBuffers[i].len; j++) {
				unsigned char tempChar = lpBuffers[i].buf[j];
				if (tempChar >= 33 && tempChar <= 125) {
					myUnsignedCharPointer[k++] = lpBuffers[i].buf[j];
				}
				testString += std::to_string(lpBuffers[i].buf[j]) + " ";
			}
			myUnsignedCharPointer[k] = 0;
			//WriteToLogFile("testString: " + testString);
			bufferContents.push_back(myUnsignedCharPointer);
			//bufferContents.push_back(lpBuffers[i].buf);
			//WriteToLogFile("lpBuffers[i].len" + std::to_string(lpBuffers[i].len));
			delete[] myUnsignedCharPointer;
		}

		// �� vector �е��ַ�����������
		std::string resultString;
		for (std::string content : bufferContents) {
			resultString += content;

		}
		//WriteToLogFile("resultString.length()" + std::to_string(resultString.length()));
		std::vector<std::string> forbidAddressList = {
			"google.com",
			"gstatic.com",
			"douyu.com",
		};
		for (auto& forbidAddress : forbidAddressList) {
			if (resultString.find(forbidAddress) != resultString.npos) {
				isMatched = true;
				WriteToLogFile("[isMatched] DestIp: 127.0.0.1 | " + resultString);
				return;
			}
		}
		return;

		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peerAddr.sin_addr, ip, INET_ADDRSTRLEN);

		std::string str(ip);
		if (str != "127.0.0.1") {
			allAddressString = "DestIp: " + str + " allAddressString:";
			for (std::string ipAddress : addressList) {
				if (str == ipAddress) {
					isMatched = true;
				}
				allAddressString += " | " + ipAddress;
			}
			if (isMatched) {
				allAddressString = "[isMatched] " + allAddressString;
			}
			WriteToLogFile(allAddressString);
		}


		//char ipv6[256];
		//inet_ntop(AF_INET6, &peerAddr_v6.sin6_addr, ipv6, 256);

		//std::string str = ipv6;
		//WriteToLogFile(""+str);
	}
	if (getpeername(s, reinterpret_cast<sockaddr*>(&peerAddr_v6), &addrv6Len) == 0)
	{
		char ipv6[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET6, &peerAddr_v6.sin6_addr, ipv6, INET6_ADDRSTRLEN);

		std::string str = ipv6;
		if (str != "::cccc:cccc:cccc:cccc") {
			allAddressString = "DestIp: " + str + " allAddressString:";
			for (std::string ipAddress : addressList) {
				if (str == ipAddress) {
					isMatched = true;
				}
				allAddressString += " | " + ipAddress;
			}
			if (isMatched) {
				allAddressString = "[isMatched] " + allAddressString;
			}
			WriteToLogFile(allAddressString);
		}
	}
	else
	{
		//WriteToLogFile("Failed to get peer information.");
	}
}

// ���� sendto ��ԭʼ����ָ������

// ���平�Ӻ�Ļص�����
int WINAPI HookedSendTo(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	//MessageBoxW(NULL, L"HookedSendTo", L"MinHook Sample", MB_OK);
	OriginalSendTo originalSendTo = s_sendto1;
	return originalSendTo(s, buf, len, flags, to, tolen);
}

// ���庯��ָ�����ͣ����ڱ���ԭʼ�� wsasend ����
typedef INT(PASCAL FAR* LPFN_WSASEND)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

// ����ԭʼ�� wsasend ����ָ��
LPFN_WSASEND RealWsaSend = nullptr;

// �Զ���� HookedWsaSend ����
INT PASCAL FAR HookedWsaSend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	GetPeerInfo(s, lpBuffers, dwBufferCount);
	if (isMatched) {
		isMatched = false;
		dwBufferCount = 0;
	}
	// ����ԭʼ�� wsasend ����
	INT result = RealWsaSend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);

	// ���������������Լ����߼�

	return result;
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
		RealWsaSend = (LPFN_WSASEND)GetProcAddress(GetModuleHandle("ws2_32.dll"), "WSASend");

		// �� wsasend �������� Hook�������滻Ϊ�Զ���� HookedWsaSend ����
		DetourAttach(&(PVOID&)RealWsaSend, HookedWsaSend);

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