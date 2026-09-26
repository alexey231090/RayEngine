#include "ProcessRunner.hpp"
#include <iostream>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace REngine {

static HANDLE s_gameProcessHandle = NULL;
static DWORD s_gameProcessId = 0;

std::string ProcessRunner::GetExecutablePath() {
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        return std::string(buffer);
    }
    return "RaylibEngineApp.exe";
}

bool ProcessRunner::LaunchGameProcess(const std::string& scenePath) {
    if (IsGameRunning()) {
        StopGameProcess();
    }

    std::string exePath = GetExecutablePath();
    std::string cmdLine = "\"" + exePath + "\" --game --scene \"" + scenePath + "\"";

    std::cout << "[REngine] [Process] Spawning standalone game process: " << cmdLine << std::endl;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cmdBuffer[1024];
    strncpy(cmdBuffer, cmdLine.c_str(), sizeof(cmdBuffer) - 1);
    cmdBuffer[sizeof(cmdBuffer) - 1] = '\0';

    BOOL success = CreateProcessA(
        NULL,
        cmdBuffer,
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (success) {
        s_gameProcessHandle = pi.hProcess;
        s_gameProcessId = pi.dwProcessId;
        CloseHandle(pi.hThread); // Thread handle not needed
        std::cout << "[REngine] [Process] Game process launched successfully (PID: " << s_gameProcessId << ")" << std::endl;
        return true;
    } else {
        std::cerr << "[REngine] [Process] Failed to launch game process. Error: " << GetLastError() << std::endl;
        return false;
    }
}

bool ProcessRunner::IsGameRunning() {
    if (s_gameProcessHandle == NULL) {
        return false;
    }

    DWORD exitCode = 0;
    if (GetExitCodeProcess(s_gameProcessHandle, &exitCode)) {
        if (exitCode == STILL_ACTIVE) {
            return true;
        }
    }

    // Process has exited
    CloseHandle(s_gameProcessHandle);
    s_gameProcessHandle = NULL;
    s_gameProcessId = 0;
    return false;
}

void ProcessRunner::StopGameProcess() {
    if (s_gameProcessHandle != NULL) {
        std::cout << "[REngine] [Process] Terminating game process (PID: " << s_gameProcessId << ")" << std::endl;
        TerminateProcess(s_gameProcessHandle, 0);
        CloseHandle(s_gameProcessHandle);
        s_gameProcessHandle = NULL;
        s_gameProcessId = 0;
    }
}

} // namespace REngine

#else
// Non-windows fallback
namespace REngine {
std::string ProcessRunner::GetExecutablePath() { return "./RaylibEngineApp"; }
bool ProcessRunner::LaunchGameProcess(const std::string&) { return false; }
bool ProcessRunner::IsGameRunning() { return false; }
void ProcessRunner::StopGameProcess() {}
}
#endif
