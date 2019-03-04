/**
  Open and read a file using bare UNIX syscalls open/read
  Dr. Lawlor, lawlor@alaska.edu, 2019-03-03 (Public Domain)
  
  See also:
  https://pubs.opengroup.org/onlinepubs/009696799/functions/open.html
  https://pubs.opengroup.org/onlinepubs/009696799/functions/read.html
*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, const char *argv[]) {
  const char *file_name=argv[1];
  if (argc<=1) file_name="Makefile";
  
  int fd=open(file_name,O_RDONLY);
  if (fd<0) { perror("Error opening file for read"); exit(1); }
  
  const int bufsize=100;
  char buf[bufsize+1];
  ssize_t nbytes=read(fd, buf, bufsize);
  if (nbytes<0) { perror("Error reading any bytes from file"); exit(1); }
  printf("Read %ld bytes:\n",(long)nbytes);
  buf[nbytes]=0; 
  printf("%s",buf);
  
  int err=close(fd);
  if (err!=0) { perror("Error closing file"); exit(1); }
  
  return 0;
}


