#ifndef _QAOP_H_
#define _QAOP_H_

#include <functional>
#include <map>

namespace qaop {

// TODO: we can use some fancy macros to make the syntax more compact.

// Writing Aspect could be quite easy and intuitive.

/*/// Aspect example:
template <typename _Base>
struct AspectFoo : public _Base::this_t { // here is the only different part
                                          // from ordinary CRTP
    using fulltype_t = typename _Base::fulltype_t;
    using this_t AspectFoo;

    AspectFoo() {
        std::cout << "ctor for aspect   " << typeid(this_t).name()
                << ", fulltype is " << typeid(fulltype_t).name()
                << std::endl;
    }

    // Aspect Member function example
    // Member function which operate on fulltype_t
    virtual fulltype_t somefunc(fulltype_t a) { }

    // Aspect interface
    virtual void funcFoo() {
        std::cout << "greeting from " << typeid(&funcfoo).name()
                << std::endl;
    }

    virtual ~AspectFoo() {
        std::cout << "dtor for aspect   " << typeid(this_t).name()
                << ", fulltype is " << typeid(fulltype_t).name()
                << std::endl;
    }
    // static member support
    static qaop::static_proxy<this_t> sp;
};
//*///

// It is worth noting that fulltype_t will keep the same for all
// apects. It is a MUST-BE because all aspect should work on same
// type, instead of lots of multiple inheritace interface.

// thus member functions should be designed to operate on fulltype_t,
// which will make the design keep clean and neat.

// Another Important thing is, AopEnhancedClass ISA OriginalClass.
// That is, if you OriginalClass as pointer in the framework,
// AopEnhancedClass could just fit well.

// Usage:
// using AopEnhancedClass = qaop::Decorate<OriginalClass>::with<AspectFoo,
// AspectBar, AspectBaz>::type;
// AopEnhancedClass instance;
// instance.funcFoo();
// instance.funcBar();
// instance.funcBaz();

// You can also combine multiple apsects into one, thus reuse them more easily.

// Usage:
// template <typename _Base>
// using AspectFooBar = qaop::Combine<AspectFoo, AspectBar>::combined<_Base>;
// using CombinedAopClass = qaop::Decorate<OriginalClass>::with<AspectFooBar,
// AspectBaz>::type;

// In order to support static member in CombinedAopClass (Not aspect template)
// we use a static member proxy.
// or called sp, which makes the interface simpler.

// template < typename _Base >
// struct AspectFoo: public _Base::this_t {
//     using fulltype_t = _Base::fulltype_t;
//     using this_t = AspectFoo;
//     ...
//     static qaop::static_proxy<this_t> sp;
// };

// Usage:
// using AopClassFooBar = qaop::Decorate<OriginalClass>::with<AspectFoo,
// AspectBar>::type;
// using AopClassFooBaz = qaop::Decorate<OriginalClass>::with<AspectFoo,
// AspectBaz>::type;

// In global initialization (e.g. in main or global class ctor), do this :
// void Initialization {
//     qaop::static_member<fulltype, type, qaop::Name("xxx")>(initialization
// parameters);
//     _OR_ fulltype::sp.template proxy<type,
//     qaop::Name("xxx")>(initialization
// parameters);

//     qaop::static_member<AopClassFooBar, double,
// qaop::Name("myDoubleMember")>(0.5467f);
//     AopClassFooBaz::sp.template proxy<double,
// qaop::Name("myDoubleMember")>(0.5201314f);
// }

/*/// now you can use them just as like class static member
    auto& staticMemberFooBar =
            AopClassFooBar::sp
                    .template proxy<double, qaop::Name("myDoubleMember")>();
    auto& staticMemberFooBaz =
            AopClassFooBaz::sp
                    .template proxy<double, qaop::Name("myDoubleMember")>();
//*///

// Enhancement original class could be divide into two categories:
// 1. add new member function and new static members for class.
// 2. enhance original member fucntions. 

// the first aim could be easily achieved by previous design. and
// the second aim would need more toys to play with.

// Among the consepts of AOP, jointpoint plays an important role, in C++,
// Jointpoints means virtual member function call. Another key concept in
// AOP is advice, which are things done at jointpoints.

// For each jointpoint, we could insert advice. the advice could be put 

// 1. before jointpoint
// 2. around jointpoint
// 3. after jointpoint

// for before case, advices should behave like stack, last inject, first exec
// for after case, advices should behave like pipe, first inject, first exec
// for around case, advices should be exectuted in a in hieratic callback way

// As for advice, if you have an idea of python Decorator (@Decorator)
// syntax sugar, advice is quite like its counterpart in AOP. 

// By design we may want some advices have same signature with original 
// member function, and we call this kind of advice "action".

// on the other side, we also want some advices are interface-insensitve with
// original member function, but have the abitity to access resources outside

// After that we need to inject the advices into specific jointpoints
// , that is, some virtual member function of original class. 
// we call this proces waven.


// In qaop, one advice is an aop member function, it act as higher level
// fucntor which accept original class member function and return a new 
// wraped function, the later could be invoked by aop-decorated class,
// in a virtual function call way.  

// Here is an simple example of advices
/* 
template < typename _Base >
struct AspectCount : public _Base::this_t {
using this_t = AspectCount;
using base_t = _Base::this_t;
using fulltype_t = _Base::fulltype_t;
using func_t = std::function<int()> ;

AspectCount() : counter(0) { }
func_t adviceIncrease (func_t &f) {
	return [=]() {
		f();
		this->counter++;
		return 0;
	};
}
func_t adviceDecrease (func_t &f) {
	return [=]() {
		f();
		if(counter > 0)
			this->counter--;
		return 0;
	};
}
int count() const {return  counter;}

int counter;
}; // struct AspectCount
*/


/*

*/



// If you have no intrests in how it works, that's all what you need to know.
// If you want to run after the rabbit, come with me ...
//*/

// forward declarition. aopfy will reconstruct the series of type after
// iteration, and put it into fulltype_t;
template <typename _AopBase, template <typename> class... __Aspects>
struct aopfy;

// parameter base wrap.Only if we have a base with the template parameters, we
// can reconstruct the fulltype.
template <typename _Base, template <typename> class... __Aspects>
struct aopbase {
    // reference_t will keep the same during iteration.
    using reference_t = aopfy<aopbase, __Aspects...>;
    using base_t _Base;
};

// iteration template defination.
template <typename _AopBase, template <typename> class __FirstAspects,
          template <typename> class... __RestAspects>
struct aopfy<_AopBase, __FirstAspects, __RestAspects...> {
    using fulltype_t = __FirstAspects<
            typename aopfy<_AopBase, __RestAspects...>::fulltype_t>;
    using this_t = AopBase::base_t;
};

// end of iteration tempalte defination.
template <typename _AopBase> struct aopfy<_AopBase> {
    using fulltype_t = _AopBase::reference_t; // magic begins from here.
    using this_t = _AopBase::base_t; // this is why the reconstuction works.
                                     // Aspects should only inherate from
                                     // this_t, not the aopfy<> class.
};

// In fact, you can use AOP just by the upper codes
// example: using Combined = Aspect1<Aspect2<Aspect3< aopfy<aopbase< A, Aspect1,
// Aspect2, Aspect3>, Aspect1, Aspect2, Aspect3 > > > >::fulltype_t; however, it
// seems weired, though it will be easier to show how it works.

// Struct Decorate makes it easy to use.
template <typename _Base> struct Decorate {
    // keep the reference during iteration.
    template <template <typename> class... __Aspects> struct remember {
        using reference_t = aopfy<aopbase<_Base, __Aspects...>, __Aspects...>;
    };

    // forward declaration;
    template <typename _Remember, template <typename> class... __Aspects>
    struct with_imp;

    template <template <typename> class... __Aspects> struct with {
        using type = with_imp<remember<__Aspects...>, __Aspects...>::combined_t;
    };

    template <typename _Remember, template <typename> class __FirstAspect,
              template <typename> class... __RestAspects>
    struct with_imp<_Remember, __FirstAspect, __RestAspects...> {
        using combined_t = __FirstAspect<
                typename with_imp<_Remember, __RestAspects...>::combined_t>;
    };

    template <typename _Remember, template <typename> class __LastAspect>
    struct with_imp<_Remember, __LastAspect> {
        using combined_t = __LastAspect<typename _Remember::reference_t>;
    };
};

// The following helper is used to combine multiple aspects into one
//
// Usage:
// template <typename _Class>
// using AspectFooBar = qaop::Combine<AspectFoo, AspectBar>::combined<_Class>;
//
template <template <typename> class... __Aspects> struct Combine;

template <template <typename> class __FirstAspect,
          template <typename> class... __RestAspects>
struct Combine<__FirstAspect, __RestAspects...> {
    template <typename _Class>
    using combined = __FirstAspect<
            typename Combine<__RestAspects...>::template combined<_Class>>;
};

template <template <typename> class __LastAspect> struct Combine<__LastAspect> {
    template <typename _Class> using combined = __LastAspect<_Class>;
};

// In order to support static member in Aspects, we use a static member proxy.
// or called sp, which makes the interface simpler.
// Usage:
// template < typename _Base >
// struct AspectA : public _Base::this_t {
//     using this_t = AspectA;
//     ...
//     static qaop::static_proxy<this_t> sp;
//};
//
// in the global initialization, you do this just as normal static member
// initiliazation.
//
// Initialization {
//	    qaop::static_member<fulltype, type, qaop::Name("xxx")>(initialization
// parameters);
// }
//
// auto i = AopEnhancedClass::sp.template proxy<type, qaop::Name("xxx")>()
//
// You do not need to define the static varible name in aspect, just use it
// by using the proxy. a little bit complex? It is in fact quite direct.
//
// Because the static varibles should _NOT_ be defined in each aspect.
// Otherwise, all class inherited/decorated with the aspect[*see note] would
// have same static varible address. That's not exactly what we want. We want
// a static member _ONLY_ for the fulltype class. and we may expect different
// fulltype have different static member values/addresses for their own.
//
// Note *: this is not 100% right, because you have template parameters. we
// should say with same (inner aspect types) the aspect would have same address.
//
// the implemetion is a little complex.
// 1. we use static var in tempalte function to simulate fulltype static member.
// 2. this template function is specialized by <fulltype, type, name_hash>
// 3. static_proxy<this_t> will help solve both fulltype & type.
// 4. the name_hash is a comlile-time constexpr.

// Because we need the complie time solve of these memeber name.
// we must use the compile time hash method to change the name
// into non-type template parameter.
//
// the following hash code is from  KlayGE, I don't find the original source
// code but from his webpage. the GPLv2 Licence/ KPL licences apply to the code
// part.

// ******Hash code from KlayGE begin ***** //
constexpr unsigned int _Hash(char const* str, size_t seed) {
    return 0 == *str ? seed
                     : _Hash(str + 1, seed ^ (*str + 0x9e3779b9 + (seed << 6) +
                                              (seed >> 2)));
}

#define CT_HASH(x) (_Hash(x, 0))
// ******Hash code from KlayG end ***** //

template <size_t N> constexpr unsigned int Name(const char (&str)[N]) {
    return _Hash(str, 0); // auto type matching from char(&)[N] to const char *
                          // ;
}

template <typename _Class, typename _Type, unsigned int N>
_Type& static_member(_Type il = _Type()) {
    static _Type object(il);
    return object;
};

template <typename _Aspect> struct static_proxy {
    template <typename _Type, unsigned int N>
    static _Type& proxy(_Type il = _Type()) {
        return qaop::static_member<typename _Aspect::fulltype_t, _Type, N>(il);
    }
};

template <typename _Class, typename _Type, unsigned int N>
_Type& proxy(_Type il = _Type()) {
    return qaop::static_member<typename _Class::fulltype_t, _Type, N>(il);
}


template <typename T> void get_addr(T&& t, T*& aim, std::true_type a) {
    aim = &t;
}
template <typename T> void get_addr(T&& t, T*& aim, std::false_type a) {
    *aim = t;
}
template <typename T> void get_addr(T&& t, T*& aim) {
    get_addr(std::forward<T&&>(t), aim, std::is_lvalue_reference<T&&>());
}

using func_t = std::function<int()>;

// Remember we have said virtual mem_func is the jointpoint,
// but what we need is a virtual fulltype_t::mem_func as the 
// entry, so we use stub to do the trick;

// stub provide some simple wrap methods for member function:
// stub<fulltype_t>::_r(&base_t::mem_func, wrap_func) will return a wrapped 
// function<int(&fulltype_t, _ret, _params...)> which has the 
// correct signature but execute wrap_func instead of 
// base member func 
 
// stub::_(&base_t::mem_func, wrap_func) do the same, just for 
// functions without ret_type.

// stub::wrap_r will retuen a function which will execute 
// the fulltype_t result and set the executed result back to 
// ref or pointer 

// stub::wrap will do the same, just for no ret case.

// in particular case, wrap on an advice, which will return
// an advice which will append advice on existing advice.

template <typename _Fulltype> struct stub {

    using advice_t = std::function<func_t(_Fulltype*, func_t&)>;

    // stub_r : stub with return value
    template <typename _Class, typename _Ret, typename... _Params>
    static std::function<int(_Fulltype*, _Ret*, _Params...)>
    _r(_Ret (_Class::*mfp)(_Params...), std::function<int()> f = nullptr) {
        return [=](_Fulltype* self, _Ret* ret, _Params... args) -> int {
            return (nullptr != f) ? f() : 0;
        };
    }

    // stub : stub return void
    template <typename _Class, typename _Ret, typename... _Params>
    static std::function<int(_Fulltype*, _Params...)>
    _(_Ret (_Class::*mfp)(_Params...), std::function<int()> f = nullptr) {
        return [=](_Fulltype* self, _Params... args) -> int {
            return (nullptr != f) ? f() : 0;
        };
    }

    // wrap_r : introduction for mem_func with return value
    template <typename _Class, typename _Ret, typename... _Params>
    static std::function<int(_Fulltype*, _Ret*, _Params...)>
    wrap_r(_Ret (_Class::*mfp)(_Params...)) {
        return [=](_Fulltype* self, _Ret* ret, _Params... args) -> int {
            get_addr((self->*mfp)(args...), ret);
            return 0;
        };
    }

    // wrap : introduction for mem_func return void
    template <typename _Class, typename _Ret, typename... _Params>
    static std::function<int(_Fulltype*, _Params...)>
    wrap(_Ret (_Class::*mfp)(_Params...)) {
        return [=](_Fulltype* self, _Params... args) -> int {
            (self->*mfp)(args...);
            return 0;
        };
    }

    // advice wrap
    template <typename _Class>
    static advice_t wrap(func_t (_Class::*mfp)(func_t&)) {
        return [=](_Fulltype* self, func_t& f) -> func_t {
            return (self->*mfp)(f);
        };
    }

}; // struct stub


// action is one of the concrete code executed at each jointpoint.

template <typename _Fulltype, typename _Callable> struct action {

    typedef std::function<func_t(_Fulltype*, func_t&)> advice_t;

    static func_t default_advice(_Fulltype* self, func_t& f) {
        return [=]() { f(); return 0; };
    }

    _Callable _fn;
    func_t _closure;
    advice_t advice;

    action(_Callable fn, advice_t ad = action::default_advice)
        : _fn(fn), advice(ad) {}

    template <typename... _Params>
    void execute(_Fulltype* self, _Params... args) {
        _closure = std::bind(_fn, self, args...);
        if (nullptr != advice)
            (advice(self, _closure))();
        else
            _closure();
    }

    template <typename _Ret, typename... _Params>
    void execute_r(_Fulltype* self, _Ret* ret, _Params... args) {
        _closure = std::bind(_fn, self, ret, args...);
        if (nullptr != advice)
            (advice(self, _closure))();
        else
            _closure();
    }

    template <typename _Ret, typename... _Params>
    func_t& bind(_Fulltype* self, _Ret* ret, _Params... args) {
        _closure = std::bind(_fn, self, ret, args...);
        return _closure;
    }

}; // struct action

// For each joint point, there may be more than one advice, so we use 
// a map to manage them. Specifically, the advice may be put 
// 1. before jointpoint
// 2. around jointpoint
// 3. after jointpoint
// for before case, advices should behave like stack, last inject, first exec
// for after case, advices should behave like pipe, first inject, first exec
// for around case, advice should work in a in inhertive callback way  

// Invoke is a resolver for all hooked advices and execute with 
// real params for certail jointpoint

// 
// In order to support jointpoints, we need a delegate template function.
// for each joint point, we need to define like this:
//
// virtual RetType method (Type1 param1, Type2 param2 ....) {
//	   return invoke<fulltype_t>(&method, param1, param2);
// }
//
//

#define HASHFUNC(x)                                                            \
    (_Hash(reinterpret_cast<char*>(reinterpret_cast<void*>(&x)), 0))

template <typename _Fulltype, typename _Class, typename _Ret,
          typename... _Params>
_Ret& invoke(_Fulltype* self, _Ret (_Class::*mf)(_Params...), _Params... args) {

    using action_t = qaop::action<_Fulltype, std::function<int(_Fulltype*, _Params...)>>;
    using action_r_t = qaop::action<_Fulltype, std::function<int(_Fulltype*, _Ret*, _Params...)>>;

    auto range = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,
                       qaop::Name("::before")>()
                         .equal_range(HASHFUNC(mf));
    for (auto it = range.first; it != range.second; it++) {
        // invoke ret ::f(args...) execute before
        it->second->template execute(self, args...);
    }

    std::function<int()> f = []() { return 0; };
    _Ret temp;
    _Ret* ret = &temp;

    auto range_i = proxy<_Fulltype, std::multimap<unsigned int, action_r_t*>,
                         qaop::Name("::insitu")>()
                           .equal_range(HASHFUNC(mf));
    for (auto it = range_i.first; it != range_i.second; it++) {
        // invoke ret ::f(args...) execute insitu
        if (nullptr == it->second->advice)
            f = it->second->template bind(self, ret, args...);
        else
            f = (it->second->advice)(self, f);
    }
    f();

    auto range_r = proxy<_Fulltype, std::multimap<unsigned int, action_r_t*>,
                         qaop::Name("::after")>()
                           .equal_range(HASHFUNC(mf));
    for (auto it = range_r.first; it != range_r.second; it++) {
        // invoke ret ::f(args...) execute after
        it->second->template execute_r(self, ret, args...);
    }

    return *ret;
}

// void version.
template <typename _Fulltype, typename _Class, typename... _Params>
void invoke(_Fulltype* self, void (_Class::*mf)(_Params...), _Params... args) {
    typedef qaop::action<_Fulltype, std::function<int(_Fulltype*, _Params...)>>
            action_t;

    auto range = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,
                       qaop::Name("::before")>()
                         .equal_range(HASHFUNC(mf));
    for (auto it = range.first; it != range.second; it++) {
        // invoke void ::f(args...) execute before
        it->second->template execute(self, args...);
    }

    std::function<int()> f = []() { return 0; };

    auto range_i = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,
                         qaop::Name("::insitu")>()
                           .equal_range(HASHFUNC(mf));
    for (auto it = range_i.first; it != range_i.second; it++) {
        // invoke void ::f(args...) execute insitu
        if (nullptr == it->second->advice)
            f = it->second->template bind(self, args...);
        else
            f = (it->second->advice)(self, f);
    }
    f();

    auto range_r = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,
                         qaop::Name("::after")>()
                           .equal_range(HASHFUNC(mf));
    for (auto it = range_r.first; it != range_r.second; it++) {
        // invoke void ::f(args...) execute after
        it->second->template execute(self, args...);
    }
}

template <typename _Fulltype> struct waven {

    template <typename _Ret, typename _Class, typename... _Params>
    static void
    before(_Ret (_Class::*func)(_Params...),
           qaop::action<_Fulltype, std::function<int(_Fulltype*, _Params...)>>*
                   action) {
        typedef qaop::action<_Fulltype,
                             std::function<int(_Fulltype*, _Params...)>>
                action_t;
        auto& mp = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,
                         qaop::Name("::before")>();
        mp.insert(std::pair<unsigned int, action_t*>(
                HASHFUNC(func), action)); // add ret ::f(args...) before
    }

#define POSITION_R(POSI)                                                       \
    template <typename _Ret, typename _Class, typename... _Params>             \
    static void POSI(                                                          \
            _Ret (_Class::*func)(_Params...),                                  \
            qaop::action<_Fulltype,                                            \
                         std::function<int(_Fulltype*, _Ret*, _Params...)>>*   \
                    action) {                                                  \
        typedef qaop::action<                                                  \
                _Fulltype, std::function<int(_Fulltype*, _Ret*, _Params...)>>  \
                action_r_t;                                                    \
        auto& mp = proxy<_Fulltype, std::multimap<unsigned int, action_r_t*>,  \
                         qaop::Name("::" #POSI)>();                            \
        mp.insert(                                                             \
                std::pair<unsigned int, action_r_t*>(HASHFUNC(func), action)); \
    }

#define POSITION_V(POSI)                                                       \
    template <typename _Class, typename... _Params>                            \
    static void                                                                \
    POSI(void (_Class::*func)(_Params...),                                     \
         qaop::action<_Fulltype, std::function<int(_Fulltype*, _Params...)>>*  \
                 action) {                                                     \
        typedef qaop::action<_Fulltype,                                        \
                             std::function<int(_Fulltype*, _Params...)>>       \
                action_t;                                                      \
        auto& mp = proxy<_Fulltype, std::multimap<unsigned int, action_t*>,    \
                         qaop::Name("::" #POSI)>();                            \
        mp.insert(std::pair<unsigned int, action_t*>(HASHFUNC(func), action)); \
    }

    POSITION_R(insitu)
    POSITION_V(insitu)

    POSITION_R(after)
    POSITION_V(after)

#undef POSITION_V
#undef POSITION_R
}; // struct waven

// Pattern 2:  Clone & Take
//
// Condition:
// It is following the Protype Design Pattern. Every class has one virtual
// clone() interface  which returns a new object based on *this
//
// At the same time, we need use "move" and swap method to swap a rval
// reference guts out. This may reduce time for building new objects. Here the
// semantic is "take", create with a default parameter and swap to rval objects
// for them to destroy.
//
// the reason we need implement it in virtual methods, instead of
// new T(std::move(obj));
// because just as the clone cases, it is not always true that T is available,
// sometimes you will only get a ptr or ref to a opaque-type object.
// one way to get a proper swapable trival object could only be use virtual
// fuction create_default()
//
// Solution:
// because this point contains the necessay type info, it is easy to
// implemented the clone mehtod as following:
//
// virtual base_t * clone() { return self_clone(this); }
//
// there is no class_name related inside the implemetion, so it can be used in
// macro, surely.
//
// Need:
// 1. typedef baseclass base_t;
// 2. of course every derived class must have the copy constructor defined.
//
template <typename T> T* self_clone(T* const self) { return new T(*self); }
// e.g. virtual base_t * clone() { return self_clone(this); }

template <typename T> T* default_create(T* const self) { return new T; }
// e.g. virtual base_t * create_default() { return default_create(this); }

// the following clone(T&) and take(T &&) are not suggested to used directly.
// because it is very easy to mix them with the class member func. (not only by
// us, sometime the compiler may also be made comfused.)
// on the contrast, clone_by_ptr/take_by_ref will be more easier to be greped.
//
// Remeber each of them return a ptr, this is because they are similar with
// the "new" allocator. and all of them allocaed mem in fact, and this mem
// should be released with "delete" on a pointer.
//
template <typename T>
typename std::enable_if<
        !std::is_constructible<T>::value,
        typename std::add_pointer<typename std::decay<T>::type>::type>::type
clone(T& obj) {
    typedef typename std::decay<T>::type rval_t;
    typedef typename std::add_pointer<rval_t>::type ptr_t;
    return dynamic_cast<ptr_t>(obj.clone());
}
template <typename T>
typename std::enable_if<
        std::is_constructible<T>::value,
        typename std::add_pointer<typename std::decay<T>::type>::type>::type
clone(T& obj) {
    typedef typename std::decay<T>::type rval_t;
    return new rval_t(obj);
}

template <typename T>
typename std::enable_if<
        !std::is_constructible<T>::value,
        typename std::add_pointer<typename std::decay<T>::type>::type>::type
take(T&& obj) {
    typedef typename std::decay<T>::type rval_t;
    typedef typename std::add_pointer<rval_t>::type ptr_t;
    ptr_t temp = dynamic_cast<ptr_t>(obj.create_default());
    swap(*temp, obj);
    return temp;
}
template <typename T>
typename std::enable_if<
        std::is_constructible<T>::value,
        typename std::add_pointer<typename std::decay<T>::type>::type>::type
take(T&& obj) {
    typedef typename std::decay<T>::type rval_t;
    typedef typename std::add_pointer<rval_t>::type ptr_t;
    ptr_t temp = new rval_t();
    swap(*temp, obj);
    return temp;
}

template <typename T> T* clone_by_ptr(T* const pointer) {
    return clone(*pointer);
}
// e.g. clone_by_ptr([protype passed by pointer]);

template <typename T> T* clone_by_ref(T& reference) { return clone(reference); }
// e.g. clone_by_ref([protype passed by reference]);

template <typename T> T* take_by_ptr(T* const pointer) {
    return take(std::move(*pointer));
}
// e.g. take_by_ptr([rval resource passed by pointer]);

template <typename T> T* take_by_ref(T& reference) {
    return take(std::move(reference));
}
// e.g. take_by_ref([rval resourece passed by reference]);
} // end of namespace qaop

#endif //_QAOP_H_
