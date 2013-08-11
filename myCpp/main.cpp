// My CPP Project.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <algorithm>
#include <iterator>
#include <memory>

#include "MyContainer.h"
#include "DesignPattern.h"
#include "Template.h"
#include "Shared_Array.h"
#include "MatrixLAPACK.h"
#include "lambda.h"

using memory::shared_ptr;

#define SIZE_T_MAX      static_cast<size_t>(-1)
#define END_OF(ptr)     ((ptr)+sizeof(ptr)/sizeof(*(ptr)))
#define ITER_RANGE(ptr) (&ptr[0]),END_OF(ptr)
#define RETURNIF(expr)  {int ret=(expr); if(ret != 0) return ret;}

void template_test();
void class_traits_test();
void serialize_test();
void tree_test();
void regex_test();
void exprTmpl_test();
int  thread_test(void);
void test_omp(unsigned N);
void test_numcpp(void);
void lambda_test();

struct TTT
{
    static void sfunc(void) { std::cout << "static func" << std::endl; }
    void func(void) { std::cout << "func" << std::endl; }
};

int test_func()
{
    TTT tobj;

    lambda::mem_fn(&TTT::func)(tobj);
    lambda::mem_fn(&TTT::sfunc)();

    return 1;
}


template <typename T>
#define TContainer std::vector<T>
std::ostream & operator << (std::ostream & ostr, TContainer &Cont)
{
    char delim='[';
    for(TContainer it=Cont.begin(); it != Cont.end(); ++it)
    {
        ostr << delim;
        ostr << *it;
        delim=',';
    }
    delim=']';
    ostr << delim;

    return ostr;
}

template <typename T>
class PTR_PROXY
{
public:
    typedef T * iterator;
    typedef const T * const_iterator;
    PTR_PROXY(T* it1, T* it2, char delim=',', char ldelim=' ', char rdelim=' '):
        _it1(it1), _it2(it2), _delim(delim), _ldelim(ldelim), _rdelim(rdelim) {}

    iterator begin() {return _it1;}
    const_iterator begin() const {return _it1;}
    iterator end() {return _it2;}
    const_iterator end() const {return _it2;}
    const char delim() const {return _delim;}
    const char ldelim() const {return _ldelim;}
    const char rdelim() const {return _rdelim;}
private:
    T * _it1;
    T * _it2;
    char _delim, _ldelim, _rdelim;
};

template <typename TIterator>
class ITER_PROXY
{
public:
    typedef TIterator iterator;
    ITER_PROXY(iterator it1, iterator it2, char delim=',', char ldelim=' ', char rdelim=' '):
        _it1(it1), _it2(it2), _delim(delim), _ldelim(ldelim), _rdelim(rdelim) {}

    iterator  begin() {return _it1;}
    iterator  end() {return _it2;}
    iterator  begin() const {return _it1;}
    iterator  end() const {return _it2;}
    const char delim() const {return _delim;}
    const char ldelim() const {return _ldelim;}
    const char rdelim() const {return _rdelim;}
private:
    iterator _it1;
    iterator _it2;
    char _delim, _ldelim, _rdelim;
};

template <typename TIterator>
std::ostream & operator << (std::ostream & ostr, const ITER_PROXY<TIterator> & proxy)
{
    char _delim=proxy.delim();
    char delim=proxy.ldelim();
    for(TIterator it=proxy.begin(); it != proxy.end(); ++it)
    {
        ostr << delim;
        ostr << *it;
        delim=_delim;
    }
    if(delim != _delim)
    {
        ostr << delim;
    }
    ostr << proxy.rdelim();

    return ostr;
}

void ContainerTest(void)
{   
    MySpace::MyContainer<int> CC;
    MySpace::MyContainer<int>::iterator it=CC.begin();
    std::vector<int> vec;
    int iarr[]={1,2,3};
    bool first=true;

    CC.assign(ITER_RANGE(iarr));
    std::back_insert_iterator<MySpace::MyContainer<int> > back_it(CC);
    copy(ITER_RANGE(iarr), back_it);
    
    std::cout << "The distance is: " << distance(CC.begin(), CC.end()) << std::endl;

    for(MySpace::MyContainer<int>::iterator it=CC.begin(); it != CC.end(); )
    {
        vec.push_back(*it);
        std::cout << *it << std::endl;
        if(first)
        {
            CC.push_back(static_cast<int>(CC.size())+1);
            it=CC.erase(it);
            first=false;
        }
        else
        {
            ++it;
        }
    }

}

