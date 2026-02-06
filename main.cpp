#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <set>
#include <spdlog/spdlog.h>

const std::vector<std::string> RobloxInstances = {
    "RobloxPlayerBeta.exe",
    "RobloxPlayer.exe",
    "Roblox.exe",
};

std::set<DWORD> knownPIDs;

bool IsRobloxInstance(const std::string& procName) {
    for (const auto& target : RobloxInstances) {
        if (_stricmp(procName.c_str(), target.c_str()) == 0)
            return true;
    }
    return false;
}

SIZE_T GetWorkingSetSize(HANDLE hProcess) {
    PROCESS_MEMORY_COUNTERS pmc = {};
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
    return 0;
}

std::string GetCookiePath() {
    char path[MAX_PATH] = {};
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path))) {
        return std::string(path) + "\\Roblox\\LocalStorage\\RobloxCookies.dat";
    }
    return "";
}

void TrimMemory(HANDLE hProcess, DWORD pid, SIZE_T memUsed) {
    constexpr SIZE_T LIMIT = 2500ull * 1024 * 1024;
    if (memUsed > LIMIT) {
        if (EmptyWorkingSet(hProcess))
            spdlog::info("Force trimmed memory for PID {} ({} MB)", pid, memUsed / 1024 / 1024);
        else
            spdlog::error("Failed to force trim memory for PID {}", pid);
    }
}

int main() {
    spdlog::info("Starting Roblox instance monitoring...");

    HANDLE mutex = CreateMutexA(NULL, TRUE, "ROBLOX_singletonMutex");
    if (!mutex || GetLastError() == ERROR_ALREADY_EXISTS) {
        spdlog::error("Another instance is already running.");
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return 1;
    }

    std::string cookiePath = GetCookiePath();
    HANDLE cookieFile = INVALID_HANDLE_VALUE;
    if (!cookiePath.empty()) {
        cookieFile = CreateFileA(cookiePath.c_str(), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (cookieFile != INVALID_HANDLE_VALUE) {
            spdlog::info("Roblox cookie file locked for write-only access.");
        }
        else {
            spdlog::error("Unable to lock Roblox cookie file for write-only.");
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }
    else {
        spdlog::error("Roblox cookie path not found.");
        std::this_thread::sleep_for(std::chrono::seconds(10));
        return 1;
    }

    constexpr int INTERVAL_MS = 1000;

    while (true) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            spdlog::error("Failed to create process snapshot.");
            std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_MS));
            continue;
        }

        PROCESSENTRY32 pe = { sizeof(pe) };
        std::set<DWORD> currentPIDs;
        std::vector<std::pair<DWORD, SIZE_T>> processes;
        SIZE_T totalMem = 0;

        if (Process32First(snapshot, &pe)) {
            do {
                std::string exeName = std::string(pe.szExeFile);
                auto pos = exeName.find_last_of("\\");
                if (pos != std::string::npos) {
                    exeName = exeName.substr(pos + 1);
                }

                if (IsRobloxInstance(exeName)) {
                    currentPIDs.insert(pe.th32ProcessID);
                    if (knownPIDs.find(pe.th32ProcessID) == knownPIDs.end()) {
                        spdlog::info("New Roblox instance detected, PID: {}", pe.th32ProcessID);
                        knownPIDs.insert(pe.th32ProcessID);
                    }

                    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_SET_QUOTA, FALSE, pe.th32ProcessID);
                    if (hProcess) {
                        SIZE_T memUsed = GetWorkingSetSize(hProcess);
                        processes.emplace_back(pe.th32ProcessID, memUsed);
                        totalMem += memUsed;
                        TrimMemory(hProcess, pe.th32ProcessID, memUsed);
                        CloseHandle(hProcess);
                    }
                    else {
                        spdlog::warn("Failed to open process PID {}", pe.th32ProcessID);
                    }
                }
            } while (Process32Next(snapshot, &pe));
        }
        CloseHandle(snapshot);

        for (auto it = knownPIDs.begin(); it != knownPIDs.end();) {
            if (currentPIDs.find(*it) == currentPIDs.end()) {
                spdlog::warn("Roblox instance PID {} no longer running - possible crash or user closed it.", *it);
                it = knownPIDs.erase(it);
            }
            else {
                ++it;
            }
        }

        std::stringstream title;
        title << "Roblox Instances: " << processes.size() << " | ";
        size_t count = 0;
        for (const auto& p : processes) {
            double memMB = p.second / 1024.0 / 1024.0;
            title << "PID " << p.first << ": " << std::fixed << std::setprecision(1) << memMB << " MB; ";
            if (++count >= 5) {
                title << "... ";
                break;
            }
        }
        double totalMB = totalMem / 1024.0 / 1024.0;
        title << "Total: " << std::fixed << std::setprecision(1) << totalMB << " MB";

        SetConsoleTitleA(title.str().c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_MS));
    }

    if (cookieFile != INVALID_HANDLE_VALUE) {
        CloseHandle(cookieFile);
    }

    ReleaseMutex(mutex);
    CloseHandle(mutex);
    return 0;
}
