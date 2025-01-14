#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void HandleClient(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    while (true) {
        // Receive data from the client
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Error receiving data! Error: " << WSAGetLastError() << std::endl;
            break;
        }
        if (bytesReceived == 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        // Null-terminate the received data and print it
        buffer[bytesReceived] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    // Close the client socket
    closesocket(clientSocket);
    std::cout << "Client socket closed." << std::endl;
}

int main() {
    // Initialize WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed! Error: " << WSAGetLastError() << std::endl;
        return -1;
    }

    // Create a listening socket
    SOCKET listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listeningSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // Set up the server address structure
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the address and port
    if (bind(listeningSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Binding failed! Error: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return -1;
    }

    // Listen for incoming connections
    if (listen(listeningSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listening failed! Error: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    // Accept incoming connections
    sockaddr_in clientAddress;
    int clientSize = sizeof(clientAddress);
    SOCKET clientSocket = accept(listeningSocket, (sockaddr*)&clientAddress, &clientSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed! Error: " << WSAGetLastError() << std::endl;
        closesocket(listeningSocket);
        WSACleanup();
        return -1;
    }

    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddress.sin_addr, clientIP, INET_ADDRSTRLEN);
    std::cout << "Connection accepted from " << clientIP << ":" << ntohs(clientAddress.sin_port) << std::endl;

    // Handle client communication
    HandleClient(clientSocket);

    // Cleanup
    closesocket(listeningSocket);
    WSACleanup();
    return 0;
}
