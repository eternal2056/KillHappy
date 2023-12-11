#include "pch.h"

// 函数定义
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
	//addressList = getIPsByDomain(urlName);

	std::vector<std::string> bufferContents;
	//WriteToLogFile("dwBufferCount" + std::to_string(dwBufferCount));
	// 遍历缓冲区数组，将每个缓冲区的内容存储到 vector 中
	for (DWORD i = 0; i < dwBufferCount; ++i) {
		// 假设 lpBuffers[i].buf 是一个以null结尾的字符数组
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

	// 将 vector 中的字符串连接起来
	std::string resultString;
	for (std::string content : bufferContents) {
		resultString += content;

	}
	//WriteToLogFile("resultString.length()" + std::to_string(resultString.length()));
	extern std::vector<std::string> forbidAddressList;
	for (auto& forbidAddress : forbidAddressList) {
		if (resultString.find(forbidAddress) != resultString.npos) {
			isMatched = true;
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

// 定义函数指针类型，用于保存原始的 wsasend 函数
typedef INT(PASCAL FAR* LPFN_WSASEND)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

// 声明原始的 wsasend 函数指针
LPFN_WSASEND RealWsaSend = nullptr;

// 自定义的 HookedWsaSend 函数
INT PASCAL FAR HookedWsaSend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	GetPeerInfo(s, lpBuffers, dwBufferCount);
	if (isMatched) {
		isMatched = false;
		dwBufferCount = 0;
		return 1;
	}
	// 调用原始的 wsasend 函数
	INT result = RealWsaSend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);

	// 在这里可以添加你自己的逻辑

	return result;
}

void HookWsaSend() {
	readAndPrintFile("D:\\AddrList.txt");
	//MessageBox(NULL, "This Is From Dll!\nInject Success!", "OK", MB_OK);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 获取原始 wsasend 函数的地址
	RealWsaSend = (LPFN_WSASEND)GetProcAddress(GetModuleHandle("ws2_32.dll"), "WSASend");

	// 对 wsasend 函数进行 Hook，将其替换为自定义的 HookedWsaSend 函数
	DetourAttach(&(PVOID&)RealWsaSend, HookedWsaSend);

	// 完成 Hook 事务
	DetourTransactionCommit();
}

void FreeWsaSend() {
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	// 恢复原始的 wsasend 函数
	DetourDetach(&(PVOID&)RealWsaSend, HookedWsaSend);

	// 完成 Hook 事务
	DetourTransactionCommit();
}