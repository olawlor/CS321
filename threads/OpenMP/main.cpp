#include <iostream>
#include <thread>
#include <omp.h>

volatile int delay=0;
void do_work(char where) {
	for (int i=0;i<10;i++) {
		for (int j=0;j<1000;j++) delay++;
		std::cout<<where<<i<<"\n";
	}
}

int main() {

#pragma omp parallel for
	for (char w='A';w<='B';w++)
		do_work(w);

	std::cout<<"Done: total="<<delay<<"\n";
	return 0;
}

