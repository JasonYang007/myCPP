
#ifndef __LAMBDA_H__
#define __LAMBDA_H__

#include "boost_pp.h"
#include "class_traits.h"

//*** Arithmetic expression templates
namespace lambda
{

template<int n, typename F> struct Def_call;

//*************** Operator definition **************
template<typename T, typename E1, typename E2>
T plus(const E1 & op1, const E2 & op2)
{ 
    return op1+op2;
}

template<typename E1, typename E2>
struct ADD
{
	const E1 & op1;
	const E2 & op2;

	ADD(const E1 & _op1, const E2 &_op2): op1(_op1), op2(_op2) {}

    template<typename P1>
    const P1 operator () (const P1 & x1) const { return op1(x1)+op2(x1);}

    template<typename P1, typename P2>
    const P1 operator () (const P1 & x1, const P2 & x2) const { return op1(x1,x2)+op2(x1,x2);}

    template<typename P1, typename P2, typename P3>
    const P1 operator () (const P1 & x1, const P2 & x2, const P3 & x3) const { return op1(x1,x2,x3)+op2(x1,x2,x3);}

};

template<typename E1, typename E2>
ADD<E1,E2> operator + (const E1 & op1, const E2 & op2)
{
    return ADD<E1,E2>(op1, op2);
}

//*************** Lambda expression ****************


template<int n> struct PlaceHolder {};

#define DEC_PLACE_HOLDER(z, n, _) extern PlaceHolder<BOOST_PP_INC(n)>  BOOST_PP_CAT(_,BOOST_PP_INC(n));

BOOST_PP_REPEAT(NPARAMS, DEC_PLACE_HOLDER, _)

template<typename T>
struct Scalar
{
    Scalar(const T _val): val(_val) {}
    const T & operator() (const T & ) const {return val;}
    template<typename S> const T & operator() (const T & , const S & ) const { return val;}
    template<typename S, typename U> const T & operator() (const T &, const S &, const U &) const { return val;}
    const T val;
};

#define POS_EVAL0(Z,N,_) \
    template<MYPP_ENUM0(N,TAG_class_Tn,_)> \
    static type eval(T & obj, MYPP_ENUM0(N,TAG_Tn_ref_An,_)) {return obj;}

template<typename T>
struct PositionEval 
{ 
    typedef T & type; 
    
