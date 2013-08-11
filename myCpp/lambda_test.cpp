#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

#include "lambda.h"

using namespace lambda;

template<typename T> struct type_conv { typedef void type;};
template<> struct type_conv<int> { typedef int type;};

template<typename F, typename R, typename A1>
struct HELLO1
{
    typedef typename functor_type<F>::type TFtor;

    HELLO1(F _f, A1 _a1): f(TFtor(_f)), a1(_a1) {}

    TFtor f;
    A1 a1;
};

template<typename F, typename R, typename A1, typename A2>
struct HELLO2
{
    typedef typename functor_type<F>::type TFtor;

    HELLO2(F _f, A1 _a1, A2 _a2): f(TFtor(_f)), a1(_a1), a2(_a2) {}

    TFtor f;
    A1 a1;
    A2 a2;
};

template<typename F, typename R, typename A1, typename A2, typename A3>
struct HELLO3
{
    typedef typename functor_type<F>::type TFtor;

    HELLO3(F _f, A1 _a1, A2 _a2, A3 _a3): f(TFtor(_f)), a1(_a1), a2(_a2), a3(_a3) {}

    TFtor f;
    A1 a1;
    A2 a2;
    A3 a3;
};

template<typename R, typename F, typename A1>
HELLO1<F,R,A1> make_hello(F f,A1 a1) { return HELLO1<F,R,A1>(f,a1); }

template<typename F, typename A1>
HELLO1<F,typename type_conv<F>::type,A1> make_hello(F f,A1 a1) 
{ return HELLO1<F,typename type_conv<F>::type,A1>(f,a1); }

template<typename R, typename F, typename A1, typename A2>
HELLO2<F,R,A1,A2> make_hello(F f,A1 a1,A2 a2) { return HELLO2<F,R,A1,A2>(f,a1,a2); }

template<typename F, typename A1, typename A2>
HELLO2<F,typename type_conv<F>::type,A1,A2> make_hello(F f,A1 a1,A2 a2) 
{ return HELLO2<F,typename type_conv<F>::type,A1,A2>(f,a1,a2); }

template<typename R, typename F, typename A1, typename A2, typename A3>
HELLO3<F,R,A1,A2,A3> make_hello(F f,A1 a1,A2 a2,A3 a3) { return HELLO3<F,R,A1,A2,A3>(f,a1,a2,a3); }

template<typename F, typename A1, typename A2, typename A3>
HELLO3<F,typename type_conv<F>::type,A1,A2,A3> make_hello(F f,A1 a1,A2 a2,A3 a3) 
{ return HELLO3<F,typename type_conv<F>::type,A1,A2,A3>(f,a1,a2,a3); }

template<typename T>
struct Unegate
{
    typedef T result_type;

    T operator()(T x) const { return -x;}

    T f(T x) {return -x;}
};

template<class T0, class T1>
void hello(T0 a0, T1 a1)
{
    std::cout << "hello: " << a0 << "," << a1 << std::endl;
}

void lambda_test()
{
    {
        apply(&hello<int,int>, tuples::tuple<int,int>(1,2));
        Unegate<int> uneg;

        int ii = func_traits<int (Unegate<int>::*)(int)>::arity;
        int (Unegate<int>::*func)(int);
        func = &Unegate<int>::f;

        apply( mem_fn(&Unegate<int>::f), tuples::tuple<Unegate<int>&,int>(uneg,1));
        ii=0;
    }
    {
        double dd=1.23;
        int ii=345;
        
        {
            typedef Unegate<double> F;
            typedef functor_type<F>::result_type result_type;

            HELLO1<F,double,double> hh1 = make_hello<double>(F(),dd);
            HELLO1<F,void,double> hh2 = make_hello(F(),dd);

        }
        {
            typedef std::plus<double> F;
            typedef functor_type<F>::result_type result_type;

            HELLO2<F,double,double,double> hh1 = make_hello<double>(F(), dd,dd);
            HELLO2<F,type_conv<F>::type,double,double> hh2 = make_hello(F(),dd,dd);
        }
        
        dd = bind<double>(Unegate<double>(), dd)();
        Unegate<double> UFunc;
        dd = bind(&Unegate<double>::operator(), _1, _2)(UFunc, dd);
        dd = bind(&Unegate<double>::f, _1, _2)(UFunc,dd);
        // Def_call<1, Unegate<double> >::type
        // dd=mem_fn(&Unegate<double>::operator())(UFunc,dd);
        {
            std::vector<std::string> vec;
            vec.push_back("abc");
            vec.push_back("EFGH");
            std::vector<size_t> ll(vec.size());

            std::transform(vec.begin(), vec.end(), ll.begin(), mem_fn(&std::string::length));
            std::for_each(vec.begin(), vec.end(), mem_fn(&std::string::clear));
        }
        ii=0;
    }

}