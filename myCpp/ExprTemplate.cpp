
#include <iostream>
#include <fstream>
#include <ctime>
#include <complex>
#include <map>
#include <cassert>
#include <string>
#include <vector>
#include <list>

#include "numcpp.h"
#include "MatrixLAPACK.h"
#include "StopWatch.h"
#include "class_traits.h"
#include "lambda.h"
#include "SerDes.h"
#include "Tuples.h"
#include "GenericFunc.h"

#undef max
#undef min
#undef abs

#define Million 1000000

using namespace tuples;

template <typename T>
struct DD
{
    T dd;
    T val;
    typedef T type;

    explicit DD(const T _dd): dd(_dd), val(_dd) {}

};

template <typename T>
struct EE
{
    T dd;

    EE(const T _dd): dd(_dd) {}

    
    friend
    double func(const EE & v1, const EE & v2)
    {
        return (v1.dd+v2.dd);
    }

    template<typename _O>
    friend
    double func(const _O & v1, const EE & v2)
    {
        return (v1.dd+v2.dd);
    }

    //template<typename _O>
    //friend
    //double func(const EE & v2, const _O & v1)
    //{
    //    return (v1.dd+v2.dd);
    //}

};

template <typename T>
struct Is_Array: false_type { enum {value=0, len=0}; };

template <typename T, unsigned n>
struct Is_Array<T[n]>: true_type { enum {value=1, len=n}; };

template <typename T, typename F>
void f(const T & var, const F & flag = false_type());

template <typename T>
void f(const T & var, const false_type &) {}

template <typename T>
void f(const T & var, const true_type &) {}

