#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main() {
	int fd=open("/proc/vmafile",O_RDWR);
	if (fd<0) { perror("Open"); return 0; }
	void *ptr=mmap(0,4096,
		PROT_READ, MAP_SHARED,
		fd,0);
	printf("Got ptr %p from mmap\n",ptr);
	
	FILE *f=fopen("/proc/self/maps","r");
	while (!feof(f)) {
		char c=fgetc(f);
		putchar(c);
	}

	printf("Reading ptr %p\n",ptr);
	int *ip=(int *)ptr;
	printf("Value is %d\n",*ip);

	return 0;
}

