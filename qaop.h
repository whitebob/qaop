#ifndef _QAOP_H_
#define _QAOP_H_

#include <map>
#include <functional>

namespace qaop {

/* Aspect example
template < typename _Base >
struct Aspect1 : public _Base::this_t { //here is the only different part from ordinary 
	typedef typename _Base::fulltype_t  fulltype_t; 
	typedef Aspect1 this_t; 
	Aspect1() {std::cout<<"from aop1    "<<typeid(this).name()<<", fulltype_t is "<<typeid(fulltype_t).name()<<std::endl;}
	fulltype_t somefunc( fulltype_t a, fulltype_t b) {}
	virtual void foo(){std::cout<<"greeting from Aop1::foo"<<std::cout<<std::endl;}
	virtual ~Aspect1() {std::cout<<"from aop1 destructor."<<std::endl;}
};
 *  It is very usefull and important that fulltype_t will keep the same for all apects.
*/ 

/* use it like this
typedef qaop::Decorate<A>::with<Aspect1, Aspect2, Aspect3>::type CombinedAspects;
*/

//forward declarition. aopfy will reconstruct the series of type after iteration, and put it into fulltype_t;
template < typename _AopBase, template <typename> class ... __Aspects >
struct aopfy;

//parameter base wrap.Only if we have a base with the template parameters, we can reconstruct the fulltype.
template < typename _Base, template <typename> class ... __Aspects >
struct aopbase {
	typedef aopfy<aopbase, __Aspects...> reference_t; //will be kept the same during iteration.
	typedef _Base Base_t;
};

//iteration template defination.
template < typename _AopBase, template <typename> class __FirstAspects, template <typename> class ... __RestAspects >
struct aopfy<_AopBase ,__FirstAspects, __RestAspects...> {
	typedef __FirstAspects<typename aopfy<_AopBase, __RestAspects...>::fulltype_t> fulltype_t; 
	typedef typename _AopBase::Base_t this_t;
};

//end of iteration tempalte defination.
template < typename _AopBase >
struct aopfy<_AopBase> {
	typedef typename _AopBase::reference_t fulltype_t; //magic begins from here.
	typedef typename _AopBase::Base_t this_t; //this is why the reconstuction works. becuse aspects only inherate from this_t, not the aopfy<> class. 
};	

//In fact, you can use AOP just by the upper codes
//example: typedef  Aspect1<Aspect2<Aspect3< aopfy<aopbase< A, Aspect1, Aspect2, Aspect3>, Aspect1, Aspect2, Aspect3 > > > >::fulltype_t  Combined;
//however, it seems wired, though it will be more easily to understand how it works.

//Struct Decorat makes the expression clear and easy to use.
template < typename _Base >
struct Decorate {
	//keep the reference during iteration.
	template < template <typename> class ... __Aspects >
	struct remember {
		typedef aopfy<aopbase<_Base, __Aspects...>, __Aspects...> reference_t; 
	};

	template < typename _Remember, template <typename> class ... __Aspects >
	struct with_imp;

	template < template <typename> class ... __Aspects >
	struct with {
		typedef typename with_imp<remember<__Aspects...>, __Aspects...>::combined_t type;
	};
	
	template < typename _Remember, template <typename> class __FirstAspect, template <typename> class ... __RestAspects >
	struct with_imp<_Remember, __FirstAspect, __RestAspects...> {
		typedef  __FirstAspect<typename with_imp<_Remember, __RestAspects...>::combined_t> combined_t;
	};
	
