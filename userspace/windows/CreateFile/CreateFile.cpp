/* Windows file input example */
#include <stdio.h> // for printf
#include <windows.h> 

int main()
{
    HANDLE hFile=CreateFile("example.txt",GENERIC_READ,
        0, 0, OPEN_EXISTING, 0, 0);
    if (hFile==INVALID_HANDLE_VALUE) { 
        printf("File open error %d\n", (int)GetLastError()); exit(1); 
    }
    
    const int buflen=1000;
    char buf[buflen];
    DWORD nread=0;
    BOOL OK=ReadFile(hFile,buf,buflen,&nread,0);
    if (!OK) { printf("Can't read any bytes from file.\n"); exit(1); }
    buf[nread]=0; // add nul terminator
    printf("Read %d bytes from file: %s\n",(int)nread,buf);
    
    CloseHandle(hFile);
    return 0;
}