struct myFunctor
{   
    myFunctor(int nn): NN(nn) {}
    int NN;
    int operator ()() {return std::rand() % NN;}
};

void AlgorithmTest(void)
{
    int jarr[]={1,2,3,4,5,6,7};
    {
        myFunctor myrand(5);

        std::generate(ITER_RANGE(jarr), myrand);
    }
    std::cout << "Random numbers are: " << ITER_PROXY<const int *>(ITER_RANGE(jarr),',', '(', ')') << std::endl;
    

    std::random_shuffle(ITER_RANGE(jarr));
    std::make_heap(ITER_RANGE(jarr));

    std::sort(jarr, jarr+3); 
    std::sort(jarr+3, END_OF(jarr));

    std::vector<int> v(std::distance(ITER_RANGE(jarr)));
    std::set_union(jarr, jarr+3, jarr+3, END_OF(jarr), v.begin());

    std::cout << "Union is: " << ITER_PROXY<std::vector<int>::iterator>(v.begin(), v.end(),',', '[', ']') << std::endl;

    std::inplace_merge(jarr, jarr+3, END_OF(jarr));

    for( int *itr=jarr, *itr2=END_OF(jarr)-1 ; itr < itr2; ++itr, --itr2)
        std::iter_swap(itr, itr2);

    std::cout << "max: " << *std::max_element(ITER_RANGE(jarr)) << std::endl;
    std::cout << "min: " << *std::min_element(ITER_RANGE(jarr)) << std::endl;


}


void VisitorTest(void)
{
    User    user;
    D1      d1;
    D2      d2;

    user.Attach(&d1);
    user.DoIt();

    user.Attach(&d2);
    user.DoIt();

    user.action();
    user.SetType(ENUM_B);
    user.action();

    //****
    DTYPE dt;
    dt.ii=123;
    user.SetDType(dt);
    BTYPE&  bb=user.GetDType();
    std::cout << "User: " << bb.str() << std::endl;

    user.Combine(&bb);
}

void BridgeTest()
{
    ContextA cA;
    ContextB cB;

    BridgeBase BBase;

    BBase.SetContext(cA);
    BBase.func();

    BBase.SetContext(cB);
    BBase.func();
}

void TemplateTest()
{
    int Fact10=(int)(Fact<10>::value);
    bool is_int=is_integral<int>::value;

    std::cout << "is_integral<int>::value = " << is_int << std::endl;
    std::cout << 10 << "! is " <<  Fact10 << std::endl;
}

#define XML_OUT(os, NAME, DATA) \
    os << "<data name=\"" #NAME "\">\n" \
       << (DATA) \
       << "\n</data>\n"

#define XML_DO(os, NAME, DOIT) \
    { os << "<data name=\"" #NAME "\">\n"; \
      DOIT; \
      os << "\n</data>\n"; \
    }

#define MYPRINTF(FMT, ...) printf("%s[%d] " FMT, __FILE__, __LINE__, __VA_ARGS__)

#define QUOTE_(X)       #X
#define QUOTE_VAL(X)    QUOTE_(X)

template <typename T>
void PRINT(T & Cont)
{
    for(typename T::iterator it=Cont.begin(); it != Cont.end(); ++it)
    {
        std::cout << *it << " ";
    }
    std::copy(Cont.begin(), Cont.end(), std::ostream_iterator<typename T::iterator::value_type>(std::cout));
}

template <typename T>
void IS_Container(T & obj)
{
    typedef typename T::iterator Iter;
    std::cout << "Container type\n";
}

template <>
void IS_Container<int>(int &obj)
{
    std::cout << "int type\n";

}

class S 
{
public:
    template<typename T>
    const S & operator << (void (*)(const T &))
    {
        // do something!
        return *this;
    }
};

template<typename T>
void func(const T &) {}

void dfunc(const double &) {}

template<typename T>
struct has_foobar_type
{
    typedef char yes[1];
    typedef char no[2];

