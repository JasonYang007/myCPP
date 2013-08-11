#ifndef __CLASS_TRAITS_H
#define __CLASS_TRAITS_H

#include <ostream>
#include <iterator>
#include <vector>
#include "Import_type_traits.h"
#include "boost_pp.h"
#include "Tuples.h"

#ifdef __GNUC__
    #include <tr1/memory>
    #define TSHARED_PTR std::tr1::shared_ptr
    #define decltype typeof
#else
    #include <memory>
    #define TSHARED_PTR std::shared_ptr
#endif

using namespace tuples;

template<typename T> 
struct array_traits 
{ 
    enum {len=0, dim=0, total=1};
    typedef tuples::tNULL ptype;
    template<int> static int size(void) { return 0; }
    static ptype _pair(void) { return ptype();}
};

namespace array_traits_detail
{
    template<typename T, int n, int i> struct _size
    {
        static int call(void) { return array_traits<T>::template size<i-1>(); }
    };
    template<typename T, int n> struct _size<T,n,0>
    {
        static int call(void) { return n; }
    };
}

template<typename T, int n>
struct array_traits<T[n]> 
{ 
    typedef typename array_traits<T>::ptype other_ptype;
    typedef _pair<int, other_ptype> ptype;
    typedef typename tuple_type<ptype>::type TShape;

    enum {len=n, dim=array_traits<T>::dim+1, total=n*array_traits<T>::total}; 
    template<int i> static int size(void) { return array_traits_detail::_size<T,n,i-1>::call(); }
    static ptype _pair(void) { return ptype(n, array_traits<T>::_pair()); }
    static TShape shape(void) { return make_tuple(_pair());}
};


template<typename T> 
struct func_traits { typedef void result_type; enum {arity=0}; typedef void class_type;};

//template<typename R,typename T1,typename T2,typename T3>
//struct func_traits<R (T1,T2,T3)> {typedef R result_type; enum {arity=3}; 
//typedef void class_type;typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;};
//
//template<typename R,typename C,typename T1,typename T2,typename T3>
//struct func_traits<R (C::*)(T1,T2,T3) const> {typedef R result_type; enum {arity=3}; 
//typedef C class_type;typedef T1 arg1_type;typedef T2 arg2_type;typedef T3 arg3_type;};

#define TAG_Tn_argn_type(z,N,_) typedef BOOST_PP_CAT(T,N) BOOST_PP_CAT(arg,BOOST_PP_INC(N))_type;

#define DEF_func_traits(z,N,_)\
    template<typename R BOOST_PP_COMMA_IF(N) MYPP_ENUM(N, TAG_class_Tn,_)>\
    struct func_traits<R (MYPP_ENUM(N, TAG_Tn,_))> \
    {\
        typedef R result_type; enum {arity=N}; \
        typedef void class_type; BOOST_PP_REPEAT(N, TAG_Tn_argn_type,_) };\
    \
    template<typename R, typename C BOOST_PP_COMMA_IF(N) MYPP_ENUM(N, TAG_class_Tn,_)>\
    struct func_traits<R (C::*)(MYPP_ENUM(N, TAG_Tn,_)) const> \
    {\
        typedef R result_type; enum {arity=N}; \
        typedef C class_type; BOOST_PP_REPEAT(N, TAG_Tn_argn_type,_)\
    };


BOOST_PP_REPEAT(NPARAMS, DEF_func_traits, _)

template<typename T>
struct func_traits<T *>:func_traits<typename remove_cv<T>::type> {};

template<typename T>
struct func_traits<T &>:func_traits<typename remove_cv<T>::type> {};

template<typename C, typename T>
struct func_traits<T C::*>:func_traits<typename remove_cv<T>::type> { typedef C class_type;};

//********** helper for functor
template<int n, typename F, F _func>
struct def_call {};

#define DEF_def_call(z,N,_) \
    template<typename F, F func>\
    struct def_call<N,F,func>\
    {\
	    typedef typename func_traits<F>::result_type result_type;\
        BOOST_PP_REPEAT(N,FUNC_TRAITS_ARG,_)\
	    result_type operator() (MYPP_ENUM(N,TAG_ARGTYPE_Xn,_)) {return func(MYPP_ENUM(N, TAG_Xn,_));}\
    };\

BOOST_PP_REPEAT(NPARAMS, DEF_def_call, _)

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
//

//********* functor

template<typename F, F func>
struct functor: func_traits<F>, def_call<func_traits<F>::arity, F, func> {};

//********* operator() detection
#ifdef _MSC_VER
template<typename T>
struct operator_traits
{
    template<typename F, F f> struct yes { typedef F type;};
    struct no { typedef void type; };

    template<typename S>
    static yes<decltype(&S::operator()), &S::operator()> test(S *);

    static no test(...);

    typedef decltype(test((T *)NULL)) _T;
    typedef typename _T::type type; // type: operator()'s type or void
    // typedef typename func_traits<type>::result_type result_type;

    enum {value=!is_void<type>::value};
};
#endif

//********** type traits
template<bool, typename T, typename F>
struct _if { typedef F type;};

template<typename T, typename F>
struct _if<true,T,F> { typedef T type;};

template<typename B,typename D>
struct is_proper_base_of:   
    _if<is_base_of<B,D>::value && !is_same<B,D>::value, true_type, false_type>::type {};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class ISerialize
{
public:
    ~ISerialize() {}
    virtual bool Serialize(std::ostream & os) const{ os << "Serialize" << std::endl; return true;}
};

