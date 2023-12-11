#include "pch.h"

typedef int (WINAPI* OriginalRecvType)(SOCKET s, char* buf, int len, int flags);

// ԭʼ�����ĵ�ַ
OriginalRecvType OriginalRecv = NULL;

bool isMatchedRecv = false;
void GetPeerInfoRecv(SOCKET s, LPWSABUF  lpBuffers, DWORD dwBufferCount)
{
	const char* urlName = "www.douyu.com";
	sockaddr_in peerAddr;
	sockaddr_in6 peerAddr_v6;
	int addrLen = sizeof(peerAddr);
	int addrv6Len = sizeof(peerAddr_v6);
	std::string allAddressString;
	std::vector<std::string> addressList;
	//addressList = getIPsByDomain(urlName);

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
	extern std::vector<std::string> forbidAddressList;
	for (auto& forbidAddress : forbidAddressList) {
		if (resultString.find(forbidAddress) != resultString.npos) {
			isMatchedRecv = true;
			WriteToLogFile("[isMatched] DestIp: 127.0.0.1 | " + resultString);
			return;
		}
	}
	return;

	if (getpeername(s, reinterpret_cast<sockaddr*>(&peerAddr), &addrLen) == 0)
	{
		//if (reinterpret_cast<sockaddr*>(&peerAddr)->sa_family == AF_INET) {
		//	WriteToLogFile("AF_INET");
		//}
		//else if (reinterpret_cast<sockaddr*>(&peerAddr)->sa_family == AF_INET6) {
		//	WriteToLogFile("AF_INET6");
		//}
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &peerAddr.sin_addr, ip, INET_ADDRSTRLEN);

		std::string str(ip);
		if (str != "127.0.0.1") {
			allAddressString = "DestIp: " + str + " allAddressString:";
			for (std::string ipAddress : addressList) {
				if (str == ipAddress) {
					isMatchedRecv = true;
				}
				allAddressString += " | " + ipAddress;
			}
			if (isMatchedRecv) {
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
					isMatchedRecv = true;
				}
				allAddressString += " | " + ipAddress;
			}
			if (isMatchedRecv){
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

// �ҹ�����
int WINAPI HookedRecv(SOCKET s, char* buf, int len, int flags) {
    // ����������������Զ������
	int k = 0;
	std::string xxx = buf;
	std::string test = "";
	char* myUnsignedCharPointer = new char[len + 1];
	for (int i = 0; i < len; i++) {
		unsigned char tempChar = buf[i];
		if (tempChar >= 33 && tempChar <= 125) {
			myUnsignedCharPointer[k++] = buf[i];
		}
	}
	std::vector<std::string> bufferContents;
	myUnsignedCharPointer[k] = 0;
	bufferContents.push_back(myUnsignedCharPointer);
	delete[] myUnsignedCharPointer;
	std::string resultString;
	for (std::string content : bufferContents) {
		resultString += content;

	}
	//WriteToLogFile("[HookedRecv] DestIp: 127.0.0.1 | " + resultString);
	//WriteToLogFile("[isMatched][HookedRecv] DestIp: 127.0.0.1 | " + xxx);
	extern std::vector<std::string> forbidAddressList;
	for (auto& forbidAddress : forbidAddressList) {
		if (resultString.find(forbidAddress) != resultString.npos) {
			isMatchedRecv = true;
			WriteToLogFile("[isMatched][HookedRecv] DestIp: 127.0.0.1 | " + resultString);
			return SOCKET_ERROR;
		}
	}

    // ����ԭʼ����
    return OriginalRecv(s, buf, len, flags);
}

void HookRecv() {
	//MessageBox(NULL, "This Is From Dll!\nInject Success!", "OK", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// ��ȡԭʼ wsasend �����ĵ�ַ
	OriginalRecv = (OriginalRecvType)GetProcAddress(GetModuleHandle("ws2_32.dll"), "recv");

	// �� wsasend �������� Hook�������滻Ϊ�Զ���� HookedWsaSend ����
	DetourAttach(&(PVOID&)OriginalRecv, HookedRecv);

	// ��� Hook ����
	DetourTransactionCommit();
}

void FreeRecv() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// �ָ�ԭʼ�� wsasend ����
	DetourDetach(&(PVOID&)OriginalRecv, HookedRecv);

	// ��� Hook ����
	DetourTransactionCommit();
}