	template < typename _Remember, template <typename> class __LastAspect >
	struct with_imp<_Remember, __LastAspect> {
		typedef __LastAspect<typename _Remember::reference_t> combined_t;
	};	
};
// the following is convenient for combine multiple aspects into one
//
// usage: 
// template <typename _Class>
// using CombinedAspects = qaop::Combine<Aspect1, Aspect2>::combined<_Class>;
//
template < template <typename> class ... __Aspects >
struct Combine;

template < template <typename> class __FirstAspect, template <typename> class ... __RestAspects >
struct Combine<__FirstAspect, __RestAspects...> {
	template <typename _Class>
	using combined = __FirstAspect<typename Combine<__RestAspects... >::template combined<_Class> >;
};

template < template <typename> class __LastAspect >
struct Combine<__LastAspect> {
	template <typename _Class>
	using combined = __LastAspect<_Class>;
};
//In order to support static member, we use a static member sp. it will make the interface simpler.
//example:
//
//template < typename _Base > 
//struct AspectA : public _Base::this_t {
//typedef AspectA this_t;
//...
//
//
//static qaop::static_proxy<this_t> sp.
//
//static get_xxx(){
//	return sp.template proxy<type, qaop::Name("xxx")>();
//}
//
//};
//  
// in the global initialization, you do this just as normal static member initiliazation.
//
// Initialization {
//	proxy<fulltype, type, qaop::Name("xxx")>(initialization parameters);
// }
//
// You do not  need to define the static varible name, just use it using the proxy.
// a little bit complex? It is in fact quite direct. 
//
// because the static varibles should be defined not in each aspect.
// Otherwise, all class inherited/decorated with the aspect[*see note] would have same static varible address.
// That' s not exactly what we want. We want a static member for the fulltype class. and we may expect 
// different fulltype may have different static member values/addresses for their own.
//
// Note *: this is not 100% right, because you have template parameters. we should say with same parameters the aspect would have same address)  
//
// the implemetion is a little complex. because we need the complie time solve of these name.
// thus we use the compile time hash method to change the name into non-type template parameter.
//
// the following hash code is from  KlayGE, I don't find the original source code but from his webpage.
// the GPLv2 Licence/ KPL licences apply to the code part.

// ******Hash code from KlayGE begin ***** //  
constexpr unsigned int _Hash(char const * str, size_t seed)
{
	   return 0 == *str ? seed : _Hash(str + 1, seed ^ (*str + 0x9e3779b9 + (seed << 6) + (seed >> 2)));
}

#define CT_HASH(x) (_Hash(x, 0))
// ******Hash code from KlayG end ***** //  

template < size_t N >
constexpr unsigned int Name(const char (&str)[N])
{
	return _Hash(str, 0); //auto type matching from char(&)[N] to const char * ;
}	

template < typename _Class, typename _Type, unsigned int  N >
_Type & static_member(_Type il = _Type()) {
	static _Type object(il);
	return object;
};

template< typename  _Class >
struct static_proxy {
	template< typename _Type, unsigned int N >
	static _Type & proxy(_Type il = _Type()) {
		return qaop::static_member<typename _Class::fulltype_t, _Type, N>(il);
	}
};

template < typename _Class, typename _Type, unsigned int N >
_Type & proxy(_Type il = _Type()) {
	return qaop::static_member<typename _Class::fulltype_t, _Type, N>(il);
}

// In order to support jointpoints, we need a delegate template function.
// use it like this: 
//
// virtual int method ( type1  param1, type2 param2 ....) {
//	invoke<fulltype_t>(&method, param1, param2); 
// }
//
//
//forward decl
//


template < typename T >
void get_addr(T&& t, T* & aim, std::true_type a) {
		aim = &t;
}	
template < typename T >
void get_addr(T&& t, T* & aim, std::false_type a) {
		*aim = t;
}	
template <typename T >
void get_addr(T&& t, T* & aim) {
	get_addr(std::forward<T&&>(t), aim, std::is_lvalue_reference<T&&>());
}

typedef std::function<int()> func_t; 

template < typename _Fulltype >
struct stub {

typedef std::function<func_t (_Fulltype *, func_t &) > advice_t; 

//stub_r : stub with return value
template < typename _Class, typename _Ret, typename ... _Params >
static std::function<int(_Fulltype *, _Ret *, _Params...)> _r ( _Ret (_Class::* mfp)(_Params...), std::function<int()> f = nullptr ) {
	return [=](_Fulltype *self, _Ret * ret, _Params... args)->int { return (nullptr != f) ? f(): 0; };
}

//stub : stub return void
template < typename _Class, typename _Ret, typename ... _Params >
static std::function<int(_Fulltype *, _Params...)> _ ( _Ret (_Class::* mfp)(_Params...), std::function<int()> f = nullptr ) {
	return [=](_Fulltype *self, _Params... args)->int { return (nullptr != f) ? f(): 0; }; 
}

//wrap_r : introduction for mem_func with return value
template < typename _Class, typename _Ret, typename ... _Params >
static std::function<int(_Fulltype *, _Ret *,  _Params...)> wrap_r ( _Ret (_Class::* mfp)(_Params...)) {
	return [=](_Fulltype *self, _Ret * ret, _Params... args)->int {
		get_addr((self->*mfp)(args...), ret); 
		return 0;
	}; 
}

//wrap : introduction for mem_func return void
template < typename _Class, typename _Ret, typename ... _Params >
static std::function<int(_Fulltype *, _Params...)> wrap ( _Ret (_Class::* mfp)(_Params...)) {
	return [=](_Fulltype *self, _Params... args)->int {
		(self->*mfp)(args...); 
		return 0;
	}; 
}


//advice wrap
template < typename _Class > 
static advice_t wrap ( func_t (_Class::* mfp)(func_t &) ) {
	return [=] (_Fulltype *self, func_t & f)->func_t { return (self->*mfp)(f); };
}	

};//struct stub

template < typename _Fulltype, typename _Callable > 
struct action {

typedef std::function<func_t (_Fulltype *, func_t &) > advice_t; 

static func_t default_advice (_Fulltype * self, func_t & f) {
	return [=](){
		//default advice before start
		//default advice before end
		f();
		//default advice after start
		//default advice after end
		return 0;
	};
}

_Callable _fn;
func_t _closure;
advice_t advice;
action(_Callable fn, advice_t ad = action::default_advice) : _fn(fn), advice(ad) {}
 
template < typename ... _Params >
void execute(_Fulltype * self, _Params ... args) {
	_closure = std::bind(_fn, self, args...);
	if (nullptr != advice)
		(advice(self, _closure))();
        else
		_closure();	
}	

template < typename _Ret, typename ... _Params >
void execute_r(_Fulltype * self, _Ret * ret,  _Params ... args) {
	_closure = std::bind(_fn, self, ret, args...);
	if (nullptr != advice)
		(advice(self, _closure))();
        else
		_closure();	
}	

template < typename _Ret, typename ... _Params >
func_t & bind(_Fulltype * self, _Ret * ret,  _Params ... args) {
	_closure = std::bind(_fn, self, ret, args...);
	return _closure;
}

};//struct action

#define HASHFUNC(x) (_Hash(reinterpret_cast<char*>(reinterpret_cast<void *>(&x)),0))

template < typename _Fulltype, typename _Class, typename _Ret, typename ... _Params >
_Ret & invoke(_Fulltype * self, _Ret (_Class:: *mf)(_Params... ), _Params ... args ){

	typedef qaop::action<_Fulltype, std::function<int(_Fulltype *, _Params...)> > action_t;
	typedef qaop::action<_Fulltype, std::function<int(_Fulltype *, _Ret *,  _Params...)> > action_r_t;

	auto range = proxy<_Fulltype, std::multimap<unsigned int, action_t * >, qaop::Name("::before")>().equal_range(HASHFUNC(mf));
	for (auto it = range.first; it != range.second; it++ ) {
		//invoke ret ::f(args...) execute before
		it->second->template execute(self, args...);
	}

	std::function<int()> f = [](){return 0;};
	_Ret temp;
	_Ret * ret = &temp;

	auto range_i = proxy<_Fulltype, std::multimap<unsigned int, action_r_t *>, qaop::Name("::insitu")>().equal_range(HASHFUNC(mf));
	for (auto it = range_i.first; it != range_i.second; it++) {
		//invoke ret ::f(args...) execute insitu
		if (nullptr == it->second->advice) 
			f = it->second->template bind(self, ret, args...);
		else
			f = (it->second->advice)(self, f);
	}
	f();

	auto range_r = proxy< _Fulltype, std::multimap<unsigned int, action_r_t *>, qaop::Name("::after")>().equal_range(HASHFUNC(mf));
	for (auto it = range_r.first; it != range_r.second; it++) {
		//invoke ret ::f(args...) execute after
		it->second->template execute_r(self, ret, args...);
	}

	return *ret;
}

//void version.
template < typename _Fulltype, typename _Class, typename ... _Params >
void invoke(_Fulltype * self, void (_Class:: *mf)(_Params... ), _Params ... args ){
	typedef qaop::action<_Fulltype, std::function<int(_Fulltype *, _Params...)> > action_t;

	auto range = proxy< _Fulltype, std::multimap<unsigned int, action_t *>, qaop::Name("::before")>().equal_range(HASHFUNC(mf));
	for( auto it = range.first; it != range.second; it++ ) {
		//invoke void ::f(args...) execute before
		it->second->template execute(self, args...);
	}

	std::function<int()> f = [](){return 0;};

	auto range_i = proxy<_Fulltype, std::multimap<unsigned int, action_t *>, qaop::Name("::insitu")>().equal_range(HASHFUNC(mf));
	for (auto it = range_i.first; it != range_i.second; it++) {
		//invoke void ::f(args...) execute insitu
		if (nullptr == it->second->advice)
			f = it->second->template bind(self, args...);
		else
			f = (it->second->advice)(self, f);
	}
	f();

	auto range_r = proxy<_Fulltype, std::multimap<unsigned int, action_t *>, qaop::Name("::after")>().equal_range(HASHFUNC(mf));
	for( auto it = range_r.first; it != range_r.second; it++ ) {
		//invoke void ::f(args...) execute after
		it->second->template execute(self, args...);
	}

}

template <typename _Fulltype>
struct waven {

template<typename _Ret, typename _Class, typename ... _Params> 
static void before(_Ret (_Class::*func)(_Params...), qaop::action<_Fulltype, std::function<int(_Fulltype *, _Params...)> > * action) { 
	typedef qaop::action<_Fulltype, std::function< int(_Fulltype *, _Params...)> > action_t; 
	auto & mp = proxy<_Fulltype, std::multimap<unsigned int, action_t *>, qaop::Name("::before")>(); 
	mp.insert(std::pair<unsigned int, action_t *>(HASHFUNC(func), action)); \
	//add ret ::f(args...) before
}

#define POSITION_R(POSI) \
template<typename _Ret, typename _Class, typename ... _Params> \
static void POSI(_Ret (_Class::*func)(_Params...),  qaop::action<_Fulltype, std::function<int(_Fulltype *, _Ret *,  _Params...)> > * action) { \
	typedef qaop::action<_Fulltype, std::function<int(_Fulltype *, _Ret *,  _Params...)> > action_r_t; \
	auto & mp = proxy<_Fulltype, std::multimap<unsigned int, action_r_t *>, qaop::Name("::"#POSI)>(); \
	mp.insert(std::pair<unsigned int,  action_r_t *>(HASHFUNC(func), action)); \
}

#define POSITION_V(POSI) \
template<typename _Class, typename ... _Params> \
static void POSI(void (_Class::*func)(_Params...),  qaop::action<_Fulltype, std::function<int(_Fulltype *,  _Params...)> > * action) { \
	typedef qaop::action<_Fulltype, std::function <int(_Fulltype *, _Params...)> > action_t; \
	auto & mp = proxy<_Fulltype, std::multimap<unsigned int, action_t *>, qaop::Name("::"#POSI)>(); \
	mp.insert(std::pair<unsigned int,  action_t *>(HASHFUNC(func), action)); \
}

POSITION_R(insitu)
POSITION_V(insitu)

POSITION_R(after)
POSITION_V(after)

#undef POSITION_V
#undef POSITION_R 
};//struct waven

//Pattern 2:  Clone & Take 
//
//Condition: 
//It is following the Protype Design Pattern. Every class has one virtual clone() interface  which returns 
//a new object based on *this 
//
//At the same time, we need use "move" and swap method to swap a rval reference
//guts out. This may reduce time for building new objects. Here the semantic is "take", create with a default parameter and
//swap to rval objects for them to destroy.
//
//the reason we need implement it in virtual methods, instead of 
//new T(std::move(obj));
//because just as the clone cases, it is not always true that T is available,
//sometimes you will only get a ptr or ref to a opaque-type object. 
//one way to get a proper swapable trival object could only be use virtual fuction create_default()
//
//Solution:
//because this point contains the necessay type info, it is easy to implemented the clone mehtod as following: 
//
// virtual base_t * clone() { return self_clone(this); } 
//
// there is no class_name related inside the implemetion, so it can be used in macro, surely.
//
// Need:
// 1. typedef baseclass base_t;
// 2. of course every derived class must have the copy constructor defined.
//
template < typename T >
T * self_clone(T * const self) { return new T(*self); } 
//e.g. virtual base_t * clone() { return self_clone(this); } 

template < typename T >
T * default_create(T * const self) { return new T; } 
//e.g. virtual base_t * create_default() { return default_create(this); } 

//the following clone(T&) and take(T &&) are not suggested to used directly.
//because it is very easy to mix them with the class member func. (not only by 
//us, sometime the compiler may also be made comfused.) 
//on the contrast, clone_by_ptr/take_by_ref will be more easier to be greped.
//
//Remeber each of them return a ptr, this is because they are similar with 
//the "new" allocator. and all of them allocaed mem in fact, and this mem 
//should be released with "delete" on a pointer.
//
template < typename T >
typename std::enable_if<!std::is_constructible<T>::value, typename 
		std::add_pointer<typename std::decay<T>::type>::type>::type 
clone(T & obj) { 
	typedef typename std::decay<T>::type rval_t;
	typedef typename std::add_pointer<rval_t>::type ptr_t;
	return dynamic_cast<ptr_t>(obj.clone()); 
}
template < typename T >
typename std::enable_if<std::is_constructible<T>::value, typename 
		std::add_pointer<typename std::decay<T>::type>::type>::type
clone(T & obj) { 
	typedef typename std::decay<T>::type rval_t;
	return new rval_t(obj); 
}

template < typename T >
typename std::enable_if<!std::is_constructible<T>::value, typename 
		std::add_pointer<typename std::decay<T>::type>::type>::type 
take(T && obj) { 
	typedef typename std::decay<T>::type rval_t;
	typedef typename std::add_pointer<rval_t>::type ptr_t;
	ptr_t temp = dynamic_cast<ptr_t>(obj.create_default());
	swap(*temp, obj);
	return temp;
}
template < typename T >
typename std::enable_if<std::is_constructible<T>::value, typename 
		std::add_pointer<typename std::decay<T>::type>::type>::type
take(T && obj) { 
	typedef typename std::decay<T>::type rval_t;
	typedef typename std::add_pointer<rval_t>::type ptr_t;
	ptr_t temp = new rval_t();
	swap(*temp, obj);
	return temp; 
}

template < typename T >
T * clone_by_ptr(T * const pointer) { return clone(*pointer); }
//e.g. clone_by_ptr([protype passed by pointer]);

template < typename T >
T * clone_by_ref(T & reference) { return clone(reference); }
//e.g. clone_by_ref([protype passed by reference]);

template < typename T >
T * take_by_ptr(T * const pointer) { return take(std::move(*pointer)); }
//e.g. take_by_ptr([rval resource passed by pointer]);

template < typename T >
T * take_by_ref(T & reference) { return take(std::move(reference)); }
//e.g. take_by_ref([rval resourece passed by reference]);
} //end of namespace qaop

#endif //_QAOP_H_ 