class DDD: public ISerialize
{
    int ii;
public:
    DDD(): ii(456) {}
    ~DDD() {}
    bool Serialize(std::ostream & os) const;
};


struct builtin_tag {};
struct iterable_tag {};
struct pointer_tag {};
struct serialize_tag {};

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, const T & obj);

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, const char * subNodeName, const T & obj);

namespace Serialize
{
template <typename T>
struct class_traits
{
    typedef typename T::iterator Iter;
    typedef iterable_tag class_category;
};

template <typename T>
struct class_traits<T *>
{
    typedef pointer_tag class_category;
};

template <typename T>
struct class_traits<TSHARED_PTR<T> >
{
    typedef pointer_tag class_category;
};

template <>
struct class_traits<ISerialize>
{
    typedef serialize_tag class_category;
};

#define DEF_BUILTIN(T) template<> struct class_traits<T> { typedef builtin_tag class_category;}

DEF_BUILTIN(char);
DEF_BUILTIN(unsigned char);
DEF_BUILTIN(wchar_t);
DEF_BUILTIN(short);
DEF_BUILTIN(unsigned short);
DEF_BUILTIN(int);
DEF_BUILTIN(unsigned);
DEF_BUILTIN(long);
DEF_BUILTIN(unsigned long);
DEF_BUILTIN(float);
DEF_BUILTIN(double);
DEF_BUILTIN(bool);

void XML_BEGIN(std::ostream &os, const char * nodeName);
void XML_END(std::ostream &os);


template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, const char * subNodeName, T & obj, const iterable_tag&)
{
    bool br = true;
    if(NULL != nodeName) XML_BEGIN(os, nodeName);
    for(typename T::const_iterator it = obj.begin(); it != obj.end() && br; ++it)
    {
        br = ::XML_OUT(os, subNodeName, (*it));
    }
    if(NULL != nodeName) XML_END(os);
    return br;
}

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, T & obj, const iterable_tag&)
{
    bool br = true;
    if(NULL != nodeName) XML_BEGIN(os, nodeName);
    //std::copy(obj.begin(),  obj.end(),
    //            std::ostream_iterator<T::iterator::value_type>(os, " "));
    XML_OUT(os, nodeName, NULL, obj, iterable_tag());
    if(NULL != nodeName) XML_END(os);
    return br;
}

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, T & obj, const builtin_tag&)
{
    bool br = true;
    if(NULL != nodeName) XML_BEGIN(os, nodeName);
    os << obj;
    if(NULL != nodeName) XML_END(os);
    return br;
}

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, T & obj, const pointer_tag&)
{   
    bool br = true;
    if(NULL != obj){
        br = ::XML_OUT(os, nodeName, *obj);
    }
    return br;
}

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, T & obj, const serialize_tag&)
{   
    bool br = true;
    if(NULL != nodeName) XML_BEGIN(os, nodeName);
    br = obj.Serialize(os);
    if(NULL != nodeName) XML_END(os);
    return br;
}

} /* end of namespace Serialize */


template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, const T & obj)
{
    bool br = Serialize::XML_OUT(os, nodeName, obj, typename Serialize::class_traits<T>::class_category());
    return br;
}

template <typename T>
bool XML_OUT(std::ostream &os, const char * nodeName, const char * subNodeName, const T & obj)
{
    bool br = Serialize::XML_OUT(os, nodeName, subNodeName, obj, typename Serialize::class_traits<T>::class_category());
    return br;
}

//*** XML object: inherited from ostream
typedef enum {NODE_DATA, NODE_OBJECT} ENUM_NODE_TYPE;

struct TreeNode
{
    typedef TSHARED_PTR<TreeNode> TShared_Ptr;
    typedef 
    
    class iterator: public std::iterator<std::bidirectional_iterator_tag, TreeNode>
    {
        iterator(TreeNode * ptr): m_oPtr(ptr) {}
        iterator(const iterator & other): m_oPtr(other.m_oPtr) {}
        ~iterator(void) {}
        iterator & operator ++();
        iterator & operator --();

        bool operator ==(const iterator & other) const {return m_oPtr==other.m_oPtr;}
        bool operator !=(const iterator & other) const { return m_oPtr!=other.m_oPtr;}

        TreeNode & operator *() { return *m_oPtr; }
        iterator & operator +=(size_t offset);

        size_t offset() const;

    private:
        TreeNode * m_oPtr;

    };

    ENUM_NODE_TYPE m_oType;
    std::string    m_oTag;
    TShared_Ptr    m_oParent;
    std::string    m_oValue; // valid for terminal leaf node
    std::vector<TShared_Ptr> m_oChildren; // for internal nodes

    void setType(const ENUM_NODE_TYPE & nodeType) { m_oType=nodeType;}
    ENUM_NODE_TYPE getType(void) const { return m_oType;}

    void setTag(const std::string & nodeTag) { m_oTag=nodeTag;}
    const std::string & getTag(void) const { return m_oTag;}
};

class XLM_ostream: public std::ostream
{
public:
    void startNode(ENUM_NODE_TYPE nodeType, std::string & nodeTag=default_tag);
    void endNode(void);

    template <typename T>
    bool addData(const T & obj);
    
    template <typename T>
    XLM_ostream & operator << (const T & obj);

private:
    static std::string default_tag;
};


#endif /* __CLASS_TRAITS_H */

