#include <iostream>
#include <thread>

volatile int delay=0;
void do_work(char where) {
	for (int i=0;i<10;i++) {
		for (int j=0;j<1000;j++) delay++;
		std::cout<<where<<i<<"\n";
	}
}

void worker_thread(void) {
	do_work('B');
}

int main() {
	std::thread t(worker_thread);
	do_work('A');
	t.join();
	std::cout<<"Done: total="<<delay<<"\n";
	return 0;
}

