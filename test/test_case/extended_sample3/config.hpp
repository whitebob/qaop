#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "queue.hpp"
#include "../../../qaop.h"
#include <iostream>

// Example 3: Add Count Aspect

namespace util {

//jointpoint is also implemented as an aspect.
//the joint points are set on general _Queue class and work a general _Item class
 
template < typename _Queue, typename _Item >
struct jointpoint_ : public _Queue::this_t {
typedef jointpoint_ this_t;
typedef typename _Queue::this_t base_t;
typedef typename _Queue::fulltype_t fulltype_t;

jointpoint_() {}

// Define jointpoints 
virtual void enqueue (_Item * item) {
	qaop::invoke(dynamic_cast<fulltype_t *>(this), &jointpoint_::enqueue, item);
};

virtual _Item *  dequeue () {
	return qaop::invoke(dynamic_cast<fulltype_t *>(this), &jointpoint_::dequeue);
}

}; //jointpoint_

//This could be easily changed to adopt different Item type.
template <typename _Class>
using Jointpoint_= jointpoint_ <_Class, util::Item>;


//As we can see, the count_ aspect has nothing to do with _Queue, it is gerneral enough.

template < typename _Class >
struct count_ : public _Class::this_t {
typedef count_ this_t;
typedef typename _Class::this_t base_t;
typedef typename _Class::fulltype_t fulltype_t;

typedef std::function<int()> closure_t;
int counter;
count_():counter(0) {}

closure_t count_advice_add (closure_t &f) {
	return [=](){
		f();
		this->counter++;
		return 0;
	};
}

closure_t count_advice_remove (closure_t &f) {
	return [=](){
		f();
		if(counter > 0)
			this->counter--;
		return 0;
	};
}
	
closure_t output_count (closure_t &f) {
	
	return [=](){
		f();
		std::cout<< "count:" <<count()<<std::endl;
		return 0;
	};
}

int count() const { return  counter;}

}; //count_


} // namespace util

namespace config {
	using Queue = typename qaop::Decorate<util::Queue>::with<util::Jointpoint_, util::count_>::type;
	using Item = util::Item;

	typedef  qaop::stub<Queue> Q;
	typedef  qaop::waven<Queue> L;
	template <typename _Callable>
		using action = qaop::action<Queue, _Callable>;

void initializtion() {

	//this is the core actions, which the util::Queue will do, the nullptr means it is default behavior.  
	static action<decltype(Q::_(&util::Queue::enqueue))> e{Q::wrap(&util::Queue::enqueue), nullptr};
	static action<decltype(Q::_r(&util::Queue::dequeue))> d{Q::wrap_r(&util::Queue::dequeue), nullptr};

	static action<decltype(Q::_(&util::Queue::enqueue))> ca{Q::wrap(&util::Queue::enqueue), Q::wrap(&Queue::count_advice_add)};
	static action<decltype(Q::_r(&util::Queue::dequeue))> cr{Q::wrap_r(&util::Queue::dequeue), Q::wrap(&Queue::count_advice_remove)};

	static action<decltype(Q::_(&util::Queue::enqueue))> be{Q::_(&util::Queue::enqueue, [=](){std::cout<<"before enqueue: ";return 0;}), Q::wrap(&Queue::output_count)};
	static action<decltype(Q::_(&util::Queue::enqueue))> ae{Q::_(&util::Queue::enqueue, [=](){std::cout<<"after enqueue: "; return 0;}), Q::wrap(&Queue::output_count)};
	
	static action<decltype(Q::_(&util::Queue::dequeue))> bd{Q::_(&util::Queue::dequeue, [=](){std::cout<<"before dequeue: "; return 0;}), Q::wrap(&Queue::output_count)};
	static action<decltype(Q::_r(&util::Queue::dequeue))> ad{Q::_r(&util::Queue::dequeue, [=](){std::cout<<"after dequeue: "; return 0;}), Q::wrap(&Queue::output_count)};

	L::insitu(&Queue::jointpoint_::enqueue, &e);
	L::insitu(&Queue::jointpoint_::dequeue, &d);

	L::insitu(&Queue::jointpoint_::enqueue, &ca);
	L::insitu(&Queue::jointpoint_::dequeue, &cr);
	
	L::before(&Queue::jointpoint_::enqueue, &be);
	L::after (&Queue::jointpoint_::enqueue, &ae);

	L::before(&Queue::jointpoint_::dequeue, &bd);
	L::after (&Queue::jointpoint_::dequeue, &ad);
}
static struct A {A(){initializtion();}} a;
} //namespace config 
#endif //_CONFIG_H_
