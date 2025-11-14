#include <windows.h>
#include <iostream>
#include <shlobj.h>
#include <string>

std::string GetCookiePath() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\Roblox\\LocalStorage\\RobloxCookies.dat";
    }
    return "";
}

int main() {
    HANDLE oldMutex = OpenMutex(SYNCHRONIZE, FALSE, L"ROBLOX_singletonMutex");
    if (oldMutex) CloseHandle(oldMutex);

    HANDLE oldEvent = OpenMutex(SYNCHRONIZE, FALSE, L"ROBLOX_singletonEvent");
    if (oldEvent) CloseHandle(oldEvent);

    HANDLE mutex1 = CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
    if (!mutex1 || GetLastError() == ERROR_ALREADY_EXISTS) return 1;

    HANDLE mutex2 = CreateMutex(NULL, TRUE, L"ROBLOX_singletonEvent");
    if (!mutex2 || GetLastError() == ERROR_ALREADY_EXISTS) {
        ReleaseMutex(mutex1);
        CloseHandle(mutex1);
        return 1;
    }

    std::cout << "Mutexes acquired successfully.\n";

    HANDLE cookieFile = INVALID_HANDLE_VALUE;
    std::string cookiePath = GetCookiePath();
    if (!cookiePath.empty()) {
        cookieFile = CreateFileA(cookiePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (cookieFile == INVALID_HANDLE_VALUE)
            std::cerr << "Failed to lock RobloxCookies.dat (" << GetLastError() << ").\n";
        else
            std::cout << "RobloxCookies.dat locked successfully.\n";
    }
    else {
        std::cerr << "Failed to get Roblox LocalStorage path.\n";
    }

    std::cout << "Press Enter to release and exit.\n";
    std::cin.get();

    if (cookieFile != INVALID_HANDLE_VALUE) CloseHandle(cookieFile);
    ReleaseMutex(mutex2);
    CloseHandle(mutex2);
    ReleaseMutex(mutex1);
    CloseHandle(mutex1);

    return 0;
}
