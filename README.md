# qaop 2.0
From this version: 

Some changes will be brougt to the whole architect.

0. The license will be Apache License 2.0. I do want more people to use it and not worried about opening their code, especially as a framework. Surely Pull Request is welcome if you really think this code is not totally piece of junk.

1. The interface of waven and use jointpointAOP class is too complex, and the syntax is somehow weird (please forgive my vanity :p). In this version the interface will be much simpler.  

2. previous aspect deocration strongly depends on the sequence. 
```
qaop::Decorate<Base>::with<AspectFoo, AspectBar>::type
```
is different with 
```
qaop::Decorate<Base>::with<AspectBar, AspectFoo>::type
```
, which they should be. In this new version, we will try to implement the composite type with set structure.
 
3. In the OOP scenario, if class B is derived from A, after using AOP enchance, we expect aop DecoratedB "ISA" DecoratedA, which is not true in version qaop 1.0. In fact, DecoratedA "ISA" A, and DecoratedB "ISA" B and also "ISA" A; however, DecoratedB don't have "ISA" relationship with DecoratedA.  It is obviously not good enough. Considering we have the following code:
```
void funcBase(A* p) {
    p->foo();
} 
void funcDerived(B* p) {
    p->foo();
    p->bar();
} 
```
We could easily use DecoratedA pointer for funcBase and DecoratedB pointer for funcDerived.
A simple trick will do, everything goes fine.  
```
using A = DecoratedA;
using B = DecoratedB;
```
However, when we want to use the decorated fulltype as interface, it is like this:
```
virtual DecoratedA * someVirtualFunc() {
    ....
}

virtual DecoratedB * someVirtualFunc() {
    ....
}

```
we could see the DecoratedB is not covariant with DecoratedA anymore, which might break the original design.
we will try to solve this in version 2.0


# qaop
QAOP is a header only C++ AOP framwork. 

The very intuitive AOP implemention is the template subclass pattern:
```
template<typename _Base>
class AspectA : public _Base {
...
};

template<typename _Base>
class AspectB : public _Base {
...
};

class Base {
...
};

typedef AspectB<AspectA<Base> > Combined;
```
However, there are some problems with this solution.

1. You could only return the _Base * pointer or reference. 
	Because for each Aspect, the final class info is unkown, and you can not pass the fulltype as a template parameter, because the combined fulltype defination needs the aspects.
	This is a typical "Chick or Egg" problem. Just as Hugo Arregui demostrate in ACCU paper(He used the CRTP to solve the problem). see https://github.com/hugoArregui/CPP_AOP-CRTP 
2. If you have static varibables in the Apsect templates, it will be hard to give a easy way to initialize it.
	Though the public static member is inherited, but for initialization, one must give out the full instance for the template. And this means, you can not write like this :
```c++
template<> Combined::static_memberA(xxx);
```
but have to write like this

```c++
template<> Combined::AspectB::AspectA::static_memberA(xxx); 
```

3. Generally you may have to use specific virtual function to realize the jointpoint, which would be very hard to port to other classes.

## QAOP will make all these things simpler and intuitive.

The aspects will be quite similar to prevous ones, just like this:
```c++
template <typename _Base>

class AspectA : public _Base::this_t {
using _Base::fulltype_t;
...
static static_proxy<fulltype_t> sp;
static int get_a() {
	return sp.template proxy<int, qaop::Name("a")>();
}

std::function<int()> AdviceA ( std::function<int()> & f) {
...
}
 
double ActionA ( int a, int b, char * c) {
...
} //suppose base::foo has interface foo (int, int, char *)

```
And in the client code, just using the following code to do the initialization and insertation of jointpoints and advices:

```c++
typedef Decorate<base>::with<Jointpoints, AspectA, AspectB...>::type CombinedType; 
void initialaztion{
	proxy<CombinedType, int, qaop::Name("a")>(5);
static  action<decltype(stub::_(&Base::foo))> a1(stub::wrap(CombinedType::ActionA));
static  action<decltype(stub::_(&Base::foo))> a2(stub::_(&Base::foo), stub::wrap(&AdviceA));
static  action<decltype(stub::_(&Base::foo))> a3(stub::_(&Base::foo, [](){std::cout<<"Let's try some Lambda!"<<std::endl;}));
	waven::before(&Base::foo, &a1);
	waven::insitu(&Base::foo, &a2);
	waven::after(&Base::foo, &a3);
}
```
Thus we can see, now the implemention of aspect has no fixed relationship with the base class and thus could be easily ported to other cases.
And the initialaztion could be represented without the implement detail. Best of all, the client code needs nearly no change at all!

This could be perfectly demostrated by the test_case. You can see all the files are exactly the same except for the _magic_ 'config.hpp'. In fact, I use symbol links for all the other files : )

