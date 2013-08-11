#ifndef __TUPLE_H__
#define __TUPLE_H__

#include <cassert>
#include <iostream>
#ifdef __GNUC__
    #include <tr1/type_traits>
    using std::tr1::remove_reference;
#else
    #include <type_traits>
    using std::remove_reference;
#endif

#include "boost_pp.h"

namespace tuples
{
struct tNULL;
extern tNULL ignore;
template<int> struct skip;
template<typename T1, typename T2> struct _pair;

struct tNULL { 
	enum { len = 0 }; 
	template<int n> struct val_type { typedef tNULL type; };
	template<int n> tNULL get(void) { return tNULL();}

	template<typename T>
	tNULL & operator = (const T & x) { return *this;}
};

inline bool operator == (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator == (const tNULL & p1, const T & p2) {return false;}
template<typename T> inline bool operator == (const T & p1, const tNULL & p2) {return false;}

inline bool operator != (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator != (const tNULL & p1, const T & p2) {return true;}
template<typename T> inline bool operator != (const T & p1, const tNULL & p2) {return true;}

inline bool operator < (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator < (const tNULL & p1, const T & p2) {return true;}
template<typename T> inline bool operator < (const T & p1, const tNULL & p2) {return false;}

inline bool operator <= (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator <= (const tNULL & p1, const T & p2) {return true;}
template<typename T> inline bool operator <= (const T & p1, const tNULL & p2) {return false;}

inline bool operator > (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator > (const tNULL & p1, const T & p2) {return false;}
template<typename T> inline bool operator > (const T & p1, const tNULL & p2) {return true;}

inline bool operator >= (const tNULL & p1, const tNULL & p2);
template<typename T> inline bool operator >= (const tNULL & p1, const T & p2) {return false;}
template<typename T> inline bool operator >= (const T & p1, const tNULL & p2) {return true;}


template<int> struct skip {};

template<int n, int len> struct idxConv { enum {idx=(n < 0 ? len+n : n)}; };
template<typename T> struct add_reference { typedef typename remove_reference<T>::type & type; };
template<typename T> struct add_const { typedef const T type; };
template<typename T> struct add_const<const T> { typedef const T type; };

namespace _pair_detail{

    template<typename T1, typename T2, int n, int len> struct val_type { typedef typename T2::template val_type<idxConv<n,len>::idx-1>::type type; };
    template<typename T1, typename T2, int len> struct val_type<T1,T2,0,len> { typedef T1 type; };

    template<typename T1, typename T2, int n, int len> struct tail { typedef typename T2::template tail<idxConv<n,len>::idx-1>::type type; };
    template<typename T1, typename T2, int len> struct tail<T1,T2,0,len> { typedef _pair<T1,T2> type; };

    template<typename T1, typename T2, int n, int len> 
    struct _get 
    {
        typedef typename add_const<T1>::type C1;
        typedef typename add_const<T2>::type C2;
        typedef typename add_const<typename val_type<T1,T2,n,len>::type>::type CR;
        typedef typename val_type<T1,T2,n,len>::type R;

        static R & call(T1 & first, T2 & second) { return second.template get<idxConv<n,len>::idx-1>();}
        static CR  call(C1 & first, C2 & second) { return second.template get<idxConv<n,len>::idx-1>();}
    };
    template<typename T1, typename T2, int len> 
    struct _get<T1,T2,0,len>
    {
        typedef typename add_const<T1>::type C1;
        typedef typename add_const<T2>::type C2;

        static T1 & call(T1 & first, T2 & second) { return first; }
        static C1 & call(C1 & first, C2 & second) { return first; }
    };
}

namespace _pair1_detail
{
	template<typename T1, int> struct val_type { typedef tNULL type; };
	template<typename T1 > struct val_type<T1,0> { typedef T1 type; };

    template<typename T1, int n> struct tail { typedef tNULL type; };
	template<typename T1> struct tail<T1,0> { typedef _pair<T1,tNULL> type; };


	template<typename T1, int n> struct _get
    {
        typedef typename add_const<T1>::type C1;
        typedef tNULL T2;
        typedef const tNULL C2;
        typedef typename add_const<typename val_type<T1,n>::type>::type CR;
        typedef typename val_type<T1,n>::type R;

        static R & call(T1 &first, T2 &second) { return second;}
        static CR  call(C1 &first, C2 &second) { return second;}
    };
	template<typename T1> struct _get<T1,0>
    {
        typedef typename add_const<T1>::type C1;
        typedef tNULL T2;
        typedef const tNULL C2;

        static T1 & call(T1 &first, T2 &second) { return first;}
        static C1 & call(C1 &first, C2 &second) { return first;}
    };
}

template<typename T1, typename T2> struct _pair;

template<typename T1>
struct _pair<T1,tNULL>
{
	static unsigned const len = 1;

	_pair(const T1 & x1, const tNULL & x2=tNULL()): first(const_cast<T1 &>(x1)) {}

	template<int n> struct val_type: _pair1_detail::val_type<T1,n> {};
    template<int n> struct tail: _pair1_detail::tail<T1,n> {};

	template<int n> typename val_type<n>::type & get(void) { return _pair1_detail::template _get<T1,n>::call(first,second);}
	template<int n> typename val_type<n>::type  get(void) const { return _pair1_detail::template _get<T1,n>::call(first,second);}

    std::ostream & out(std::ostream & ostr, const char c = ',') const { return ostr << first; }
    std::istream & in(std::istream & istr, const char delim= ',') { return istr >> first; }

    int size(void) const { return 1;}

	T1 first;
	tNULL second;
};

template<typename T1, typename T2>
struct _pair
{
	static unsigned const len = 1 + T2::len;

	_pair(const T1 & x1, const T2 & x2): first(const_cast<T1 &>(x1)), second(const_cast<T2 &>(x2)) {}

    template<int n> struct val_type { typedef typename _pair_detail::val_type<T1,T2,n,len>::type type; };
    template<int n> struct tail{typedef typename  _pair_detail::tail<T1,T2,n,len>::type type; };

    #define Thelper _pair_detail::template _get<T1,T2,n,len>
    template<int n> typename val_type<n>::type & get(void) { return Thelper::call(first,second);}

    typedef typename add_const<T1>::type C1;
    typedef typename add_const<T2>::type C2;

    #define Thelper2 _pair_detail::template _get<C1,C2,n,len>
    template<int n> typename val_type<n>::type get(void) const { return Thelper::call(first,second);}

    std::ostream & out(std::ostream & ostr, const char delim= ',') const 
    { 
        ostr << first << delim << ' '; return second.out(ostr, delim); 
    }
    
    std::istream & in(std::istream & istr, const char delim= ',') 
    { 
        istr >> first; 
        
        char c; istr >>  c;
        if(c != delim) istr.putback(c);

        return second.in(istr, delim); 
    }
    

    int size(void) const {return len;}
	
	T1 first;
	T2 second;
};


//*** tuple definition

#define DEF_TUPLE_ASSIGN(n, VAR) this->template get<n>() = VAR.template get<n>()

template<typename T1, typename T2=tNULL, typename T3=tNULL, typename T4=tNULL, typename T5=tNULL,
         typename T6=tNULL, typename T7=tNULL, typename T8=tNULL, typename T9=tNULL, typename T10=tNULL> 
struct tuple: _pair<T1, typename tuple<T2,T3,T4,T5,T6,T7,T8,T9,T10,tNULL>::TBase>
{
	typedef _pair<T1, typename tuple<T2,T3,T4,T5,T6,T7,T8,T9,T10,tNULL>::TBase> TBase;

	explicit tuple(
        const T1 &x1=T1(), const T2 & x2=T2(), const T3 & x3=T3(), const T4 & x4=T4(), const T5 & x5=T5(),
        const T6 &x6=T6(), const T7 & x7=T7(), const T8 & x8=T8(), const T9 & x9=T9(), const T10 & x10=T10() )
        : TBase(x1,tuple<T2,T3,T4,T5,T6,T7,T8,T9,T10,tNULL>(x2,x3,x4,x5,x6,x7,x8,x9,x10)) {}

    explicit tuple(const TBase & pair): TBase(pair) {}
	
	template<typename S1, typename S2, typename S3, typename S4, typename S5,
             typename S6, typename S7, typename S8, typename S9, typename S10>
	tuple & operator = (const tuple<S1,S2,S3,S4,S5,S6,S7,S8,S9,S10> & t)
	{
        DEF_TUPLE_ASSIGN(0,t); DEF_TUPLE_ASSIGN(1,t); DEF_TUPLE_ASSIGN(2,t); DEF_TUPLE_ASSIGN(3,t);
		DEF_TUPLE_ASSIGN(4,t); DEF_TUPLE_ASSIGN(5,t); DEF_TUPLE_ASSIGN(6,t); DEF_TUPLE_ASSIGN(7,t);
        DEF_TUPLE_ASSIGN(8,t); DEF_TUPLE_ASSIGN(9,t); DEF_TUPLE_ASSIGN(10,t);
		return *this;
	}

};

template<typename T1>
struct tuple<T1,tNULL,tNULL,tNULL,tNULL,tNULL,tNULL,tNULL,tNULL,tNULL>: _pair<T1,tNULL>
{
	typedef _pair<T1,tNULL> TBase;

	explicit tuple(const T1 & x1=T1(), const tNULL & = tNULL(), const tNULL & = tNULL(), const tNULL & = tNULL(), 
                   const tNULL & = tNULL(), const tNULL & = tNULL(), const tNULL & = tNULL(),
                   const tNULL & = tNULL(), const tNULL & = tNULL(), const tNULL & = tNULL()): TBase(x1) {}
	
};

//*** make_tuple function templates
#define TAG_pair_Tn(z,n,_) _pair<BOOST_PP_CAT(T,n),
#define TAG_ANGLE_BRACKET(z,n,_) > 
#define TAG_x_temp_get(z,n,_) BOOST_PP_COMMA_IF(n) x.template get<n>()
#define PAIR_LIST(n) BOOST_PP_REPEAT(BOOST_PP_INC(n), TAG_pair_Tn, _) tNULL \
                     BOOST_PP_REPEAT(BOOST_PP_INC(n), TAG_ANGLE_BRACKET,_) 

#define DEF_make_tuple(z,n,_)\
    template<MYPP_ENUM0(n,TAG_class_Tn,_)>\
    inline tuple<MYPP_ENUM0(n,TAG_Tn,_)> make_tuple(MYPP_ENUM0(n,TAG_const_Tn_ref_An,_))\
    {\
        return tuple<MYPP_ENUM0(n,TAG_Tn,_)>(MYPP_ENUM0(n,TAG_an,_));\
    }\
    template<MYPP_ENUM0(n,TAG_class_Tn,_)>\
    inline tuple<MYPP_ENUM0(n,TAG_Tn,_)> make_tuple(const PAIR_LIST(n) & x)\
    {\
        return tuple<MYPP_ENUM0(n,TAG_Tn,_)>(MYPP_ENUM0(n,TAG_x_temp_get,_));\
    }\
    template<MYPP_ENUM0(n,TAG_class_Tn,_)>\
    inline tuple<MYPP_ENUM0(n,TAG_Tn_ref,_)> tie(MYPP_ENUM0(n,TAG_Tn_ref_An,_))\
    {\
        return tuple<MYPP_ENUM0(n,TAG_Tn_ref,_)>(MYPP_ENUM0(n,TAG_an,_));\
    }\

BOOST_PP_REPEAT(NPARAMS, DEF_make_tuple, _)

//template<typename T1, typename T2, typename T3>
//inline tuple<T1,T2,T3> make_tuple(const T1& x1, const T2& x2, const T3& x3)
//{
//    return tuple<T1,T2,T3>(x1,x2,x3);
//}
//template<typename T1, typename T2, typename T3>
//inline tuple<T1,T2,T3> make_tuple( const _pair<T1,_pair<T2,_pair<T3,tNULL> > > & x)
//{
//    return tuple<T1,T2,T3>(x.template get<0>(),x.template get<1>(),x.template get<2>());
//}
//
//template<typename T1, typename T2, typename T3>
//inline tuple<T1&,T2&,T3&> tie(T1& x1, T2& x2, T3& x3)
//{
//    return tuple<T1&,T2&,T3&>(x1,x2,x3);
//}

//---- pair to tuple type


template<typename T1>
struct tuple_type { typedef tuple<T1> type;};

#define DEF_tuple_type(z,n,_)\
    template<MYPP_ENUM0(n,TAG_class_Tn,_)>\
    struct tuple_type<PAIR_LIST(n) > { typedef tuple<MYPP_ENUM0(n,TAG_Tn,_) > type; };

BOOST_PP_REPEAT(NPARAMS, DEF_tuple_type, _)

//template<typename T1, typename T2, typename T3>
//struct tuple_type<_pair<T1,_pair<T2,_pair<T3,tNULL> > > > { typedef tuple<T1,T2,T3> type; };


//*** stream-like input/output to/from tuple

template<int n> void fskip(const tNULL (&)[n]) {}

template<typename T, typename S>
inline const S & operator >> (const _pair<T,S> & p, typename remove_reference<T>::type & x)
{
	x = p.first;
	return p.second;
}

template<typename T, typename S>
inline const S & operator >> (const _pair<T,S> & p, tNULL & x)
{
	return p.second;
}

template<typename T>
inline tNULL & operator >> (const tNULL & p, T & x)
{
	return ignore;
}

template<typename T, typename S, int n>
inline const typename _pair<T,S>::template tail<n>::type & operator >> (const _pair<T,S> & p, const skip<n> &)
{
	return p >> skip<n-1>() >> ignore;
}

template<typename T, typename S>
inline const _pair<T,S> & operator >> (const _pair<T,S> & p, const skip<0> & x)
{
	return p;
}

template<typename T, typename S, int n>
inline const typename _pair<T,S>::template tail<n>::type & operator >> (const _pair<T,S> & p, void (*)(const tNULL (&)[n]) )
{
    return p >> skip<n-1>() >> ignore;
}

//-------------------------------------------------

template<typename T, typename S>
inline S & operator << (_pair<T,S> & p, const typename remove_reference<T>::type & x)
{
	p.first = x;
	return p.second;
}

template<typename T, typename S>
inline S & operator << (_pair<T,S> & p, const tNULL & x)
{
	return p.second;
}

template<typename T>
inline tNULL & operator << (const tNULL & p, T & x)
{
	return ignore;
}

template<typename T, typename S, int n>
inline typename _pair<T,S>::template tail<n>::type & operator << (_pair<T,S> & p, const skip<n> &)
{
	return p << skip<n-1>() << ignore;
}

template<typename T, typename S>
inline _pair<T,S> & operator << (_pair<T,S> & p, const skip<0> & x)
{
	return p;
}



//*** tuple comparisons

template<typename T, typename S, typename T1, typename S1>
inline bool operator < (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    if(p1.first == p2.first)
    {
        return p1.second < p2.second;
    }
    else
    {
        return p1.first < p2.first;
    }
}

template<typename T, typename S>
inline bool operator < (const _pair<T,S> & p1, const tNULL & p2) { return false; }

template<typename T, typename S>
inline bool operator < (const tNULL & p1, const _pair<T,S> & p2) { return true; }

template<typename T, typename S, typename T1, typename S1>
inline bool operator <= (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    return (p1 == p2) || (p1 < p2);
}

template<typename T, typename S, typename T1, typename S1>
inline bool operator > (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    return !(p1 <= p2);
}

template<typename T, typename S, typename T1, typename S1>
inline bool operator >= (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    return !(p1 < p2);
}

template<typename T, typename S, typename T1, typename S1>
inline bool operator == (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    bool br = (p1.size()==p2.size() && p1.first == p2.first);
    br = (br && p1.second == p2.second);
    return br;
}

template<typename T, typename S, typename T1, typename S1>
inline bool operator != (const _pair<T,S> & p1, const _pair<T1,S1> & p2)
{
    return !(p1==p2);
}

//******* tuple helper functions
template<typename T, typename S, int n>
typename _pair<T,S>::template val_type<n>::type & get(_pair<T,S> & t) 
{ 
    return t.get<n>();
}

//******* tuple input/output streaming
struct iostream_proxy;
iostream_proxy set_open(char c);
iostream_proxy set_close(char c);
iostream_proxy set_delimiter(char c);
iostream_proxy operator << (std::ostream & ostr, iostream_proxy proxy);
iostream_proxy operator >> (std::istream & istr, iostream_proxy proxy);

struct iostream_proxy
{
    iostream_proxy(std::ios_base *_iosptr=NULL, char _odelim='(', char _cdelim=')', char _delim=',')
            : iosptr(_iosptr), odelim(_odelim), cdelim(_cdelim), delim(_delim) {}

    // output stream

    iostream_proxy & operator << (const iostream_proxy & proxy)
    {
        if(proxy.odelim != '\0') odelim=proxy.odelim;
        if(proxy.cdelim != '\0') cdelim=proxy.cdelim;
        if(proxy.delim  != '\0') delim=proxy.delim;
        return *this;
    }
    template<typename T, typename S>
    iostream_proxy & operator << (const _pair<T,S> & p);

    iostream_proxy & operator << (std::ostream & (*manip)(std::ostream &));

    iostream_proxy & operator << (std::ios_base & (*manip)(std::ios_base &));

    // input stream
    
    iostream_proxy & operator >> (const iostream_proxy & proxy)
    {
        if(proxy.odelim != '\0') odelim=proxy.odelim;
        if(proxy.cdelim != '\0') cdelim=proxy.cdelim;
        if(proxy.delim  != '\0') delim=proxy.delim;
        return *this;
    }
    template<typename T, typename S>
    iostream_proxy & operator >> (_pair<T,S> & p);

    iostream_proxy & operator >> (std::istream & (*manip)(std::istream &));

    iostream_proxy & operator >> (std::ios_base & (*manip)(std::ios_base &));
    
    char odelim;
    char cdelim;
    char delim;
    std::ios_base * iosptr;
};

//*** iostream_proxy: member function definition
template<typename T, typename S>
iostream_proxy & iostream_proxy::operator << (const _pair<T,S> & p)
{
    std::ostream & ostr = *dynamic_cast<std::ostream *>(iosptr);
    std::ios_base::fmtflags flags = ostr.flags();


    ostr << std::boolalpha;
    ostr << this->odelim;
    p.out(ostr, delim);
    ostr << this->cdelim;

    ostr.flags(flags);
    return *this;
}

template<typename T, typename S>
iostream_proxy & iostream_proxy::operator >> (_pair<T,S> & p)
{
    std::istream & istr = *dynamic_cast<std::istream *>(iosptr);
    std::ios_base::fmtflags flags = istr.flags();

    char c;
    istr >> std::boolalpha >> std::skipws >> c; 
    assert(c==this->odelim);
    
    p.in(istr, this->delim);
    
    istr >> c;
    assert(c==this->cdelim);

    istr.flags(flags);
    return *this;
}


template<typename T, typename S>
std::ostream & operator << (std::ostream & ostr, const _pair<T,S> & p)
{
    iostream_proxy proxy(&ostr);
    proxy << p;
    
    return ostr;
}

template<typename T, typename S>
std::istream & operator >> (std::istream & istr, _pair<T,S> & p)
{
    iostream_proxy proxy(&istr);
    proxy >> p;
    
    return istr;
}


} /* end of namespace */
#endif /* __TUPLE_H__ */
