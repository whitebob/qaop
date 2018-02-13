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
tempalte<> Combined::static_memberA(xxx);
```
but have to write like this

```c++
template<> Combined::AspectB::AspectA::static_memberA(xxx); 
```

QAOP will make all these things simpler and intuitive.
