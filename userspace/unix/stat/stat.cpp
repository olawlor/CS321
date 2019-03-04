/**
  Get info about a file, using stat
  Dr. Lawlor, lawlor@alaska.edu, 2019-03-03 (Public Domain)
  
  See also:
  https://pubs.opengroup.org/onlinepubs/009696799/functions/stat.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, const char *argv[]) {
  const char *file_name=argv[1];
  if (argc<=1) file_name=".";
  
  struct stat s;
  if (0!=stat(file_name,&s)) { perror("Error getting status"); exit(1); }
  
  printf("File length: %ld bytes\n", (long)s.st_size);
  printf("File access mode: %03o\n", (int)s.st_mode); // UNIX permissions in octal
  printf("Number of links: %d\n", (int)s.st_nlink);
  printf("Owning user ID: %d\n", (int)s.st_uid);
  printf("Owning group ID: %d\n", (int)s.st_gid);
  printf("Filesystem block size: %d bytes\n", (int)s.st_blksize);
  
  return 0;
}


