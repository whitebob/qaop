#ifndef _QUEUE_H_
#define _QUEUE_H_
namespace util {
	class Item {
		friend class Queue;
		Item* next;
		public:
		Item() : next(nullptr){}
	};

	class Queue {
			Item* first;
			Item* last;
		public:
			Queue() : first(nullptr), last(nullptr) {}
			void enqueue( Item* item ) {
				if (last) {
					last->next = item;
					last = item;
				} else
					last = first = item;
			}
			Item* dequeue() {
				Item* res = first;
				if( first == last )
					first = last = nullptr;
				else
					first = first->next;
				return res;
			}
	}; // class Queue
} // namespace util
#endif // _QUEUE_H_
