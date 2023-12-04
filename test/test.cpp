#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread> // 包含用于 sleep 的头文件

#pragma comment(lib, "Ws2_32.lib") // 链接到 Winsock 库

int main() {
    // 初始化 Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // 创建套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    // 设置目标地址和端口
    sockaddr_in targetAddr;
    targetAddr.sin_family = AF_INET;
    targetAddr.sin_port = htons(12345); // 你的目标端口
    inet_pton(AF_INET, "127.0.0.1", &(targetAddr.sin_addr)); // 你的目标 IP 地址

    // 准备要发送的数据
    const char* dataToSend = "Hello, UDP World!";

    // 设置循环标志
    bool shouldRun = true;

    while (shouldRun) {
        // 使用 sendto 发送数据
        int result = sendto(sock, dataToSend, strlen(dataToSend), 0, (const sockaddr*)&targetAddr, sizeof(targetAddr));
        if (result == SOCKET_ERROR) {
            std::cerr << "Failed to send data: " << WSAGetLastError() << "\n";
        }
        else {
            std::cout << "Data sent successfully\n";
        }

        // 暂停 3 秒钟
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // 关闭套接字和清理 Winsock
    closesocket(sock);
    WSACleanup();

    return 0;
}
