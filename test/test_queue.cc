#include<cstdio>
#include<mutex>
#include "../qaop.h"
namespace util {
	class Item {
		friend class Queue;
		Item* next;
		public:
		Item() : next(0){}
	};
	class Queue {
			Item* first;
			Item* last;
		public:
			Queue() : first(0), last(0) {}
			void enqueue( Item* item ) {
				printf( "  > Queue::enqueue()\n" );
				if( last ) {
					last->next = item;
					last = item;
				} else
					last = first = item;
				printf( "  < Queue::enqueue()\n" );
			}
			Item* dequeue() {
				printf("  > Queue::dequeue()\n");
				Item* res = first;
				if( first == last )
					first = last = 0;
				else
					first = first->next;
				printf("  < Queue::dequeue()\n");
				return res;
			}
	}; // class Queue
} // namespace util

namespace util {

template < typename _Queue, typename _Item >
struct count_ : public _Queue::this_t {
typedef count_ this_t;
typedef typename _Queue::this_t base_t;
typedef typename _Queue::fulltype_t fulltype_t;

int counter;
count_():counter(0) {}
void enqueue (_Item * item) {
	base_t::enqueue(item);
	counter++;
	}
_Item * dequeue() {
	Item * res = base_t::dequeue();
	if (counter > 0) counter--;
        return res;	
	}
int count() const { return  counter;}
};

template <typename _Class>
using Count_= count_ <_Class, util::Item>;


template < typename _Queue, typename _Item >
struct lock_ : public _Queue::this_t {
	typedef lock_ this_t;
	typedef typename _Queue::this_t base_t;
	typedef typename _Queue::fulltype_t fulltype_t;
	
	std::mutex lock;
	void enqueue(_Item * item) {
		lock.lock();
		try {
			base_t::enqueue(item);
		} catch (...) {
			lock.unlock();
			throw;
		}
		lock.unlock();
	}
	_Item *  dequeue() {
		_Item * res;
		lock.lock();
		try {
			res = base_t::dequeue();
		} catch (...) {
			lock.unlock();
			throw;
		}
		lock.unlock();
		return res;
	}
};

template <typename _Class>
using Lock_= lock_ <_Class, util::Item>;

}


int main(int argc, char **argv) {
	using namespace util;

	/* version 1 Begin
	Queue q; // version 1 End */
	
	/* verion 2 Begin
	typedef qaop::Decorate<Queue>::with<Count_>::type CountQueue;
	CountQueue q; //version 2 End */
	
	//* verion 3 Begin
	typedef qaop::Decorate<Queue>::with<Count_,Lock_>::type LockCountQueue;
	LockCountQueue q; //version 3 End */
	
	Item I1, I2, I3;
	q.enqueue(&I1);
	printf( "count: %d \n",q.count());
	q.enqueue(&I2);
	printf( "count: %d \n",q.count());
	q.enqueue(&I3);
	printf( "count: %d \n",q.count());
	q.dequeue();
	printf( "count: %d \n",q.count());
	q.dequeue();
	printf( "count: %d \n",q.count());
	q.dequeue();
	printf( "count: %d \n",q.count());
	return 0;
};
