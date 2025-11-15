#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <set>

const std::vector<std::wstring> RobloxInstances = {
    L"RobloxPlayerBeta.exe",
    L"RobloxPlayer.exe",
    L"Roblox.exe",
    L"RobloxStudioBeta.exe"
};

bool IsRobloxInstance(const std::wstring& procName) {
    for (const auto& target : RobloxInstances) {
        if (_wcsicmp(procName.c_str(), target.c_str()) == 0)
            return true;
    }
    return false;
}

SIZE_T GetWorkingSetSize(HANDLE hProcess) {
    PROCESS_MEMORY_COUNTERS pmc = { 0 };
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

int TrimRobloxInstances(DWORD priorityClass) {
    int totalTrimmed = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32W) };
    if (!Process32FirstW(hSnapshot, &pe)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    do {
        if (IsRobloxInstance(pe.szExeFile)) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_SET_QUOTA | PROCESS_VM_READ | PROCESS_SET_INFORMATION, FALSE, pe.th32ProcessID);
            if (hProcess) {
                SIZE_T before = GetWorkingSetSize(hProcess);
                if (before > 0) {
                    BOOL success = EmptyWorkingSet(hProcess);
                    if (success && priorityClass != 0) {
                        SetPriorityClass(hProcess, priorityClass);
                    }
                    totalTrimmed++;
                }
                CloseHandle(hProcess);
            }
        }
    } while (Process32NextW(hSnapshot, &pe));

    CloseHandle(hSnapshot);
    return totalTrimmed;
}

std::string GetCookiePath() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\Roblox\\LocalStorage\\RobloxCookies.dat";
    }
    return "";
}

int main() {
    SetConsoleTitle(L"Roblox Instance Manager");

    std::wcout << L"Mutexes opened.\n";

    HANDLE mutex1 = CreateMutex(NULL, TRUE, L"ROBLOX_singletonMutex");
    if (!mutex1 || GetLastError() == ERROR_ALREADY_EXISTS) {
        std::wcerr << L"Failed to open mutex 'ROBLOX_singletonMutex'. Another instance may be running.\n";
        return 1;
    }

    HANDLE mutex2 = CreateMutex(NULL, TRUE, L"ROBLOX_singletonEvent");
    if (!mutex2 || GetLastError() == ERROR_ALREADY_EXISTS) {
        ReleaseMutex(mutex1);
        CloseHandle(mutex1);
        std::wcerr << L"Failed to open mutex 'ROBLOX_singletonEvent'. Another instance may be running.\n";
        return 1;
    }

    HANDLE cookieFile = INVALID_HANDLE_VALUE;
    std::string cookiePath = GetCookiePath();
    if (!cookiePath.empty()) {
        cookieFile = CreateFileA(cookiePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (cookieFile != INVALID_HANDLE_VALUE) {
            std::wcout << L"Cookie file locked.\n";
        }
        else {
            std::wcout << L"Failed to lock cookie file. Game may be broken or load incorrectly.\n";
        }
    }

    std::wcout << L"Waiting for Roblox instances...\n";

    std::set<DWORD> seenPIDs;


    while (true) {
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot != INVALID_HANDLE_VALUE) {
            PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32W) };
            if (Process32FirstW(hSnapshot, &pe)) {
                do {
                    if (IsRobloxInstance(pe.szExeFile)) {
                        if (seenPIDs.find(pe.th32ProcessID) == seenPIDs.end()) {
                            std::wcout << L"New Roblox instance detected: " << pe.szExeFile << L" (PID: " << pe.th32ProcessID << L")\n";
                            seenPIDs.insert(pe.th32ProcessID);
                        }
                    }
                } while (Process32NextW(hSnapshot, &pe));
            }
            CloseHandle(hSnapshot);
        }

        if (seenPIDs.size() > 0) {
            std::wcout << L"Enable memory trimming for all Roblox instances? (y/n): ";
            char enableTrimming = 'n';
            std::cin >> enableTrimming;

            if (enableTrimming == 'y' || enableTrimming == 'Y') {
                std::wcout << L"Enter trim interval (minutes, recommended: 60): ";
                int minutes = 60;
                std::cin >> minutes;

                std::wcout << L"Choose priority for all Roblox instances:\n";
                std::wcout << L"1. Normal\n";
                std::wcout << L"2. Below Normal\n";
                std::wcout << L"3. Idle\n";
                std::wcout << L"4. High\n";
                std::wcout << L"5. Realtime\n";
                std::wcout << L"Enter choice (1-5): ";
                int priorityChoice = 1;
                std::cin >> priorityChoice;

                DWORD priorityClass = NORMAL_PRIORITY_CLASS;
                switch (priorityChoice) {
                case 2: priorityClass = BELOW_NORMAL_PRIORITY_CLASS; break;
                case 3: priorityClass = IDLE_PRIORITY_CLASS; break;
                case 4: priorityClass = HIGH_PRIORITY_CLASS; break;
                case 5: priorityClass = REALTIME_PRIORITY_CLASS; break;
                default: priorityClass = NORMAL_PRIORITY_CLASS; break;
                }

                std::wcout << L"Trimming enabled for all Roblox instances. Interval: " << minutes << L" minutes.\n";
                while (true) {
                    int trimmedCount = TrimRobloxInstances(priorityClass);
                    if (trimmedCount > 0) {
                        std::wcout << L"Trimmed " << trimmedCount << L" Roblox instance(s) this cycle.\n";
                    }
                    else {
                        std::wcout << L"No Roblox instances found this cycle.\n";
                    }
                    std::this_thread::sleep_for(std::chrono::minutes(minutes));
                }
            }
            else {
                std::wcout << L"Trimming disabled.\n";
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (cookieFile != INVALID_HANDLE_VALUE) CloseHandle(cookieFile);
    ReleaseMutex(mutex2);
    CloseHandle(mutex2);
    ReleaseMutex(mutex1);
    CloseHandle(mutex1);

    return 0;
}
