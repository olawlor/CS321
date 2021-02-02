/* Windows example of listing files in a directory.

See: https://stackoverflow.com/questions/883594/microsoft-visual-studio-opendir-and-readdir-how
*/
#include <stdio.h> // for printf
#include <windows.h> // for FindFirstFile

int main() 
{
    WIN32_FIND_DATA findData;
    HANDLE hFind=FindFirstFile("*.txt", &findData);

    if(hFind != INVALID_HANDLE_VALUE) {
        printf("Found files: \n");
        do {
            printf("  %s (%x)\n", findData.cFileName, findData.dwFileAttributes);
        }
        while(FindNextFile(hFind, &findData));
        
        FindClose(hFind);
    }
    return 0;
}

