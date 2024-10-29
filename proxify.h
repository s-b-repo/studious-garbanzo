#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include "windivert.h"

#pragma comment(lib, "Ws2_32.lib")

const char* proxy_ip = "proxy_ip_address";  // Replace with the proxy IP
const uint16_t proxy_port = 8080;           // Replace with the proxy port
const uint32_t target_pid = 1234;           // Replace with the target application PID

// Forwards traffic to the proxy, which should route it to the final destination
bool forward_to_proxy(SOCKET& proxy_sock, char* packet_data, UINT packet_len) {
    // Send the captured packet to the proxy
    if (send(proxy_sock, packet_data, packet_len, 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send data to proxy.\n";
        return false;
    }
    return true;
}

// Receives the response from the proxy and relays it back to the original destination
bool receive_from_proxy(SOCKET& proxy_sock, char* packet_data, UINT& packet_len) {
    int recv_len = recv(proxy_sock, packet_data, packet_len, 0);
    if (recv_len == SOCKET_ERROR || recv_len == 0) {
        std::cerr << "Failed to receive data from proxy.\n";
        return false;
    }
    packet_len = recv_len;
    return true;
}

int main() {
    HANDLE handle;
    WINDIVERT_ADDRESS addr;
    char packet[MAX_PACKET_SIZE];
    UINT packet_len;

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Connect to the proxy
    SOCKET proxy_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (proxy_sock == INVALID_SOCKET) {
        std::cerr << "Failed to create socket.\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in proxy_addr;
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(proxy_port);
    proxy_addr.sin_addr.s_addr = inet_addr(proxy_ip);

    if (connect(proxy_sock, (sockaddr*)&proxy_addr, sizeof(proxy_addr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to proxy.\n";
        closesocket(proxy_sock);
        WSACleanup();
        return 1;
    }

    // Open a WinDivert handle to capture outgoing TCP packets from the target PID
    handle = WinDivertOpen("outbound and ip and tcp", WINDIVERT_LAYER_NETWORK, 0, 0);
    if (handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open WinDivert handle.\n";
        closesocket(proxy_sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Capturing packets from PID " << target_pid << " and forwarding through proxy.\n";

    // Packet capture loop
    while (true) {
        if (!WinDivertRecv(handle, packet, sizeof(packet), &addr, &packet_len)) {
            std::cerr << "Failed to receive packet.\n";
            continue;
        }

        // Filter packets by process ID
        if (addr.ProcessId == target_pid) {
            std::cout << "Forwarding packet from PID " << target_pid << " through proxy.\n";
            
            // Forward packet data to the proxy
            if (forward_to_proxy(proxy_sock, packet, packet_len)) {
                // Receive response from the proxy
                if (receive_from_proxy(proxy_sock, packet, packet_len)) {
                    // Send the modified packet back to its original destination
                    if (!WinDivertSend(handle, packet, packet_len, &addr, nullptr)) {
                        std::cerr << "Failed to send packet.\n";
                    }
                }
            }
        }
    }

    // Clean up resources
    WinDivertClose(handle);
    closesocket(proxy_sock);
    WSACleanup();
    return 0;
}