template<typename T>
void it_print(const T & darr)
{
    for(typename T::const_iterator it=darr.begin(); it != darr.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}


template<typename T>
void * createObj(void) { return new T();}



struct B
{
    enum Enum {A1, B1, C1, D1, E1};

    static Enum eval;
    static int cnt;
    int id;
public:
    B(void): id(++cnt) { std::cout << "B() called(" << id << ")" << std::endl;}
    B(const B & b): id(++cnt) {std::cout << "B(const B &) called(" << id << ")" << std::endl;}
    virtual void DO(void) 
    { 
        std::cout << "B::DO" << std::endl;
    }
    ~B(void) { --cnt; std::cout << "~B() called(" << id << ")" << std::endl;}
};
int B::cnt=0;
REGISTER_ENUM(B, Enum, A1, B1, C1, D1, E1);

struct B2
{
    enum Enum { AA2, BB2, CC2};
    static Enum eval;
};
REGISTER_ENUM(B2, Enum, AA2, BB2, CC2);

struct D: public B
{
public:
    virtual void DO(void) 
    { 
        B::DO(); std::cout << "D::DO" << std::endl;
    }
    virtual void f(void) {}
};

template<typename T, typename R>
void print_ptr_info(const T & lexpr, const R & rexpr)
{
    int * ptr = lexpr.get_sptr().get();
    size_t use_cnt = lexpr.use_count();
    size_t ref_cntl = lexpr.ref_count(ptr);
    size_t ref_cntr = rexpr.ref_count(ptr);
    size_t ext_cnt = use_cnt -(ref_cntl+ref_cntr+1);
    std::cout << "ptr: " << ptr << "\ntotal used: " << use_cnt << 
                 " external: " << ext_cnt <<
                 " left: " << ref_cntl << " right: " << ref_cntr << std::endl;
}

struct Bexpr
{
    Bexpr(const B & _op): op(_op) {}
    B op;
};

Bexpr B_test(const B & b)
{
    std::cout << "B_test called.\nNow exiting... " << std::endl;
    return Bexpr(b);
}


typedef const char * Tcchar;

tuple<Tcchar> taglist(Tcchar p1)
{
    return make_tuple(p1);
}

tuple<Tcchar,Tcchar> taglist(Tcchar p1, Tcchar p2)
{
    return make_tuple(p1,p2);
}

tuple<Tcchar,Tcchar,Tcchar> taglist(Tcchar p1, Tcchar p2,Tcchar p3)
{
    return make_tuple(p1,p2,p3);
}

template<int n, typename S>
void ppp_tst_n(const _pair<const char *,S> & p)
{
    const char * ptr = p.first;
    std::cout << ">>> " << ptr << std::endl;
    ppp_tst_n<n-1>(p.second);    
}


template<typename S>
void ppp_tst(const _pair<const char *,S> & p)
{
    /*const char * ptr = p.first;
    std::cout << ">>> " << ptr << std::endl;
    if(p.size() > 1)  ppp_tst(p.second);*/
    ppp_tst_n<_pair<const char *,S>::len>(p);
}


void ppp_tst(const tNULL & p)
{
}

template<>
void ppp_tst_n<1,tNULL> (const _pair<const char *,tNULL>  & p)
{
    const char * ptr = p.first;
    std::cout << ">>> " << ptr << std::endl;
}

template<typename S, const char *>
_pair<const char *,S> & getitem(const _pair<const char *,S> & p, int k, const char * ptr)
{
    if(k==0) { ptr = p.get<0>(); return p;}
    if(k >= p.size()) { return p; }
    return getitem(p.second, k, ptr);
}

void nop(void) {}

int arr[][2][3]=
    {{{1,2,3,},
      {4,5,6,}},
     {{9,8,7},
      {6,5,4}}};


template<typename T>
void ff(const T & obj, const true_type &, char term_char)
{
    static char open_delim='{';
    static char close_delim='}';
    static char separator=',';

    int dim = array_traits<T>::dim;
    int total = array_traits<T>::total;
    typename array_traits<T>::TShape ss = array_traits<T>::shape();

    int len = array_traits<T>::len;
    std::cout << open_delim ;
    for(int ii=0; ii < len; ++ii)
    {
        char t_char = (ii < len-1 ? separator: close_delim);

        ff(obj[ii], t_char);
    }
    std::cout << term_char;

}

template<typename T>
void ff(const T & obj, const false_type &, char term_char)
{
    std::cout << obj << term_char;
}


template<typename T>
void ff(const T & obj, char term_char=' ')
{
    ff(obj, Is_Array<T>(), term_char);
}



template<typename R, typename T1>
R Func(T1) { return R(); }

template<typename T>
T add(T x, T y) { return x+y; }

struct DDClass;

struct baseClass
{ 
    virtual DDClass *getDDptr(void) { return NULL;}
};
struct DDClass: baseClass 
{ 
    DDClass * getDDptr(void) { return this; }
};
struct DD2Class: baseClass {};

bool debug = false;

template<typename T>
struct WrapObj
{
    typedef T Ptype;

    WrapObj(typename remove_reference<Ptype>::type & obj): ref(obj) {}

    typename remove_reference<T>::type & ref;
};

template<typename T,typename A>
struct WrapObj<std::vector<T,A> & >
{   
    typedef std::list<T,A> & Ptype;

    WrapObj(const Ptype obj) 
    {
        for(typename std::list<T,A>::const_iterator it = obj.begin(); it != obj.end(); ++it)
            ref.push_back(*it);
    }
    std::vector<T,A> ref;
};

template<typename T,typename A>
struct WrapObj<const std::vector<T,A> & >
{   
    typedef const std::list<T,A> & Ptype;

    WrapObj(const Ptype obj) 
    {
        for(typename std::list<T,A>::const_iterator it = obj.begin(); it != obj.end(); ++it)
            ref.push_back(*it);
    }
    std::vector<T,A> ref;
};

template<typename T, T> struct Ftor;

template<typename R, typename T1, typename T2, typename T3, R (*func)(T1, T2, T3) >
struct Ftor<R (*)(T1,T2,T3), func>
{   
    typedef typename WrapObj<T1>::Ptype P1;
    typedef typename WrapObj<T2>::Ptype P2;
    typedef typename WrapObj<T3>::Ptype P3;

    static R call(P1 p1, P2 p2, P3 p3)
    {
        WrapObj<T1> x1(p1);
        WrapObj<T2> x2(p2);
        WrapObj<T3> x3(p3);
        R rcode;

        rcode = func(x1.ref, x2.ref, x3.ref);

        return rcode;
    }
};

typedef int (*TFunc)(const std::vector<double>&, int &, std::vector<double>&);
int cppfunc(const std::vector<double> &, int &, std::vector<double>&)
{
    return 0;
}

struct BDeal;
struct DDeal1;
struct DDeal2;
struct DDeal3;
struct Simul;

//***** Visitor template
// B: immediate base class, D: current derived class
// B0: ultimate base class at the top of hierarchy
// R: reference type to be visited
template<typename B, typename D, typename B0, typename S> struct Visitor;

//*** Specialization for ultimate base classs
template<>
struct Visitor<void, BDeal, BDeal, Simul>
{
    virtual void Exec(int x, int y, Simul *ptr)=0;
};

template<typename B, typename D>
struct Visitor<B, D, BDeal, Simul>: B
{
    virtual void Exec(int x, int y, Simul *ptr);
};

//****** Actual class definitions

// Original declaration header:
// struct BDeal
struct BDeal: Visitor<void, BDeal, BDeal, Simul>
{
};

// Original declaration header:
// struct DDeal1: BDeal
struct DDeal1: Visitor<BDeal, DDeal1, BDeal, Simul>
{
    void run_deal1(int x)
    {
        std::cout << "DDeal1: " << x << std::endl;
    }
};

// Original declaration header:
// struct DDeal2: BDeal
struct DDeal2: Visitor<BDeal, DDeal2, BDeal, Simul>
{
    void run_deal2(int x, int y)
    {
        std::cout << "DDeal2: " << x << ", " << y << std::endl;
    }
};

// Original declaration header:
// struct DDeal3: DDeal2
struct DDeal3: Visitor<DDeal2, DDeal3, BDeal, Simul>
{
    void run_deal3(int x, int y)
    {
        std::cout << "DDeal3: " << x << ", " << y << std::endl;
    }
};

struct Simul
{
    //************** Old style
    virtual void Exec_old(int x, int y, BDeal * ptr)
    {
        if(DDeal1 * dptr = dynamic_cast<DDeal1 *>(ptr))
        {
            dptr->run_deal1(x);   
        }
        else if(DDeal2 * dptr = dynamic_cast<DDeal2 *>(ptr))
        {
            dptr->run_deal2(x,y);
        }
        else if(DDeal3 * dptr = dynamic_cast<DDeal3 *>(ptr))
        {
            dptr->run_deal3(x,y);
        }
    }

    //************* New style
    //*      Separate out Deal-specific algorithms into independent functions.
    //*      They will be called back after deal pointer resolution
    virtual void Exec(int x, int y, BDeal * ptr)
    {
        ptr->Exec(x,y,this);
    }
    void Exec(int x, int y, DDeal1 * ptr)
    {
        ptr->run_deal1(x);   
    }
    void Exec(int x, int y, DDeal2 * ptr)
    {
        ptr->run_deal2(x,y);
    }
    void Exec(int x, int y, DDeal3 * ptr)
    {
        ptr->run_deal3(x,y);
    }
};

template<typename B, typename D>
void Visitor<B, D, BDeal, Simul>::Exec(int x, int y, Simul *ptr) { ptr->Exec(x,y,static_cast<D *>(this)); }

namespace GCD_Helper
{
    template<long x, long y, bool order> struct GCD { enum { value = GCD<y%x,x,true>::value }; };
    template<long x, long y> struct GCD<x,y,false> { enum { value = GCD<x%y,y,true>::value }; };

    template<long x, bool order> 
    struct GCD<0,x,order> { enum {value=x}; };

    template<long x, bool order> 
    struct GCD<x,0,order> { enum {value=x}; };
}

template<long x, long y>
struct GCD { enum {value = GCD_Helper::GCD<x,y,x<=y>::value}; };


template<typename T>
struct SIN
{
    T operator() (const T & x) { return std::sin(x);}
};

using numcpp::ENUM_PWFUNC;
using numcpp::ENUM_PWPOLICY;
using numcpp::default_policy;

template<ENUM_PWFUNC PWFunc, ENUM_PWPOLICY Policy = default_policy<PWFunc>::POLICY> struct XX {};

void benchmark_array();

void benchmark_matrix();

void benchmark(void)
{
//    benchmark_array();
    benchmark_matrix();
}

int _add(double x, double y)
{
    return (int)(x+y);
}

void vadd(double & x, double y)
{
    x += y;
}

struct TEST
{
    typedef double result_type;

    result_type operator()(double) { return result_type(); }
};

struct TEST2
{
};

template<typename T> struct Ftraits { typedef void result_type;};

template<typename R, typename T1, typename T2>
struct Ftraits<R (*)(T1,T2)> { typedef R result_type; };

template<typename T, typename C, typename T1, typename T2>
struct Ftraits<T (C::*)(T1,T2) const> { typedef T result_type; };

void lambda_test();

namespace IMP1
{
    void print() { std::cout << "IMPL: 1" << std::endl;}
}

namespace IMP2
{
    void print() { std::cout << "IMPL: 2" << std::endl;}

}

namespace GGG_1
{
    namespace IMP=IMP1;

    struct hello
    {
        void operator() () {IMP::print();}
    };
    void func() {IMP::print();}

}

namespace GGG_2
{
    namespace IMP=IMP2;

    struct hello
    {
        void operator() () {IMP::print();}
    };
    void func() {IMP::print();}

}

void exprTmpl_test()
{
//    {
//        using namespace GGG_1;
//        hello HH;
//        HH();
//        func();
//    }
//    {
//        using namespace GGG_2;
//        hello HH;
//        HH();
//        func();
//    }


    std::cout << "lambda_test..." << std::endl;
    lambda_test();
    std::cout << "Done!" << std::endl;

    {
        #ifdef _MSC_VER
        {
            bool br;
            
            br = operator_traits<std::plus<double> >::value;
            typedef operator_traits<std::plus<double> >::type plusF;
            // plusF Fxx;
            //func_traits<operator_traits<std::plus<double> >::type>::result_type d1;

            br = operator_traits<TEST>::value;
            typedef operator_traits<TEST>::type F;
            func_traits<F>::result_type vvv=0;

            br = operator_traits<TEST2>::value;
            typedef operator_traits<TEST2>::type II;

            //func_traits<typename has_operator<TEST2>::func_type>::result_type vv=0;
            //__has_operator<typename has_operator<TEST2>::func_type>::result_type vv=0;

            nop();
        }
        #endif
        
        using namespace lambda;

        double x1=3.0, x2=2.5, x3=4.0;
        double dd = PositionEval<PlaceHolder<1> >::eval(_1, x1,x2);

        dd = lambda::bind<double>(std::plus<double>(), ref(x1), x2)();

//        int ii = bind(_add, 1.0, _1)(x2);
//        bind(vadd, ref(x1),_1)(x2);
        //func_traits<int (&)(double, double)>::result_type ii;

        nop();

        //double zz;
        //zz=(_1 + Scalar<double>(1.0))(x1); //,x2);
        //zz=(_1+_2)(x1,x2);
        //zz=(_1 + _2 + _3 + 1.0)(x1,x2,x3);
        //std::cout << "zz=" << zz << std::endl;
    }

//    exit(0);

    using namespace numcpp;

    std::cout << "\nExpression template testing started...\n" << std::endl;
       {
           std::cout << "LU Decompotition ... "; 
           if(debug) std::cout << std::endl;
           #define _DIM 3
           Matrix<double> mm1(_DIM,_DIM,randN);
           Matrix<double> l(_DIM,_DIM);
           Matrix<double> u(_DIM,_DIM);
           Array<int> p(_DIM);

           if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
           lu(mm1, l, u, p);
           if(debug) std::cout << "l:\n" << l << std::endl;
           if(debug) std::cout << "u:\n" << u << std::endl;
           if(debug) std::cout << "p:\n" << p << std::endl;
           
           Array<bool> mask(3);
           mask[0]=true; mask[2]=true;

           if(debug) std::cout << "l*u:\n" << dot(l,u)(pivot2index(p),mask2index(mask)) << std::endl;
           if(debug) std::cout << "l*u:\n" << dot(l,u) << std::endl;

           std::cout << "Done!"<<std::endl;
           std::cout << "slogdet(mat): " << slogdet(mm1) << std::endl;
           std::cout << "det(mat): " << det(mm1) << std::endl;
       }

    { // stack test
        Matrix<double> m1(2,2, randN);
        Matrix<double> m2(2,2, randN);
        Array<double> arr(2, randN);

        if(debug) std::cout << "m1:\n" << m1 << std::endl;
        if(debug) std::cout << "m2:\n" << m2 << std::endl;
        if(debug) std::cout << "arr:\n" << arr << std::endl;
        if(debug) std::cout << "vstack(m1,arr):\n" << vstack(m1,arr) << std::endl;
        if(debug) std::cout << "vstack(arr,m1):\n" << vstack(arr,m1) << std::endl;
        if(debug) std::cout << "vstack(m1,m2):\n" << vstack(m1,m2) << std::endl;
        if(debug) std::cout << "hstack(m1,arr):\n" << hstack(m1,arr.asColMat()) << std::endl;
        if(debug) std::cout << "hstack(arr,m1):\n" << hstack(arr.asColMat(),m1) << std::endl;
        if(debug) std::cout << "hstack(m1,m2):\n" << hstack(m1,m2) << std::endl;

    }
    {
        std::ofstream ostr("PWFunc.out");

        std::ofstream & os = ostr;
//        std::ostream & os = std::cout;

        double xarr[]={0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5};

        const size_t len = sizeof(xarr)/sizeof(xarr[0]);
        const size_t len2 = 101;

        Array<double> xi(xarr);
        Array<double> yi(len);
        std::transform(xi.begin(), xi.end(), yi.begin(), SIN<double>());
        
        if(debug) os << "xi: \n" << xi << std::endl;
        if(debug) os << "yi: \n" << yi << std::endl;

        std::vector<double> Xi(xi.begin(), xi.end());
        std::vector<double> Yi(yi.begin(), yi.end());

        Array<double> xii(linspace<double>(-1, 4, len2));
        Array<double> yii(xii.size());

        if(debug) os << "xii: \n" << xii << std::endl;

        {
            PWFunc<CONST> func(.5);

            std::transform(xii.begin(), xii.end(), yii.begin(), func);
            if(debug) os << "PWFunc<CONST>: \n" << yii << std::endl;
        }

        {
            PWFunc<PWCONST> func(Xi, Yi, 0.1);

            std::transform(xii.begin(), xii.end(), yii.begin(), func);

            if(debug) os << "PWFunc<PWCONST>: \n" << yii << std::endl;
        }

        {
            PWFunc<PWCONST,CONT_RIGHT> func(Xi, Yi, 0.1);

            std::transform(xii.begin(), xii.end(), yii.begin(), func);

            if(debug) os << "PWFunc<PWCONST,CONT_RIGHT>: \n" << yii << std::endl;
        }
       
        {
            PWFunc<PWLINEAR,TERM_CONST,TERM_LINEAR> func(Xi, Yi);

            Array<double> yii(xii.size());
            std::transform(xii.begin(), xii.end(), yii.begin(), func);

            if(debug) os << "xii: \n" << xii << std::endl;
            if(debug) os << "PWFunc<PWLINEAR,TERM_CONST,TERM_LINEAR>: \n" << yii << std::endl;
        }

        {
            PWFunc<PWLINEAR,TERM_LINEAR,TERM_CONST> func(Xi, Yi);

            Array<double> yii(xii.size());
            std::transform(xii.begin(), xii.end(), yii.begin(), func);

            if(debug) os << "xii: \n" << xii << std::endl;
            if(debug) os << "PWFunc<PWLINEAR,TERM_LINEAR,TERM_CONST>: \n" << yii << std::endl;
        }

        // os.close();
    }
    {
        double a[]={1., 2., 3.};
        Array<double> vec(a);

        if(debug) std::cout << "vec: \n" << vec << std::endl;

        double d[][3]={ {1., 2., 3.}, {4.,5.,6.}};
        Matrix<double> mat(d);

        if(debug) std::cout << "mat: \n" << mat << std::endl;
    }
    {
        Matrix<int> mat=range(9).reshape(3,3).Tr();
        Array<int> vv(5, 1);

        if(debug) std::cout << "mat: \n" << mat << std::endl;
        if(debug) std::cout << "vv: \n" << vv << std::endl;
        if(debug) std::cout << "convolve<FULL>(mat,vv): \n" << convolve<FULL>(mat,vv) << std::endl;
        if(debug) std::cout << "convolve<FULL>(mat.row(0),vv): \n" << convolve<FULL>(mat.row(0),vv) << std::endl;
        if(debug) std::cout << "convolve<SAME>(mat,vv): \n" << convolve<SAME>(mat,vv) << std::endl;
        if(debug) std::cout << "mat.row(0): \n" << mat.row(0) << std::endl;
        if(debug) std::cout << "convolve<SAME>(mat.row(0),vv): \n" << convolve<SAME>(mat.row(0),vv) << std::endl;
        if(debug) std::cout << "convolve<VALID>(mat,vv): \n" << convolve<VALID>(mat,vv) << std::endl;
        if(debug) std::cout << "mat.row(0): \n" << mat.row(0) << std::endl;
        if(debug) std::cout << "convolve<VALID>(mat.row(0),vv): \n" << convolve<VALID>(mat.row(0),vv) << std::endl;
    }

    {
       long g = GCD<151230,102131232>::value;
       long gg = g;

       Array<int> vec=range(3);
       Array<int> vv=ones<int>(3);

       if(debug) std::cout << "vec: " << vec << std::endl;
       if(debug) std::cout << "vv: " << vv<< std::endl;
       int res = dot(vec,vv);
       if(debug) std::cout << "dot(mat,vv): " << res << std::endl;
    }

    {
        // Visitor test
        Simul ss;
        BDeal * ptr;
        DDeal1 d1;
        DDeal2 d2;
        DDeal3 d3;

        ptr = &d1;
        ss.Exec(1,2, ptr);

        ptr = &d2;
        ss.Exec(1,2, ptr);

        ptr = &d3;
        ss.Exec(1,2, ptr);

    }

    {
        TFunc func = &cppfunc;
        std::list<double> dummy;

        WrapObj<std::vector<double>& >::Ptype p1(dummy);
        WrapObj<int>::Ptype p2=0;

        WrapObj<std::vector<double>& > x1(p1);
        WrapObj<int> x2(p2);

        std::cout << "x1.ref: " << typeid(x1.ref).name() << std::endl;
        std::cout << "x2.ref: " << typeid(x2.ref).name() << std::endl;

        Ftor< TFunc, &cppfunc >::call(p1,p2,p1); 
        p2=0;
    }

    if(true)
    {
        int nrows=3;
        int ncols=3;

        Array<double> brr(10, numcpp::random<NORMAL>());
        Array<double> arr(brr);

        brr.item(0) = 10;

        // arr=brr;

        Matrix<double> mm(nrows, ncols, numcpp::random<NORMAL>());

        if(debug) std::cout << std::endl;
        if(debug) std::cout << "mm: \n" << mm << std::endl;

        Matrix<double> minv(inv(mm));

        if(debug) std::cout << "inv(mm): \n" << minv << std::endl;
        if(debug) std::cout << "dot(mm,inv(mm)):\n" << dot(mm , minv) << std::endl;
        if(debug) std::cout << "mm*inv(mm):\n" << mm*minv << std::endl;

        {
            Array<double> arr(ncols, randN);
            if(debug) std::cout << "Broadcasting test:\n" << arr << std::endl;
            if(debug) std::cout << "arr:\n" << arr << std::endl;
            if(debug) std::cout << "mm*arr:\n" << mm*arr << std::endl;
            if(debug) std::cout << "arr*mm:\n" << arr*mm << std::endl;
        }

        Matrix<double> mm2;
        Array<Integer> idx;
        idx=range(nrows*ncols);
        
        mm2 = minv;
        if(debug) std::cout << "mm2:\n" << mm2 << std::endl;

        mm2.item(0,0) = mm2.item(1,1);

    }
    {
        std::vector<int> vec;
        numcpp::Iterator<int, std::vector<int> > it(vec,0);
        numcpp::Iterator<int, std::vector<int> > it2(vec,10);
        ++it;
        it+2;
        Integer offset = (Integer)(it2-it);
    }
    {
        baseClass * bptr;
        DDClass * dptr;


        bool br = is_base_of<baseClass,baseClass>::value;
        br = is_same<baseClass, baseClass>::value;
        br = is_proper_base_of<baseClass, DDClass>::value;

        bptr = new DDClass;
        dptr = bptr->getDDptr();
    
        bptr = new DD2Class;
        dptr = bptr->getDDptr(); 

        //func_traits<decltype(Func<int,double>(double()))>::type iii;
        //func_traits<decltype(Func<int>(double())) >::type iii;
        int iii = Func<int>(double());
        //decltype(Func<int>(double())) jjj=iii;
    
        //typedef Func<int,double> TFunc;

        //func_traits<Func<int,double> >::return_type kkk=jjj;
        functor<double (*)(int), Func<double,int> > ftor;
        double dval = ftor(1);

        functor<double (*)(double,double), add<double> > ftor2;
        dval = ftor2(1.1, 2.2);
    
        //ff(arr);
    }

    if(false){
        using namespace tuples;

        ppp_tst(taglist("hello", "cc", "123"));
    
        tuple<int, double, int, bool> tpl; 
        tpl << 123 << 3.14 << 987 << true;

        std::cout << set_open('[') << set_close(']') << set_delimiter(',') << std::boolalpha << tpl << std::endl;
        std::cout << tpl << std::endl;

        std::cout << "Enter tuple in the form [int,double,int,bool]: " << std::endl;
        std::cin >> set_open('[') >> set_close(']') >> set_delimiter(',') >> tpl;
        std::cout << "Result: " << tpl << std::endl;

        typedef void (&TT1)(const tNULL (&)[1]);
        typedef void (&TT2)(const tNULL (&)[2]);

        double dval = tpl.get<-2>();
        

        TT1  f1 = fskip<1>; // >> dval2;
        TT2  f2 = fskip<2>;

        // tpl >> (TT1)fskip<1> >> dval2;

        tuple<int,double> tpl2;
        tpl2=tpl;

        bool br = (tpl==tpl2);
        br = (tpl <= tpl2);
        br = (tpl < tpl2);
        br = (tpl >= tpl2);
        br = (tpl > tpl2);
        br = (tpl != tpl2);

        std::cout << std::endl;

        nop();
    }

    std::cout << ">>> cnt= " << B_test(B()).op.cnt << std::endl;;

    B::Enum eval = B::D1;
    B2::Enum eval2 = B2::CC2;

    std::string str;    serialize_enum(eval, str);
    std::string str2;   serialize_enum(eval2, str2);
    
    B::Enum eVal;   deserialize_enum(eVal,  str);
    B2::Enum eVal2; deserialize_enum(eVal2, str2); 

    {
        D * ptr = new D();
        shared_ptr<B> sptr1(ptr);
        sptr1->DO();
        std::cout << "*sptr1.usecount:  " << sptr1.use_count() << std::endl;
        {
            shared_ptr<B> sptr2(sptr1);
            sptr2->DO();
            std::cout << "*sptr2.usecount:  " << sptr2.use_count() << std::endl;
        }
        sptr1->DO();
        std::cout << "*sptr1.usecount:  " << sptr1.use_count() << std::endl;

        typedef void * (*TMemFn)(void);
        TMemFn farr[] = { &createObj<double>, &createObj<int>, &createObj<D>};
        TMemFn dfunc;

        dfunc = farr[0];
        shared_ptr<double> dptr((double *)dfunc());
        std::cout << "double: " << *dptr << std::endl;

        dfunc = farr[1];
        shared_ptr<int> iptr((int *)dfunc());
        std::cout << "int: " << *iptr << std::endl;

        dfunc = farr[2];
        shared_ptr<B> bptr((B *)dfunc());
        bptr->DO();
    }


    {
        Array<double> dd(3, randN);
        double dval=0.123;
        asarray(0.232);

        Array<double> dd2(dd);

        size_t ucnt = dd.use_count();

        Array<double> dd3(copy(dd));

        size_t ucnt1 = dd3.use_count();



        size_t ucnt2 = dd2.use_count();
        
        dd2 = dd3;

        size_t ucnt3 = dd2.use_count();

    }


    DD<double> dd(1.0);
    EE<double> ee(2.0);
    StopWatch swatch;

    double vv = func(ee,2.0); // + func(2.0, dd);

    using namespace numcpp;
    {
        Matrix<double> mm(3,3,randN);

        //decltype(mm*mm) expr(mm*mm);

        Matrix<Integer> iarr1(3);
        Matrix<Integer> iarr2(3);

        //decltype(zip(iarr1,iarr2)) zexpr = zip(iarr1,iarr2);

        if(debug) std::cout << "mm:\n" << mm << std::endl;
        //if(debug) std::cout << "expr(mm*mm):\n" << expr << std::endl;
        //mm=expr;
        //if(debug) std::cout << "mm=expr:\n" << mm << std::endl;
        nop();
    }

    if(true)
    {
       using namespace numcpp;
    
    #define LL 3

       //result_traits<bool,bool>::value_type res;
       //std::cout << "bool" << "+" << "bool" << ": " << typeid(res).name() << std::endl;
       {
           std::cout << "integer array test..." << std::endl;
           Array<int> iarr(LL);
           Array<int> iarr2(LL);
           {
               print_ptr_info(iarr2[iarr], iarr2*iarr2*iarr2 + iarr2*iarr2+sin(iarr2));

               Array<int> tmp(iarr);

               size_t use_count = iarr.use_count();
               int * iptr = iarr.get_sptr().get();
               size_t refCnt = (iarr+iarr*iarr).ref_count(iptr);

           }
           print_ptr_info(iarr, iarr);

           iarr=range(1, LL+1, 1);
           iarr2=2;
           std::cout << "iarr: "  << iarr << std::endl;
           std::cout << "iarr2: " << iarr2 << std::endl;
           std::cout << "iarr ^ iarr2 " << (iarr ^ iarr2) << std::endl;
           std::cout << "Done!" << std::endl;
       }

       {  // nonzero test
           Matrix<double> mm(3,3,randN);

           if(debug) std::cout << " mm:\n" << mm << std::endl;
           if(debug) std::cout << "mm < 0.0:\n" << (mm < 0.0) << std::endl;
           if(debug) std::cout << "(mm < 0.0).nonzero():\n" << (mm < 0.0).nonzero() << std::endl;
           if(debug) std::cout << "nonzero(mm < 0.0):\n" << nonzero(mm < 0.0) << std::endl;
           if(debug) std::cout << "mm[nonzero(mm<0.0)]:\n" << mm[nonzero(mm < 0.0)] << std::endl;
       }
       
       {
           // peak-to-peak test
           Matrix<double> mm(3,3,randN);

           if(debug) std::cout << " mm:\n" << mm << std::endl;
           if(debug) std::cout << " ptp(mm):\n" << ptp(mm) << std::endl;
           if(debug) std::cout << " ptp(mm,0):\n" << ptp(mm,0) << std::endl;
           if(debug) std::cout << " ptp(mm,1):\n" << ptp(mm,1) << std::endl;
       }

       {
           Array<double> arr(3, randN);
           Array<Integer> idx(range(3));
           Array<double> vals(3, randN);

           if(debug) std::cout << " arr:\n" << arr << std::endl;
           if(debug) std::cout << " idx:\n" << idx << std::endl;
           if(debug) std::cout << " vals:\n" << vals << std::endl;
           arr[idx]=vals;
           if(debug) std::cout << " arr[idx]=vals:\n" << arr << std::endl;
       }

       { // reference count test

           Matrix<double> mm(3,3,randN);


           size_t ucnt = mm.use_count();

           std::cout << "mm:\n" << mm << std::endl;

           std::cout << "mm*mm:\n" << mm*mm << std::endl;

           size_t ucnt1 = mm.use_count();

           mm = mm*mm;

           size_t ucnt2 = mm.use_count();

           std::cout << "mm=mm*mm\n" << mm << std::endl;

           size_t ucnt3 = mm.use_count();

           Matrix<double> mm2(mm);

           mm.column(0) = 0.0;

           std::cout << "mm:\n" << mm << std::endl;

           //std::pair<int,int> Idx;
           //Idx.first = 0; Idx.second = 1;
           tuple<int,int> Idx(0,1);
           std::cout << "mm(pair(0,1)):\n" << mm[Idx] << std::endl;

           Array<int> idx1(range(3));
           Array<int> idx2(range(3));

           std::swap(idx1[0], idx1[2]);
           std::cout << "zip(idx1,idx2):\n" << zip(idx1,idx2) << std::endl;
           std::cout << "mm[zip(idx1,idx2)]:\n" << mm[zip(idx1,idx2)] << std::endl;

           nop();
       }

       {
           std::complex<double> zz(0.,0.);
           
           std::cout << "zz: ";
           StreamOut(std::cout, zz);
           std::cout << std::endl;

           Matrix<double> mm(LL*LL,1,randN);
           mm=linspace(0.0, 1.0, LL*LL);
           Matrix<double> mm2(mm.reshape(LL,LL));

           std::cout << "matrix: asarray, reshape, pow(3) ..." << std::endl;
           std::cout << "mm.asarray: \n" << mm.asarray() << std::endl;
           std::cout << "matrix: \n" << mm.reshape(LL,-1) << std::endl;
           
           mm.resize(2,2);
           std::cout << "matrix.resize(2,2): \n" << mm << std::endl;
           std::cout << "copy of matrix: \n" << mm2<< std::endl;

           std::complex<double> z(std::cos(1.234),std::sin(1.234));
           std::cout<< "view<complex<double> >(10.*mm) * z:\n" << 
               ( view<std::complex<double> >(10.*mm) * z ) << std::endl;

           std::cout << "real(z*10.*mm):\n" << real(z*view<std::complex<double> >(10.*mm)) << std::endl;
           std::cout << "imag(z*10.*mm):\n" << imag(z*view<std::complex<double> >(10.*mm)) << std::endl;
           std::cout << "abs(z*10.*mm):\n" << abs(z*view<std::complex<double> >(10.*mm)) << std::endl;
           std::cout << "angle(z*10.*mm):\n" << angle(z*view<std::complex<double> >(10.*mm)) << std::endl;
           std::cout << "log(z*10.*mm):\n" << log(z*view<std::complex<double> >(10.*mm)) << std::endl;
           std::cout << "real(z*10.*mm)+imag(z*10.*mm):\n" << real(z*view<std::complex<double> >(10.*mm))+imag(z*view<std::complex<double> >(10.*mm)) << std::endl;
       }
       {
           Matrix<double> mm(LL,LL, randN);
           std::cout << "mm: \n" << mm << std::endl;
           std::cout << "mm*mm*mm: \n" << (mm*mm*mm) << std::endl;
           std::cout << "Done!" << std::endl;
       }
       
       Array<double> darr(LL, randU);
       Array<double> darr2(LL, randU);

       std::cout << "array: atan2 ..." << std::endl;
       std::cout << "\n" << darr << std::endl;
       std::cout << "OP(atan2)\n";
       std::cout << "\n" << darr2 << std::endl;
       std::cout << "=\n" << atan2(darr, darr2) << std::endl;
       std::cout << "Done!" << std::endl;

       Array<int> idxArr(2);
       Array<int> idxArr2(2);
       idxArr2[0]=1;
       idxArr[0]=0; idxArr[1]=LL-1;

       Array<bool> mask(LL);
       Array<bool> mask2(LL);
       Array<double,_SArray<double> > ss(0.5);

       std::cout << "array: mask, sin ..." << std::endl;
       std::cout << "darr:\n" << darr << std::endl;
       size_t ucnt;
       ucnt = darr.use_count();
       
       mask = (darr < 0.6);
       ucnt = darr.use_count();

       int ii = sum(mask);

       ucnt = darr.use_count();
       darr[mask]=0.0;
       std::cout << "mask:\n" << mask << std::endl;
       std::cout << "After darr[mask]=0.0:\n" << darr << std::endl;
       std::cout << "sin(pi*darr):\n" << sin(3.141592*darr) << std::endl;
       std::cout << "sin(pi*darr[~mask]):\n" << sin(3.141592*darr[~mask]) << std::endl;
       std::cout << "Done!" << std::endl;

       //std::cout << "darr2:\n" << darr2 << std::endl;
       //std::cout << "idxArr:\n" << idxArr << std::endl;

       //it_print(darr[idxArr]);
       //std::cout << "darr:\n" << darr[idxArr] << std::endl;
       //darr[idxArr]=darr2[idxArr];
       //std::cout << "darr:\n" << darr[idxArr] << std::endl;
    }

    //*** Matrix test

#define ROWS 3 // 1100 // 3 //1100 //1400
#define COLS ROWS //1000

    if(true)
    {

       using namespace numcpp;

       //DMatrix<double> mm1(ROWS, randN);
       Matrix<double> mm1(ROWS, ROWS, randN);

       SMatrix<double> mm2(ROWS, randN);
//       Matrix<double> mm2(ROWS, ROWS, randN);

//       Matrix<double> mm3(ROWS, ROWS);
       LTMatrix<double> mm3(ROWS, COLS);
       {
           Matrix<int> mm(range(0, ROWS*ROWS));
           Matrix<int> mm1(mm.reshape(ROWS,ROWS));
           Matrix<int> mm2(ROWS,ROWS);

           std::cout << "mm1:\n" << mm1 << std::endl;
           std::cout << "mm2:\n" << mm1 << std::endl;
           mm2.Tr() = mm1;
           std::cout << "mm2.Tr() = mm1:\n" << mm2 << std::endl;

           std::cout << "mm1:\n" << mm1 << std::endl;

           std::cout << "clip(mm1, 2, 7):\n" << clip(mm1, 2, 7) << std::endl;

           std::cout << "mm1*mm1:\n" << mm1*mm1 << std::endl;
           mm1 *= mm1;
           std::cout << "mm1 *= mm1:\n" << mm1 << std::endl;

           std::cout << "mm1 <<= 1 ... "; swatch.start();
           if(debug) std::cout << std::endl;
           if(debug) std::cout << "mm1:\n" << mm1 << std::endl;

           mm1 <<= 1;

           if(debug) std::cout << std::endl;
           if(debug) std::cout << "result ==> mm1:\n" << mm1 << std::endl;

           std::cout << "mm1 < 100 ... ";
           
           mm2 = mm1 < 100;
           if(debug) std::cout << "result ==> mm1 < 100:\n" << mm2 << std::endl;
           
           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

       }
       {
           Array<double> vec(ROWS, randN);
           Array<double> res(ROWS, randN);
           std::cout << "dot(dot(vec,mm1),mm1) ... "; swatch.start();
           res = dot(dot(vec,mm1),mm1);
           if(debug) std::cout << std::endl;
           if(debug) std::cout << "vec:\n" << vec << std::endl;
           if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
           if(debug) std::cout << "dot(mm1,vec):\n" << dot(mm1,vec) << std::endl;
           if(debug) std::cout << "res=dot(dot(vec,mm1),mm1):\n" << res << std::endl;
           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

       }

       {
           Matrix<double> mm(ROWS+3, COLS, randN);

           if(debug) std::cout << "mm:\n" << mm << std::endl;
           if(debug) std::cout << "diag(mm):\n" << diag(mm) << std::endl;
           if(debug) std::cout << "triu(mm,-1):\n" << triu(mm,-1) << std::endl;
           if(debug) std::cout << "tril(mm,-1):\n" << tril(mm,-1) << std::endl;

           if(debug) std::cout << "flattenC(mm):\n" << mm.flattenC() << std::endl;
           if(debug) std::cout << "flattenF(mm):\n" << mm.flattenF() << std::endl;

           diag(mm) = 0.0;
           if(debug) std::cout << "diag(mm)=0.0:\n" << mm << std::endl;
       }

       {
           DMatrix<double> test(ROWS, COLS, randN), res(ROWS, COLS, randN);

           std::cout << "diag mat ... "; swatch.start();
           if(debug) std::cout << std::endl;

           if(debug) std::cout << "diag mat:\n" << test << std::endl;
           res=test*test*test;
           if(debug) std::cout << "mat*mat*mat:\n" << res << std::endl;

           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;
       }

       {
           UTMatrix<double> test(ROWS, COLS, randN), res(ROWS, COLS, randN);

           std::cout << "UT mat ... "; swatch.start();
           if(debug) std::cout << std::endl;

           if(debug) std::cout << "UT mat:\n" << test << std::endl;
           res=test*test*test;
           if(debug) std::cout << "mat*mat*mat:\n" << res << std::endl;

           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;
       }

       {
           LTMatrix<double> test(ROWS, COLS, randN), res(ROWS, COLS, randN);

           std::cout << "LT mat ... "; swatch.start();
           if(debug) std::cout << std::endl;

           if(debug) std::cout << "LT mat:\n" << test << std::endl;
           if(debug) res=test*test*test;
           if(debug) std::cout << "mat*mat*mat:\n" << res << std::endl;

           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

       }

       {
           DMatrix<double> test1(ROWS, COLS, randN);
           LTMatrix<double> test2(ROWS, COLS, randN);
           Matrix<double> res(ROWS, COLS, randN);

           std::cout << "diag*LT*LT ... "; swatch.start();
           if(debug) std::cout << std::endl;

           if(debug) std::cout << "diag mat:\n" << test1 << std::endl;
           if(debug) std::cout << "LT mat:\n" << test2 << std::endl;
           if(debug) std::cout << "(diag+LT)*LT:\n" << (test1+test2)*test2 << std::endl;
           res = (test1+test2)*test2;
           if(debug) std::cout << "(diag+LT)*LT:\n" << res << std::endl;

           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

           std::cout << "LT*LT ... "; swatch.start();
           if(debug) std::cout << std::endl;

           res = test2 * test2;
           if(debug) std::cout << "mat*mat:\n" << Matrix<double>(res) << std::endl;

           swatch.stop();
           std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

       }

       if(debug){
           int idx, n, d, mm;
           
           n=10; 
           for(int r=0; r < n; ++r){
               for(int c=0; c < n; ++c){
                    d=c-r;
                    mm=3*std::min(r,c);
                    bool br = (abs(d) > 1);
                    //idx = br*(-1) + (d==0)*r + (d==1)*(n+r) + (d==-1)*(2*n-1+c);
                    idx = br*-1+ (!br)*(mm+(3-d)%3); // (d==0)*(mm) + (d==1)*(mm+2) + (d==-1)*(mm+1);
                    std::cout << " ";
                    if(idx >= 0)
                        std::cout << std::setw(2) << idx;
                    else
                        std::cout << "  ";
               }
               std::cout << std::endl;
           }
       }

       std::cout << "MASK TESTS ... "; swatch.start();
       if(debug) std::cout << std::endl;

       if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
       size_t ucnt = mm1.use_count();

       DIAGVIEW<double, Matrix<double> > dmask(mm1);
       ucnt = dmask.use_count();

       Matrix<double, DIAGVIEW<double, Matrix<double> > > matd(dmask);
       ucnt = matd.use_count();
       if(debug) std::cout << "DIAGVIEW(mm1):\n" << matd << std::endl;
       
       UTVIEW<double, Matrix<double> > utmask(mm1);
       ucnt = matd.use_count();

       Matrix<double, UTVIEW<double, Matrix<double> > > matut(utmask);
       if(debug) std::cout << "UTVIEW(mm1):\n" << matut << std::endl;

       if(debug) std::cout << "mm2:\n" << mm2 << std::endl;
       ucnt = matut.use_count();
       
       matut = mm2;
       
       ucnt = matut.use_count();

       if(debug) std::cout << "UTVIEW(mm1)=mm2:\n" << matut << std::endl;
       if(debug) std::cout << "mm1:\n" << mm1 << std::endl;

       LTVIEW<double, Matrix<double> > ltmask(mm1);
       Matrix<double, LTVIEW<double, Matrix<double> > > matlt(ltmask);
       if(debug) std::cout << "LTVIEW(mm1):\n" << matlt << std::endl;
       if(debug) std::cout << "mm2:\n" << mm2 << std::endl;
       matlt = mm2;
       if(debug) std::cout << "LTVIEW(mm1)=mm2:\n" << matlt << std::endl;
       if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
       
       swatch.stop();
       std::cout << "Done ! (" << swatch.str() << ")" << std::endl;

       if(debug) std::cout << "mm1: use_count=" << mm1.use_count() << std::endl;
       mm1 = dot(mm2.Tr(), mm2);
       if(debug) std::cout << "mm1=mm2.Tr*mm2: use_count=" << mm1.use_count() << std::endl;
       std::cout << "Cholesky test ... "; swatch.start();
       if(debug) std::cout << std::endl;
       if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
       cholesky(mm1, mm3);
       if(debug) std::cout << "cholesky:\n" << mm3 << std::endl;
       if(debug) std::cout << "cholesky.T:\n" << mm3.Tr() << std::endl;
       if(debug) std::cout << "cholesky*cholesky.T:\n" << dot(mm3,mm3.Tr()) << std::endl;
       swatch.stop();
       std::cout << "Done! (" << swatch.str() << ")" << std::endl;

       std::cout << "Eigenvalue ... "; swatch.start();
       if(debug) std::cout << std::endl;
       Array<double>  eigvals(ROWS);
       Matrix<double> eigvec(ROWS,ROWS);
       eigh(mm2, eigvals, eigvec);
       if(debug) std::cout << std::endl;
       if(debug) std::cout << "mm2:\n" << mm2 << std::endl;
       if(debug) std::cout << "eigen values:\n" << eigvals << std::endl;
       if(debug) std::cout << "eigen vectors:\n" << eigvec << std::endl;
       if(debug) std::cout << "Q*D*Q.T:\n" << dot(eigvec ,dot(DMatrix<double>(eigvals) , eigvec.Tr())) << std::endl;
       
       if(debug) std::cout << "mm2:\n" << mm2 << std::endl;
       eigvalsh(mm2, eigvals);
       if(debug) std::cout << "eigen values:\n" << eigvals << std::endl;
       
       swatch.stop();
       std::cout << "Done! (" << swatch.str() << ")" << std::endl;

       {
           std::cout << "Lstsq Solution... "; swatch.start();
           if(debug) std::cout << std::endl;

           int nrows=ROWS+5;
           int ncols=ROWS;
           int nrhs=3;
           int K = std::min(nrows,ncols);

           Matrix<double> A(nrows,ncols, randN);
           Matrix<double> x(ncols,nrhs);
           Matrix<double> B(nrows,nrhs, randN);
           Array<double> rss(nrhs);
           lapack::Int rank;
           Array<double> singular(K);

           lstsq(A,x,B,rss,rank,singular);

           if(debug) std::cout << "A:\n" << A << std::endl;
           if(debug) std::cout << "B:\n" << B << std::endl;
           if(debug) std::cout << "x:\n" << x << std::endl;
           if(debug) std::cout << "rss:\n" << rss << std::endl;
           if(debug) std::cout << "\nrank:" << rank << std::endl;
           if(debug) std::cout << "S:\n" << singular << std::endl;

           swatch.stop();
           std::cout << "Done! (" << swatch.str() << ")" << std::endl;
       }


       {
           std::cout << "QR Decompotition ... "; swatch.start();
           if(debug) std::cout << std::endl;

           Matrix<double> q(ROWS,ROWS);
           UTMatrix<double> r(ROWS, COLS);
           if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
           qr(mm1, q, r);

           swatch.stop();
           if(debug) std::cout << "q:\n" << q << std::endl;
           if(debug) std::cout << "r:\n" << r << std::endl;
           std::cout << "Done! (" << swatch.str() << ")" << std::endl;
       }

       {
           std::cout << "LU Decompotition ... "; swatch.start();
           if(debug) std::cout << std::endl;

           Matrix<double> l(ROWS,ROWS);
           Matrix<double> u(ROWS,ROWS);
           Array<int> p(ROWS);
           if(debug) std::cout << "mm3:\n" << mm3 << std::endl;
           if(debug) std::cout << "sum(mm3,0):\n" << sum(mm3,0) << std::endl;
           if(debug) std::cout << "sum(mm33,1):\n" << sum(mm3,1) << std::endl;
           if(debug)  std::cout << "cumsum(mm3,0):\n" << cumsum(mm3,0) << std::endl;
           if(debug)  std::cout << "cumsum(mm3,1):\n" << cumsum(mm3,1) << std::endl;

           if(debug) std::cout << "prod(mm3,0):\n" << prod(mm3,0) << std::endl;
           if(debug) std::cout << "prod(mm33,1):\n" << prod(mm3,1) << std::endl;
           if(debug)  std::cout << "cumprod(mm3,0):\n" << cumprod(mm3,0) << std::endl;
           if(debug)  std::cout << "cumprod(mm3,1):\n" << cumprod(mm3,1) << std::endl;

           if(debug) std::cout << "mm1[mm1 > 0]:\n" << mm1[mm1 > 0.0] << std::endl;
           if(debug) std::cout << "eye(3,2):\n" << eye<double>(3,2) << std::endl;

           lu(mm1, l, u, p);
           
           swatch.stop();
           std::cout << "Done! (" << swatch.str() << ")" << std::endl;

           if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
           if(debug) std::cout << "l:\n" << l << std::endl;
           if(debug) std::cout << "u:\n" << u << std::endl;
           if(debug) std::cout << "p:\n" << p << std::endl;
           Array<bool> mask(ROWS);
           mask[0]=true; mask[2]=true;

           if(debug) std::cout << "l*u:\n" << dot(l,u)(pivot2index(p),mask2index(mask)) << std::endl;
           if(debug) std::cout << "l*u:\n" << dot(l,u) << std::endl;

           std::cout << "det(mat): " << det(mm1) << std::endl;
       }

       {
            if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
            if(debug) std::cout << "mm1(R(-1),R(-1,_N,-1)):\n" << mm1(R(-1),R(-1,_N,-1)) /* R(0,ROWS,2)) */ << std::endl;
            Array<bool> mask(ROWS);
            mask[0]=true; mask[2]=true;
            if(debug) std::cout << "mask:\n" << mask << std::endl;
            if(debug) std::cout << "mm1.column(0)[mask]:\n" << mm1.column(0)[mask] << std::endl;

            if(debug) std::cout << "compress0(mask, mm1):\n" << compress0(mask, mm1) << std::endl;
            if(debug) std::cout << "compress1(mask, mm1):\n" << compress1(mask, mm1) << std::endl;
            if(debug) std::cout << "mm1(mask,mask):\n" << mm1(mask,mask) << std::endl;

            Array<int> idx1(ROWS);
            idx1 = range(ROWS);
            std::swap(idx1[0], idx1[2]);
            Array<int> idx2(ROWS);
            idx2 = range(ROWS);

            if(debug) std::cout << "idx1:\n" << idx1 << std::endl;
            if(debug) std::cout << "idx2:\n" << idx2 << std::endl;
            if(debug) std::cout << "zip(idx1,idx2):\n" << zip(idx1,idx2[R(_N,_N,-1)]) << std::endl;

            if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
            if(debug) std::cout << "repeat(mm1, 2, 1):\n" << repeat(mm1, 2, 1) << std::endl;
            if(debug) std::cout << "Array(mm1):\n" << static_cast<Array<double> >(mm1) << std::endl;
            if(debug) std::cout << "mm1.indexed:\n" << mm1(idx1, idx2) << std::endl;
            if(debug) std::cout << "mm3.indexed:\n" << mm3(idx1, idx2) << std::endl;
       
            mm1=mm3(idx1,idx2);
            if(debug) std::cout << "After assignment mm1=mm3(idx1,idx2):" << std::endl;
            if(debug) std::cout << "mm1:\n" << mm1 << std::endl;
            if(debug) std::cout << "mm3:\n" << mm3 << std::endl;

            if(debug) std::cout << "round(mm3, 2):\n" << round(mm3,2) << std::endl;
            if(debug) std::cout << "trunc(10*mm3):\n" << trunc(10.0*mm3) << std::endl;
            if(debug) std::cout << "ceil(10*mm3):\n" << ceil(10.0*mm3) << std::endl;
            if(debug) std::cout << "floor(10*mm3):\n" << floor(10.0*mm3) << std::endl;
            if(debug) std::cout << "hypot(mm1,mm3):\n" << hypot(mm1,mm3) << std::endl;
            mm1.sort(0);
            if(debug) std::cout << "After mm1.sort(0):\n" << mm1 << std::endl;

            
            if(debug) std::cout << "mean(mm1):\n" << mean(mm1) << std::endl;
            if(debug) std::cout << "mean(mm1,0):\n" << mean(mm1,0) << std::endl;
            if(debug) std::cout << "mean(mm1,1):\n" << mean(mm1,1) << std::endl;

            Array<double> weight(mm1.size(), randN);
            weight *= weight;
            if(debug) std::cout << "weight:\n" << weight << std::endl;
            if(debug) std::cout << "average(mm1, weight):\n" << average(mm1, weight) << std::endl;
            if(debug) std::cout << "average(mm1, 0, weight):\n" << average(mm1, 0, weight[R(0,mm1.nrows)]) << std::endl;
            if(debug) std::cout << "average(mm1, 1, weight):\n" << average(mm1, 1, weight[R(0,mm1.ncols)]) << std::endl;

            if(debug) std::cout << "cov(mm1):\n" << cov(mm1) << std::endl;
            if(debug) std::cout << "corrcoef(mm1):\n" << corrcoef(mm1) << std::endl;

            if(debug) std::cout << "var(mm1):\n" << var(mm1) << std::endl;
            if(debug) std::cout << "var(mm1,0):\n" << var(mm1,0) << std::endl;
            if(debug) std::cout << "var(mm1,1):\n" << var(mm1,1) << std::endl;

            if(debug) std::cout << "std(mm1):\n" << std_(mm1) << std::endl;
            if(debug) std::cout << "std(mm1,0):\n" << std_(mm1,0) << std::endl;
            if(debug) std::cout << "std(mm1,1):\n" << std_(mm1,1) << std::endl;

            Array<double> bins;
            bins = linspace(-2.0, 2.0, 10);
            if(debug) std::cout << "bins:\n" << bins<< std::endl;
            if(debug) std::cout << "mm.asarray():\n" << mm1.asarray() << std::endl;

            if(debug) std::cout << "histogram(mm,bins):\n" << histogram(mm1.asarray(),bins,true) << std::endl;
            if(debug) std::cout << "histogram(mm,5):\n" << histogram(mm1.asarray(),5,true) << std::endl;

            if(debug) std::cout << "diff(mm):\n" << diff(mm1) << std::endl;
            if(debug) std::cout << "diff(mm,0):\n" << diff(mm1,0) << std::endl;
            if(debug) std::cout << "diff(mm,1):\n" << diff(mm1,1) << std::endl;

            if(debug) std::cout << "ratio(mm):\n" << ratio(mm1) << std::endl;
            if(debug) std::cout << "ratio(mm,0):\n" << ratio(mm1,0) << std::endl;
            if(debug) std::cout << "ratio(mm,1):\n" << ratio(mm1,1) << std::endl;

            Matrix<double> rr;
            {
                rr = diff(mm1,0);
                if(debug) std::cout << "rr:\n" << rr << std::endl;
            }

       }
    }
    if(true) 
    {
        using namespace numcpp;

        int nrows=ROWS+3;
        int ncols=ROWS;
        int K = std::min(nrows, ncols);

        Matrix<double> mm1(nrows, ncols, randN);
        Matrix<double> U(nrows, K);
        Array<double>  S(K);
        Matrix<double> Vt(K, ncols);

        std::cout << "SVD ... "; swatch.start();

        long rcode = svd(mm1, U, S, Vt, false);
        if(debug) std::cout << std::endl;
        if(debug) std::cout << "mm1: \n" << mm1 << std::endl;
        if(debug) std::cout << "U: \n" << U << std::endl;
        if(debug) std::cout << "S: \n" << S << std::endl;
        if(debug) std::cout << "Vt: \n" << Vt << std::endl;
        if(debug) std::cout << "U*S*Vt: \n" << dot(dot(U,diag(S)),Vt) << std::endl;

        Matrix<double> mpinv(ncols, nrows);
        rcode = pinv(mm1, mpinv);
        if(debug) std::cout << "pinv(mm1): \n" << mpinv << std::endl;
        if(debug) std::cout << "pinv(mm1)*mm1: \n" << dot(mpinv,mm1) << std::endl;

        swatch.stop();
        std::cout << "Done! (" << swatch.str() << ")" << std::endl;
    }
    
    {
       using namespace numcpp;
       Matrix<double> mm1(ROWS, COLS, randN);
       Matrix<double> mm2(ROWS, COLS, randN);
       SMatrix<double> mm3(ROWS, randN);

       Matrix<double> res1(ROWS, ROWS);
       { // C-style execution
           std::cout << "\nMatrix Mult (C): ";
           swatch.start();

           for(int rr=0; rr < ROWS; ++rr)
           {
               for(int cc=0; cc < ROWS; ++cc)
               {
                   double ss=0.0;
                   for(int k=0; k < COLS; ++k)
                   {
                        ss += mm1(rr,k) * mm2(k,cc);
                   }
                   res1(rr,cc) =ss + mm3(rr,cc);
                   // mm3(rr,cc) = ss  + mm2(rr,cc)*2.0 + mm3(rr,cc);
               }
           }

           swatch.stop();
           std::cout << swatch.str() << std::endl;
           if(debug) std::cout << "result: \n" << res1 << std::endl;
       }

       Matrix<double> res2(ROWS, ROWS);
       {
           // expression template
           std::cout << "\nMatrix Mult (Expr Templ): ";
           swatch.start();

           res2 = dot(mm1,mm2)+ mm3; // + mm2*2.0 + mm3;
           
           swatch.stop();
           std::cout << swatch.str() << std::endl;
           if(debug) std::cout << "mm1: \n" << mm1 << std::endl;
           if(debug) std::cout << "mm3: \n" << mm3 << std::endl;
           if(debug) std::cout << "result: \n" << res2 << std::endl;
           std::cout << "Frob norm of Diff: " << norm<3>(res1-res2) << std::endl;
       }

       Matrix<double> res3(ROWS, ROWS);
       {
           // Naive C++
           std::cout << "\nMatrix Mult (Naive C++): "; std::cout.flush();
           swatch.start();

           Matrix<double> tmp(ROWS, ROWS);
           for(int rr=0; rr < ROWS; ++rr)
           {
               for(int cc=0; cc < ROWS; ++cc)
               {
                   double ss=0.0;
                   for(int k=0; k < COLS; ++k)
                   {
                       ss += mm1(rr,k)*mm2(k,cc);
                   }
                   tmp(rr,cc) = ss;
               }
           }
           Matrix<double> tmp2(ROWS, ROWS);
           for(int rr=0; rr < ROWS; ++rr)
           {
               for(int cc=0; cc < ROWS; ++cc)
               {
                   double ss=0.0;
                    
                   tmp2(rr,cc) = tmp(rr,cc) + mm3(rr,cc);
               }
           }

           for(int rr=0; rr < ROWS; ++rr)
           {
               for(int cc=0; cc < ROWS; ++cc)
               {
                   res3(rr,cc) = tmp2(rr,cc);
               }
           }

           swatch.stop();
           std::cout << swatch.str() << std::endl;
           std::cout << "Frob norm of Diff: " << sumSq(res1-res3) << std::endl;
           if(debug) std::cout << "result: \n" << res3 << std::endl;

           std::cout.flush();
       }

    }
    benchmark();
}


