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
struct jointpoint_ : public _Queue::this_t {
typedef jointpoint_ this_t;
typedef typename _Queue::this_t base_t;
typedef typename _Queue::fulltype_t fulltype_t;

jointpoint_() {}

virtual void enqueue (_Item * item) {
	printf("invoke enqueue\n");
	qaop::invoke(dynamic_cast<fulltype_t *>(this), &jointpoint_::enqueue, item);
};

virtual _Item *  dequeue () {
	printf("invoke dequeue\n");
	return qaop::invoke(dynamic_cast<fulltype_t *>(this), &jointpoint_::dequeue);
}

};

template <typename _Class>
using Jointpoint_= jointpoint_ <_Class, util::Item>;

template < typename _Queue, typename _Item >
struct count_ : public _Queue::this_t {
typedef count_ this_t;
typedef typename _Queue::this_t base_t;
typedef typename _Queue::fulltype_t fulltype_t;

int counter;
count_():counter(0) {}

/*
 {counter++;}

 {counter--;}

*/
	

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

std::function<int()> & lock_advice (std::function<int()> & f) {
		return [this, &f]() {
			lock.lock();
			try {
				f();
			} catch (...) {
				lock.unlock();
				throw;
			}
			lock.unlock();
			return 0;
		};
}

};

template <typename _Class>
using Lock_= lock_ <_Class, util::Item>;

}

typedef qaop::Decorate<util::Queue>::with<util::Jointpoint_,util::Count_, util::Lock_>::type JointLockCountQueue;

int print(JointLockCountQueue * self){
	printf("Action called! %x \n", self);
	return 0;
}

int print_a(JointLockCountQueue * self, util::Item * &pi) {
	printf("Action called! Item %x, this %x \n", pi, self);
	return 0;
}

int print2(JointLockCountQueue * self, util::Item * pi) {
	printf("Action called! Item %x, this %x \n", pi, self);
	return 0;
}

int dequeue_w (JointLockCountQueue * self, util::Item *& pi) {
	pi = self->Queue::dequeue();
	return 0;
}



void initializtion() {
	using namespace util;
	typedef qaop::Decorate<Queue>::with<Jointpoint_, Count_, Lock_>::type JointLockCountQueue;
	
	static qaop::action<JointLockCountQueue, decltype(print)> m{print};
	static qaop::action<JointLockCountQueue, decltype(print_a)> m_a{print_a};
	static qaop::action<JointLockCountQueue, decltype(print2)> n{print2};
	static qaop::action<JointLockCountQueue, decltype(dequeue_w)> d{dequeue_w,nullptr};
	//static qaop::action<JointLockCountQueue, decltype(util::Queue::enqueue)> e{util::Queue::enqueue, nullptr};
	qaop::add_insitu<JointLockCountQueue>(&JointLockCountQueue::jointpoint_::dequeue, &d);
	//qaop::add_insitu<JointLockCountQueue>(&JointLockCountQueue::jointpoint_::enqueue, &e);
	qaop::add_before<JointLockCountQueue>(&JointLockCountQueue::jointpoint_::dequeue, &m);
	qaop::add_after<JointLockCountQueue>(&JointLockCountQueue::jointpoint_::dequeue, &m_a);
	qaop::add_before<JointLockCountQueue>(&JointLockCountQueue::jointpoint_::enqueue, &n);
}
static struct A {A(){initializtion();}} a;

int main(int argc, char **argv) {
	using namespace util;

	/* version 1 Begin
	Queue q; // version 1 End */
	
	/* verion 2 Begin
	typedef qaop::Decorate<Queue>::with<Count_>::type CountQueue;
	CountQueue q; //version 2 End */
	
	/* verion 3 Begin
	typedef qaop::Decorate<Queue>::with<Count_,Lock_>::type LockCountQueue;
	LockCountQueue q; //version 3 End */
	
	//* verion 4 Begin
	typedef qaop::Decorate<Queue>::with<Jointpoint_, Count_, Lock_>::type JointLockCountQueue;
	JointLockCountQueue q,p; //version 3 End */
	
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
	p.enqueue(&I1);
	p.dequeue();
	return 0;
};