    BOOST_PP_REPEAT(NPARAMS, POS_EVAL0, _)
};


#define POS_EVAL(Z,N,M) \
        template<MYPP_ENUM0(BOOST_PP_ADD(N,M), TAG_class_Tn, _)>\
        static T ## M & eval(PlaceHolder<BOOST_PP_INC(M)> & obj, MYPP_ENUM0(BOOST_PP_ADD(N,M),TAG_Tn_ref_An,_)) \
        {return a ## M;}

#define TMPL_POS_EVAL(Z,M,_)\
    template<>\
    struct PositionEval<PlaceHolder<BOOST_PP_INC(M)> >\
    {\
        BOOST_PP_REPEAT(BOOST_PP_SUB(NPARAMS,M), POS_EVAL, M)\
    };


BOOST_PP_REPEAT(NPARAMS, TMPL_POS_EVAL, _)


//**** functor type: wraps a function or functor into a functor
template<typename F, bool br> struct _functor_type;

template<typename F> struct _functor_type<F,false>  // F is a function
{ 
    enum { arity = func_traits<F>::arity };
    typedef Def_call<func_traits<F>::arity,F> type; 
    typedef typename func_traits<F>::result_type result_type;
}; 

template<typename F> struct _functor_type<F,true> // F is a class(functor)
{ 
    typedef F type;
#ifdef _MSC_VER
private:
    typedef typename operator_traits<F>::type _T;
public:
    typedef typename func_traits<_T>::result_type result_type;
#else
    typedef typename F::result_type result_type;
#endif
public:
}; 

#define DEF_GET_TYPE(TYPE)\
    namespace type_detail{\
        template<typename T>\
        struct has_##TYPE\
        {\
            struct yes { char c;};\
            struct no { char c[2]; };\
            \
            template<typename S>\
            static yes test(S *, typename S::TYPE * ptr=NULL);\
            \
            static no test(...);\
            \
            enum { value=sizeof(test(static_cast<T *>(NULL)))==sizeof(yes) };\
        };\
        \
        template<typename T, bool br>\
        struct __##TYPE { typedef void type; enum{value=false};};\
        \
        template<typename T>\
        struct __##TYPE<T,true> { typedef typename T::TYPE type; enum{value=true};};\
    } /* end of namespace */ \
    \
    template<typename T>\
    struct get_##TYPE: type_detail::__##TYPE<T, type_detail::has_##TYPE<T>::value> {};

DEF_GET_TYPE(result_type);

template<typename F> struct functor_type: _functor_type<F, type_detail::has_result_type<F>::value > {};

template<typename F>
typename functor_type<F>::type make_functor(F f)
{
    return typename functor_type<F>::type(f);      
}

//**** BINDn definition
#define BIN_POS_EVAL(z,N,M)\
    BOOST_PP_COMMA_IF(N) PositionEval<BOOST_PP_CAT(A,N)>::eval(BOOST_PP_CAT(a,N), MYPP_ENUM0(BOOST_PP_DEC(M),TAG_Xn,_))

#define BIND_OP(z,N,M)\
    template<MYPP_ENUM0(N,TAG_class_Tn,_)>\
    R operator()(MYPP_ENUM0(N,TAG_Tn_ref_Xn,_))\
    { return f( BOOST_PP_REPEAT(BOOST_PP_INC(M), BIN_POS_EVAL, BOOST_PP_INC(N) ) ); }

#define BIND_OP_void(z,N,M)\
    template<MYPP_ENUM0(N,TAG_class_Tn,_)>\
    void operator()(MYPP_ENUM0(N,TAG_Tn_ref_Xn,_))\
    { f( BOOST_PP_REPEAT(BOOST_PP_INC(M), BIN_POS_EVAL, BOOST_PP_INC(N) ) ); }

#define TMPL_BIND(z,N,_)\
    template<typename F,typename R, MYPP_ENUM0(N, TAG_class_An,_)>\
    struct BOOST_PP_CAT(BIND, BOOST_PP_INC(N))\
    {\
	    typedef typename functor_type<F>::type Ftor;\
	    BOOST_PP_CAT(BIND, BOOST_PP_INC(N))(F _f, MYPP_ENUM0(N, TAG_An_uAn,_))\
            :f(Ftor(_f)), MYPP_ENUM0(N, TAG_An_uAn_init, _){}\
	    R operator()(void) { return f( MYPP_ENUM0(N, TAG_an,_) );}\
        BOOST_PP_REPEAT(BOOST_PP_INC(N), BIND_OP, N)\
    private:\
	    Ftor f;\
	    BOOST_PP_REPEAT(BOOST_PP_INC(N), TAG_An_an, _)\
    };\
    template<typename F, MYPP_ENUM0(N, TAG_class_An,_)>\
    struct BOOST_PP_CAT(BIND, BOOST_PP_INC(N))<F,void,MYPP_ENUM0(N,TAG_An,_)>\
    {\
	    typedef typename functor_type<F>::type Ftor;\
	    BOOST_PP_CAT(BIND, BOOST_PP_INC(N))(F _f, MYPP_ENUM0(N, TAG_An_uAn,_))\
            :f(Ftor(_f)), MYPP_ENUM0(N, TAG_An_uAn_init, _){}\
	    void operator()(void) { f( MYPP_ENUM0(N, TAG_an,_) );}\
        BOOST_PP_REPEAT(BOOST_PP_INC(N), BIND_OP_void, N)\
    private:\
	    Ftor f;\
	    BOOST_PP_REPEAT(BOOST_PP_INC(N), TAG_An_an, _)\
    };

TMPL_BIND(z,0,_)
TMPL_BIND(z,1,_)
TMPL_BIND(z,2,_)
TMPL_BIND(z,3,_)
TMPL_BIND(z,4,_)
TMPL_BIND(z,5,_)
TMPL_BIND(z,6,_)
TMPL_BIND(z,7,_)
TMPL_BIND(z,8,_)

//BOOST_PP_REPEAT_2ND(NPARAMS, TMPL_BIND, _)

//template<typename F,typename R,typename A1,typename A2>
//struct BIND2
//{
//	typedef typename functor_type<F>::type Ftor;
//
//	BIND2(F _f,A1 _a1,A2 _a2):f(Ftor(_f)),a1(_a1),a2(_a2){}
//
//	R operator()(void) { return f(a1,a2);}
//
//	template<typename T1>
//	R operator()(T1& x1) { return f(
//		PositionEval<A1>::eval(a1,x1),
//		PositionEval<A2>::eval(a2,x1));}
//
//	template<typename T1,typename T2>
//	R operator()(T1& x1,T2& x2) { return f(
//		PositionEval<A1>::eval(a1,x1,x2),
//		PositionEval<A2>::eval(a2,x1,x2));}
//private:
//	Ftor f;
//	A1 a1;
//	A2 a2;
//};
//
//template<typename F,typename A1,typename A2>
//struct BIND2<F,void,A1,A2>
//{
//	typedef typename functor_type<F>::type Ftor;
//
//	BIND2(F _f,A1 _a1,A2 _a2):f(Ftor(_f)),a1(_a1),a2(_a2){}
//
//	void operator()(void) { f(a1,a2);}
//
//	template<typename T1>
//	void operator()(T1& x1) { f(
//		PositionEval<A1>::eval(a1,x1),
//		PositionEval<A2>::eval(a2,x1));}
//
//	template<typename T1,typename T2>
//	void operator()(T1& x1,T2& x2) { f(
//		PositionEval<A1>::eval(a1,x1,x2),
//		PositionEval<A2>::eval(a2,x1,x2));}
//private:
//	Ftor f;
//	A1 a1;
//	A2 a2;
//};


//**** end of BINDn definition

//********** Functor
template<int n, typename F, typename C> struct _Def_call;

template<int n, typename F> struct Def_call
    : _Def_call<n,F, typename func_traits<F>::class_type> 
{
    typedef _Def_call<n,F, typename func_traits<F>::class_type> TBase;
    Def_call(F f): TBase(f) {}
};



#define DEF_CALL(z,N,_) \
    template<typename F>\
    struct _Def_call<N,F,void>\
    {\
	    typedef typename func_traits<F>::result_type result_type;\
        BOOST_PP_REPEAT(N,FUNC_TRAITS_ARG,_)\
        \
	    _Def_call(F f): func(f) {}\
        \
	    result_type operator() (MYPP_ENUM(N,TAG_ARGTYPE_Xn,_)) {return func(MYPP_ENUM(N, TAG_Xn,_));}\
	    F func;\
    };\
    \
    template<typename F,typename C>\
    struct _Def_call<N,F,C>\
    {\
	    typedef typename func_traits<F>::result_type result_type;\
	    BOOST_PP_REPEAT(N,FUNC_TRAITS_ARG,_)\
        \
	    _Def_call(F f): func(f) {}\
        \
	    result_type operator() (C &c BOOST_PP_COMMA_IF(N) MYPP_ENUM(N,TAG_ARGTYPE_Xn,_)) \
            {return (c.*func)(MYPP_ENUM(N, TAG_Xn,_));}\
	    F func;\
    };


BOOST_PP_REPEAT(9, DEF_CALL, _)

//template<typename F>
//struct _Def_call<3,F,void>
//{
//	typedef typename func_traits<F>::result_type result_type;
//	typedef typename func_traits<F>::arg1_type arg1_type;
//	typedef typename func_traits<F>::arg2_type arg2_type;
//	typedef typename func_traits<F>::arg3_type arg3_type;
//
//	_Def_call(F f): func(f) {}
//
//	result_type operator() (arg1_type x1,arg2_type x2,arg3_type x3) {return func(x1,x2,x3);}
//	F func;
//};
//
//template<typename F,typename C>
//struct _Def_call<3,F,C>
//{
//	typedef typename func_traits<F>::result_type result_type;
//	typedef typename func_traits<F>::arg1_type arg1_type;
//	typedef typename func_traits<F>::arg2_type arg2_type;
//	typedef typename func_traits<F>::arg3_type arg3_type;
//
//	_Def_call(F f): func(f) {}
//
//	result_type operator() (C &c,arg1_type x1,arg2_type x2,arg3_type x3) {return (c.*func)(x1,x2,x3);}
//	F func;
//};


//*** mem_fn

template<typename F>
_Def_call<func_traits<F>::arity, F, typename func_traits<F>::class_type> mem_fn(F f)
{
    return _Def_call<func_traits<F>::arity, F, typename func_traits<F>::class_type>(f);
}

//*** reference wrapper
template<typename T>
class reference_wrapper
{
public:
    typedef T type;
    typedef typename remove_reference<T>::type & TRef;
    
    explicit reference_wrapper(TRef obj): r(obj) {}

    operator TRef () const {return r;}
    TRef get() const { return r;}
    T*  get_pointer() const {return &r;}
private:    
    TRef r;
};

template<typename T>
reference_wrapper<T> ref(T& obj) { return reference_wrapper<T>(obj);}

template<typename T>
reference_wrapper<const T> cref(const T& obj) { return reference_wrapper<const T>(obj);}

//**** bind functions

#define DEF_bind(z,N,_)\
    template<typename F,MYPP_ENUM0(N, TAG_class_An, _)>\
    BOOST_PP_CAT(BIND,BOOST_PP_INC(N))<F,typename functor_type<F>::result_type,\
	    MYPP_ENUM0(N,TAG_An,_)> bind(F f,MYPP_ENUM0(N,TAG_An_xn,_))\
    {\
        return BOOST_PP_CAT(BIND,BOOST_PP_INC(N))<F,typename functor_type<F>::result_type,\
	        MYPP_ENUM0(N,TAG_An,_)>(f, MYPP_ENUM0(N,TAG_Xn,_));\
    }\
    template<typename R, typename F,MYPP_ENUM0(N, TAG_class_An, _)>\
    BOOST_PP_CAT(BIND,BOOST_PP_INC(N))<F,R,\
	    MYPP_ENUM0(N,TAG_An,_)> bind(F f,MYPP_ENUM0(N,TAG_An_xn,_))\
    {\
        return BOOST_PP_CAT(BIND,BOOST_PP_INC(N))<F,R,\
	        MYPP_ENUM0(N,TAG_An,_)>(f, MYPP_ENUM0(N,TAG_Xn,_));\
    }

BOOST_PP_REPEAT(NPARAMS, DEF_bind, _)

//template<typename F,typename A1,typename A2,typename A3>
//BIND3<F,typename functor_type<F>::result_type,
//	A1,A2,A3> bind(F f,A1 x1,A2 x2,A3 x3)
//{
//return BIND3<F,typename functor_type<F>::result_type,
//	A1,A2,A3>(f, x1, x2, x3);
//}
//
//template<typename R, typename F,typename A1,typename A2,typename A3>
//BIND3<F,R,A1,A2,A3> bind(F f,A1 x1,A2 x2,A3 x3)
//{
//return BIND3<F,R,A1,A2,A3>(f, x1, x2, x3);
//}


//**** apply function: applies a function/functor to a tuple of arguments
template<class F, int arity> struct _apply;

template<class F>
struct _apply<F,0>
{
    _apply(F func) { func(); }
};

template<class F>
struct _apply<F,1>
{   
    template<class T0>
    _apply(F func, tuples::tuple<T0> tpl) { func(tpl.get<0>());}
};

template<class F>
struct _apply<F,2>
{   
    template<class T0,class T1>
    _apply(F func, tuples::tuple<T0,T1> tpl) { func(tpl.get<0>(),tpl.get<1>());}
};

template<class F, class T>
void apply(F func, T tpl)
{
    _apply<F, T::len>(func, tpl);
}


} /* end of namespace */

#endif /* __LAMBDA_H__ */