    template<typename C>
    static yes & check(typename C::foobar *);
    
    template<typename>
    static no & check(...);

    static const bool value = (sizeof(check<T>(NULL)) == sizeof(yes));
};

template<typename T, typename Ty>
struct has_member_type
{
    typedef char yes[1];
    typedef char no[2];

    template<typename C>
    static yes & check(Ty C::*);
    
    template<typename>
    static no & check(...);

    static const bool value = (sizeof(check<T>(NULL)) == sizeof(yes));
};

typedef char yes[1];
typedef char no[2];

namespace main_detail
{
    struct B { int elem; typedef int foobar;};
    struct C { double xxx; };
}

int main(int argc, char * argv[])
{
	return 0;

    if(true)
    {
        //lambda_test();
        test_numcpp();
        return 0;
    }
    if(false)
    {
		if(argc < 2)
		{
			std::cerr << "Usage: program num_sample1 ..." << std::endl;
			return -1;
		}
		for(int ii=1; ii < argc; ++ii)
		{
			unsigned ss=atoi(argv[ii]);
			test_omp(ss);
		}
		return 0;
    }
    if(false)
    {
       test_func(); 
       return 0;
    }

    if(false)
    {
       thread_test();
       return 0;
    }

   if(false){
      typedef lapack::Int Int;

       double darr[][2]={{0.99523   , 0.782703  },
                         {-0.267914 , 0.836062  }};
       Int ipiv[2]={0,0};
       Int m=2;
       Int n=2;
       Int lda=2;
       Int rcode;
       double *A=&darr[0][0];
       lapack::dgetrf_(&m, &n, A, &lda, ipiv, &rcode);
       
       std::cout << "ipiv: ";
       for(int ii=0; ii < 2; ++ii)
         std::cout << ipiv[ii] << ", ";

       std::cout << "\nlu: ";
       for(int ii=0; ii < 2; ++ii)
       {
           for(int jj=0; jj < 2; ++jj)
              std::cout << darr[ii][jj] << ",";
           std::cout << std::endl;
        }

   }

    if(false)
    {
        typedef main_detail::B B;
        typedef main_detail::C C;

        double C::* ptr;
        C cc;
        ptr = &C::xxx;
        cc.*ptr = 3.14;


        bool br = has_foobar_type<B>::value;
        br = has_foobar_type<C>::value;

        br = has_member_type<B,int>::value;
        br = has_member_type<C,int>::value;

        int ii=0;
    }

    if(false){ 
        Master master;
        shared_ptr<BaseData> ptr;
        
        ptr.reset(new DerivedData1());
        master.Do(1,3.14,ptr.get());

        ptr.reset(new DerivedData2());
        master.Do(1,3.14,ptr.get());

    }

    S empty;
    typedef void (*Fdouble)(const double &);

    // S empty1 = empty << func<double>; // Compilation error! Cannot use function template as a function argument
    S empty2 = empty.operator<<( func<double> ); // Ok
    S empty3 = empty << (Fdouble)func<double>; // Ok
    S empty4 = empty << dfunc; // Ok

    exprTmpl_test();
    
    return 0;

    regex_test();
    tree_test();
    serialize_test();
    template_test();

    class_traits_test();

    TSHARED_PTR<int> sptr;
    bool br=(sptr==NULL);

    size_t vv=0, ss=SIZE_T_MAX;
    --vv;

    std::vector<int> VEC;
    IS_Container(VEC);
    
    int ii;
    IS_Container(ii);

    MYPRINTF(" Value is %d\n", 9876);
    MYPRINTF(" String is %s\n", QUOTE_VAL(__LINE__));

    XML_OUT(std::cout, MYDATA , (123,343));
    XML_DO(std::cout, YOURDATA,
        for(int i=0; i < 3; ++i)
        {
            XML_OUT(std::cout, iDATA, i);
        }
    );

    TemplateTest();
    BridgeTest();
    VisitorTest();
    ContainerTest();
    AlgorithmTest();
    
    RETURNIF(test_func());

    std::vector<double> vec(10);
    PRINT(vec);

    std::vector<int> ivec(5);
    PRINT(ivec);

    std::vector<double>::iterator it;
    br=(it == vec.end());

    br=(++it == vec.end());

	return 0;
}

