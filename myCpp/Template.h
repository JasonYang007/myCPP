#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__

#ifdef __GNUC__
    #include <tr1/type_traits>
    using std::tr1::is_base_of;
    using std::tr1::is_same;
    using std::tr1::false_type;
    using std::tr1::true_type;
    using std::tr1::is_integral;
#else
    #include <type_traits>
    using std::is_base_of;
    using std::is_same;
    using std::false_type;
    using std::is_integral;
#endif
#include <iterator>


template<int n>
class Fact
{
public:
    enum {value=n*Fact<n-1>::value};
};

template<>
class Fact<0>
{
public:
    enum {value=1};
};

struct yes_type { static bool value;};
struct no_type  { static bool value;};

bool yes_type::value=true;
bool no_type::value=false;

template<typename T> struct is_bool: public no_type {};
template<> struct is_bool<bool>:public yes_type {};

//template<typename T>
//void print(const T&)
//{
//    std::cout << "Unknown type" << std::endl;
//}

//template<>
void print(const no_type&)
{
    std::cout << "no_type" << std::endl;
}

//template<>
void print(const yes_type&)
{
    std::cout << "yes_type" << std::endl;
}


class B {public: typedef B B_type;};
class D:public B {};
class DD:public D {};
class CC {};

struct B_tag {};
struct CC_tag {};
struct iterator_tag {};
struct integral_tag {};
struct unknown {};
struct no_B_tag {};

template<typename T>
struct type_traits: B_tag {typedef typename T::B_type type;};

template<>
struct type_traits<int>: no_B_tag {};

template<>
struct type_traits<CC>: no_B_tag {};

template<typename T>
struct is_iterable: std::tr1::true_type { typedef typename T::iterator iter;};


void print(const B&)
{
    std::cout << "B" << std::endl;
}

void print(const B_tag &)
{
    std::cout << "B_tag" << std::endl;
}

void print(const no_B_tag &)
{
    std::cout << "no_B_tag" << std::endl;
}

template<bool, typename T, typename F> struct if_;
template<typename T, typename F> struct if_<true, T, F>:T {};
template<typename T, typename F> struct if_<false, T, F>:F {};


template<int> struct _B_based;
template<> struct  _B_based<0>: unknown      {enum { value=0 };};
template<> struct  _B_based<1>: B_tag        {enum { value=1 };};
template<> struct  _B_based<2>: CC_tag       {enum { value=2 };};
template<> struct  _B_based<3>: iterator_tag {enum { value=3 };};
template<> struct  _B_based<4>: integral_tag {enum { value=4 };};

#define IS_BASE_VAL(_B, _T) is_base_of<_B, _T>::value

template<typename T> 
struct B_based: //_B_based<   
                            //1*IS_BASE_VAL(B, T) + 
                            //2*IS_BASE_VAL(CC, T) + 
                            ///*3*IS_BASE_VAL(std::iterator, T) +*/ 
                            //4*std::tr1::is_integral<T>::value +
                            //0
                            if_<IS_BASE_VAL(B,T), B_tag,
                            if_<IS_BASE_VAL(CC,T), CC_tag,
                            if_<std::tr1::is_integral<T>::value, integral_tag, unknown> > >
                        /*>*/ {};

void template_test()
{
    //print(D());

    print(type_traits<B>());
    print(type_traits<D>());
    print(type_traits<CC>());

    bool br = std::tr1::is_base_of<B, D>::value;
    bool br2 = std::tr1::is_base_of<B, CC>::value;


    print(yes_type());
    print(is_bool<int>());
    print(is_bool<bool>());
    print(B());
    print(DD());
}





#endif /* __TEMPLATE_H__ */

