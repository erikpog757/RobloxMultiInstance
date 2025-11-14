#include <windows.h>
#include <iostream>

int main() {
    // Attempt to close any existing Roblox mutex handles to clear old locks
    HANDLE oldMutex = OpenMutex(SYNCHRONIZE, FALSE, L"ROBLOX_singletonMutex");
    if (oldMutex) {
        CloseHandle(oldMutex);
        std::cout << "Closed existing ROBLOX_singletonMutex\n";
    }

    HANDLE oldEvent = OpenMutex(SYNCHRONIZE, FALSE, L"ROBLOX_singletonEvent");
    if (oldEvent) {
        CloseHandle(oldEvent);
        std::cout << "Closed existing ROBLOX_singletonEvent\n";
    }

    // Create and acquire Roblox mutexes to block Roblox from creating its own
    HANDLE robloxMutex = CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
    if (!robloxMutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cerr << "Failed to create ROBLOX_singletonMutex or it already exists.\n";
        return 1;
    }
    HANDLE robloxEvent = CreateMutex(NULL, TRUE, L"ROBLOX_singletonEvent");
    if (!robloxEvent || GetLastError() == ERROR_ALREADY_EXISTS) {
        std::cerr << "Failed to create ROBLOX_singletonEvent or it already exists.\n";
        ReleaseMutex(robloxMutex);
        CloseHandle(robloxMutex);
        return 1;
    }

    std::cout << "Mutexes acquired successfully.\n";

    // Launch Roblox using the URI scheme to pass the parameters exactly as requested
    LPCWSTR robloxURI = L"roblox-player:1+launchmode:app+channel:production";

    HINSTANCE result = ShellExecute(NULL, L"open", robloxURI, NULL, NULL, SW_SHOWNORMAL);
    if ((INT_PTR)result <= 32) {
        std::cerr << "Failed to launch Roblox via URI, error code: " << (INT_PTR)result << "\n";
        ReleaseMutex(robloxMutex);
        CloseHandle(robloxMutex);
        ReleaseMutex(robloxEvent);
        CloseHandle(robloxEvent);
        return 1;
    }

    std::cout << "Roblox launched successfully. Press Enter to release mutexes and exit.\n";

    // Keep the program running and mutexes locked until user input
    std::cin.get();

    // Release and close mutexes properly before exiting
    ReleaseMutex(robloxMutex);
    CloseHandle(robloxMutex);
    ReleaseMutex(robloxEvent);
    CloseHandle(robloxEvent);

    return 0;
}
