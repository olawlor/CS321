#include <iostream>
#include <thread>

volatile int delay=0;
void do_work(char where) {
	for (int i=0;i<10;i++) {
		for (int j=0;j<1000;j++) delay++;
		std::cout<<where<<i<<"\n";
	}
}

void *worker_thread(void *arg) {
	do_work('B');
	return 0;
}

int main() {
	pthread_t t;
	pthread_create(&t,NULL,worker_thread,0);

	do_work('A');

	void *ret=0;
	pthread_join(t,&ret);

	std::cout<<"Done: total="<<delay<<"\n";
	return 0;
}

