/**
  List files, using POSIX opendir/readdir
  Dr. Lawlor, lawlor@alaska.edu, 2019-03-03 (Public Domain)

 https://pubs.opengroup.org/onlinepubs/009696799/functions/opendir.html
 https://pubs.opengroup.org/onlinepubs/009696799/functions/readdir.html 
*/
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> // POSIX directory read

int main(int argc, const char *argv[]) {
  const char *dir_name=argv[1];
  if (argc<=1) dir_name=".";
  
  DIR *dir = opendir(dir_name);
  if (!dir) { perror("Error opening directory"); exit(1); }
  printf("Opened directory %p\n", dir);
  
  while (struct dirent *ent = readdir(dir)) {
    printf("  Entry: inode %ld, name '%s'  (entry at %p)\n", 
      (long)ent->d_ino, ent->d_name, ent);
  }
  closedir(dir);
  
  return 0;
}


