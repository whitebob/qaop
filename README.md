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

