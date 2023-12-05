#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#undef UNICODE

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
int __cdecl main(int argc, char** argv)
{

    //-----------------------------------------
    // Declare and initialize variables
    WSADATA wsaData;
    int iResult;
    INT iRetval;

    DWORD dwRetval;

    int i = 1;

    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

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
        return 1;
    }

    //--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    dwRetval = getaddrinfo("www.bilibili.com", 0, &hints, &result);
    if (dwRetval != 0) {
        printf("getaddrinfo failed with error: %d\n", dwRetval);
        WSACleanup();
        return 1;
    }

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        switch (ptr->ai_family) {
        case AF_INET:
            sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            printf("IPv4 address %s\n",
                inet_ntoa(sockaddr_ipv4->sin_addr));
            break;
        case AF_INET6:
            // the InetNtop function is available on Windows Vista and later
            // sockaddr_ipv6 = (struct sockaddr_in6 *) ptr->ai_addr;
            // printf("\tIPv6 address %s\n",
            //    InetNtop(AF_INET6, &sockaddr_ipv6->sin6_addr, ipstringbuffer, 46) );

            // We use WSAAddressToString since it is supported on Windows XP and later
            sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
            // The buffer length is changed by each call to WSAAddresstoString
            // So we need to set it for each iteration through the loop for safety
            ipbufferlength = 46;
            iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
                ipstringbuffer, &ipbufferlength);
            if (iRetval)
                printf("WSAAddressToString failed with %u\n", WSAGetLastError());
            else
                printf("IPv6 address %s\n", ipstringbuffer);
            break;
        }
    }

    freeaddrinfo(result);
    WSACleanup();

    return 0;
}