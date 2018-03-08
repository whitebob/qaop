#include "config.hpp"

int main(int argc, char **argv) {
	using namespace config;

	Item I1, I2, I3;
	Queue q;	

	q.enqueue(&I1);
	q.enqueue(&I2);
	q.enqueue(&I3);

	Item * pItm = nullptr;

	pItm = q.dequeue();
	pItm = q.dequeue();
	pItm = q.dequeue();

	return 0;
};
