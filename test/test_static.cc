#include<iostream>
#include<typeinfo>
#include"../qaop.h"


struct base {
	virtual int test() {return 0;}
};	

void initialization();

struct A {
	A(){initialization();}
};

template<typename _Class>
struct AspectA : public _Class::this_t {
	typedef AspectA this_t;
	typedef typename _Class::fulltype_t fulltype_t;	
	const static qaop::static_proxy<this_t> sp;
	virtual int foo () {return 0;}
	static int get_a(){ 
		std::cout<<"get_a called in AspectA"<<std::endl;
		return sp.template proxy<int, qaop::Name("AspectA::a")>();
	} 
};


template<typename _Class>
struct AspectB : public _Class::this_t {
	typedef AspectB this_t;
	typedef typename _Class::fulltype_t fulltype_t;
	virtual int bar() { return 0;}
};

typedef qaop::Decorate<base>::with<AspectA, AspectB >::type Final;
typedef qaop::Decorate<base>::with<AspectB, AspectA >::type Final2;


void  initialization() {
	std::cout<<"initialization start"<<std::endl;
	qaop::proxy<Final, int, qaop::Name("AspectA::a")>(1);
	qaop::proxy<Final2, int, qaop::Name("AspectA::a")>(2);
	std::cout<<"initialization end"<<std::endl;
};

static A a;
int main(int argc, char** argv) {
	Final o;
	o.foo();
	o.bar();
	std::cout<<typeid(o).name()<<std::endl;
	std::cout<<Final::get_a()<<std::endl;
	Final2 o2;
	std::cout<<Final2::get_a()<<std::endl;
	return 0;
}
