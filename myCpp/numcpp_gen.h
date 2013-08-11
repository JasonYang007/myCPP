//********************** Expression Template **********************

using memory::shared_array;
using namespace tuples;

typedef int Integer;

// Forward declaration
class _Range;

template<typename T> class _Array;
template<typename T> class _SArray;
template<typename T, typename E=_Array<T> > struct Array;
template<typename T, typename E=_Array<T> > struct Matrix;
template<typename ASS_FUNC> struct Array_Assign;
template<typename T> struct SYMMAT;
template<typename T> struct SMatrix;
template<typename T> struct UTMatrix;
template<typename T> struct LTMatrix;
template<typename T> struct DMatrix;
template<typename T, typename E> struct MTRANS;
template<typename T, typename E> struct MRESHAPE;
template<typename T, typename E> T max(Array<T,E>  const& arr);
template<typename T, typename E> T min(Array<T,E>  const& arr);
template<typename T, typename E1, typename E2, typename F> struct MBinOp;
template<typename T, typename E1, typename E2> struct MADD;
template<typename T, typename E1, typename E2> struct MSUB;
template<typename T, typename E1, typename E2> struct MMULT;
template<typename T, typename E1, typename E2> struct MDIV;
template<typename T, typename E1, typename E2> struct MMOD;
template<typename T1,typename E1, typename T2, typename E2> struct ZIP;
template<typename T, typename E,typename E1,typename E2> struct INDEX2;
template<typename T> struct Identity;
template<typename K> struct KeyComp;
template<typename T, typename E, typename F> struct UnaryOp;
template<typename T, typename E, typename F> struct MUnaryOp;
template<typename T, typename S> struct _view;
template<typename T> Array<T,_SArray<T> >  constasarray(T const & val);
template<typename T, typename E> struct MREPEAT;
template<typename T, typename E> struct Iterator;
template<typename T, typename E> struct Const_Iterator;
template<typename T> struct complex_traits;
template<typename T> struct sum_prod;
template<typename T, typename E> typename sum_prod<Array<T,E> >::type sum(Array<T,E>  const& );
template<typename T, typename E> typename sum_prod<Matrix<T,E> >::type sum(Matrix<T,E>  const& , Integer);
template<typename T, typename E> typename sum_prod<Array<T,E> >::cum_type cum_sum(Array<T,E>  const& );
template<typename T, typename E> typename sum_prod<Matrix<T,E> >::cum_type cum_sum(Matrix<T,E>  const& , Integer);
template<typename T, typename E>
    Array<tuple<Integer,Integer>, ZIP<Integer,_Array<Integer>, Integer, _Array<Integer> > > nonzero(Matrix<T,E>  const& mat);

enum ENUM_RAND { UNIFORM, NORMAL };
template<ENUM_RAND> struct random;

#define _N  INT_MAX
#define LVALUE_PTR(E,expr) (IsLValue<E >::value ? const_cast<E *>(&expr): NULL)
#define LVALUEM_PTR(T,E,expr) (IsLValue<Matrix<T,E > >::value ? const_cast<Matrix<T,E >*>(&expr): NULL)

#define CAST_PTR(D,ptr)   static_cast<D *>(ptr)
#define CAST_THIS(D)      CAST_PTR(D,this)

#define CONST_CAST_PTR(D,ptr) const_cast<typename remove_cv<D >::type *>(ptr)
#define CONST_CAST_THIS(D)    CONST_CAST_PTR(D,this)

#define CAST_OBJ(D,obj)       static_cast<D &>(obj)
#define CONST_CAST_OBJ(D,obj) const_cast<typename remove_cv<D >::type &>(obj)

#define INT_RANGE(idx) _Range(idx,idx+1,1)

#define DEFAULT_WIDTH 10 // width for streaming out the array data

// Generic iterator
template<typename T, typename E, typename D, typename diff_t>
struct IteratorBase
{
    IteratorBase(): idx(0) {}
    IteratorBase(E const & _expr, Integer _idx=0): expr(_expr), idx(_idx) {}
    
    bool operator == (D const & other) { return idx == other.idx;}
    bool operator != (D const & other) { return idx != other.idx;}
    bool operator < (D const & other) { return idx < other.idx; }
    bool operator <= (D const & other) { return idx <= other.idx; }
    bool operator > (D const & other) { return idx > other.idx; }
    bool operator >= (D const & other) { return idx >= other.idx; }

    D & operator ++ (void) { ++idx; return CAST_OBJ(D,*this);}
    D operator ++ (int)  { D it(expr,idx); ++idx; return it;}

    D & operator -- (void) { --idx; return CAST_OBJ(D,*this);}
    D operator -- (int)  { D it(expr,idx); --idx; return it;}

    D & operator += (size_t offset) { idx += (Integer)offset; return CAST_OBJ(D,*this);}
    D & operator -= (size_t offset) { idx -= (Integer)offset; return CAST_OBJ(D,*this);}

    D operator + (size_t offset) { return D(expr, idx + (Integer)offset);}
    D operator - (size_t offset) { return D(expr, idx - (Integer)offset);}
    diff_t operator - (D const & other) { return (diff_t)(idx - other.idx);}

    Integer offset(void) const { return idx; }

    E expr;
    Integer idx;
};


template<typename T, typename E>
struct Iterator: IteratorBase<T,E,Iterator<T,E>,Integer>, 
                 std::iterator<std::random_access_iterator_tag, T, Integer, T*, T&>
{
    typedef IteratorBase<T,E,Iterator<T,E>,Integer> TBase;
    typedef std::iterator<std::random_access_iterator_tag, T, Integer, T*, T&> TIter;
    
    Iterator() {}
    Iterator(E & _expr, Integer _idx=0): TBase(_expr,_idx) {}
    
    typename TIter::reference operator *(void) {return this->expr[this->idx];}
    typename TIter::pointer operator ->(void) const {return &this->expr[this->idx];}
    typename TIter::reference operator [](Integer index) { return this->expr[this->idx+index];}
};

template<typename T, typename E>
struct Const_Iterator:  IteratorBase<T const,E,Const_Iterator<T,E>,Integer>, 
                        std::iterator<std::random_access_iterator_tag, T const, Integer, T const*, T const&>
{
    typedef IteratorBase<T const,E,Const_Iterator<T,E>,Integer> TBase;
    typedef std::iterator<std::random_access_iterator_tag, T const, Integer, T const*, T const&> TIter;
    Const_Iterator() {}
    Const_Iterator(E const & _expr, Integer _idx=0): TBase(const_cast<E &>(_expr),_idx) {}
    Const_Iterator(Iterator<T, E>  const& iter): TBase(const_cast<E &>(iter.expr),iter.idx) {}

    typename TIter::value_type operator *(void) const {return this->expr[this->idx];}
    typename TIter::pointer operator ->(void) const {return &this->expr[this->idx];}
    typename TIter::reference operator [](Integer index) const { return this->expr[this->idx+index];}
};


template<typename D, typename E>
struct DefConstIter
{
    typedef typename E::const_iterator const_iterator;
    const_iterator begin(void) const { return CAST_THIS(D const)->expr.begin();}
    const_iterator end(void) const { return CAST_THIS(D const)->expr.end();}
};

template<typename D, typename E>
struct DefIter
{
    typedef typename E::iterator iterator;
    iterator begin(void) { return CAST_THIS(D)->expr.begin();}
    iterator end(void) { return CAST_THIS(D)->expr.end();}
};


template<typename D, typename E>
struct DefBothIter
{
    typedef typename E::iterator iterator;
    iterator begin(void) { return CAST_THIS(D)->expr.begin();}
    iterator end(void) { return CAST_THIS(D)->expr.end();}

    typedef typename E::const_iterator const_iterator;
    const_iterator begin(void) const { return CAST_THIS(D const)->expr.begin();}
    const_iterator end(void) const { return CAST_THIS(D const)->expr.end();}
};

#define EXPOSE_ITER(TIter)\
    typedef typename TIter::const_iterator const_iterator;\
    typedef typename TIter::iterator iterator;\
    using TIter::begin;\
    using TIter::end;

#define EXPOSE_CONST_ITER(TIter)\
    typedef typename TIter::const_iterator const_iterator;\
    using TIter::begin;\
    using TIter::end;

//---------- pointer iterator

template<typename T>
class Pointer_Iterator: public std::iterator<std::random_access_iterator_tag, T, Integer, T*, T&>
{
public:
    typedef Pointer_Iterator TSelf;
    typedef std::iterator<std::random_access_iterator_tag, T, Integer, T*, T&> TIter;

    Pointer_Iterator(T * _ptr): ptr(_ptr) {}
    Pointer_Iterator(TSelf const & other): ptr(other.ptr) {}
    
    TSelf & operator ++ () { ++ptr; return *this;}
    TSelf operator ++ (int) { TSelf tmp(*this); operator++(); return tmp;}

    TSelf & operator -- () { --ptr; return *this;}
    TSelf operator -- (int) { TSelf tmp(*this); operator--(); return tmp;}

    TSelf & operator + (size_t offset) { ptr += offset; return *this;}
    TSelf & operator - (size_t offset) { ptr -= offset; return *this;}

    bool operator == (TSelf const & other) { return ptr == other.ptr;}
    bool operator != (TSelf const & other) { return ptr != other.ptr;}
    bool operator < (TSelf const & other) { return ptr < other.ptr; }
    bool operator <= (TSelf const & other) { return ptr <= other.ptr; }
    bool operator > (TSelf const & other) { return ptr > other.ptr; }
    bool operator >= (TSelf const & other) { return ptr >= other.ptr; }

    typename TIter::difference_type operator - (TSelf const & other) { return (typename TIter::difference_type)(ptr - other.ptr);}

    typename TIter::reference operator *() { return *ptr;}
    typename TIter::pointer operator ->() { return ptr;}

    T * get() {return ptr;}

private:
    T * ptr;
};

template<typename T>
inline Pointer_Iterator<T> operator + (Pointer_Iterator<T> const& it, Integer offset)
{
    Pointer_Iterator<T>(it.get()+offset);
}

template<typename T>
inline Pointer_Iterator<T> operator - (Pointer_Iterator<T> const& it, Integer offset)
{
    Pointer_Iterator<T>(it.get()-offset);
}

template<typename T>
inline Pointer_Iterator<T> operator + (Integer offset, Pointer_Iterator<T> const& it)
{
    Pointer_Iterator<T>(it.get()+offset);
}


//***************

template<typename T> struct IsLValue: false_type {};
template<typename T> struct IsLValue<_Array<T> >: true_type {};
template<typename T> struct IsLValue<Matrix<T, _Array<T> > >: true_type {};
template<typename T> struct IsLValue<SMatrix<T> >: true_type {};
template<typename T> struct IsLValue<UTMatrix<T> >: true_type {};
template<typename T> struct IsLValue<LTMatrix<T> >: true_type {};
template<typename T> struct IsLValue<DMatrix<T> >: true_type {};

struct LRValue
{
    LRValue(void): lvalue(false), unnamed(true) {}
    LRValue(bool _lvalue, bool _unnamed): lvalue(_lvalue), unnamed(_unnamed) {}

    bool lvalue;
    bool unnamed;
};

LRValue unnamed_lvalue(void);
#define SET_NAMED_LVALUE   this->lvalue=true; this->unnamed=false;
#define SET_LRVALUE(obj)   this->lvalue=(obj).lvalue; this->unnamed=(obj).unnamed;

template<typename D>
struct RefCountCRTPBase
{
    size_t ref_count(void *ptr) const 
    { 
        return CAST_THIS(D const)->ref_count_cum(ptr) - CAST_THIS(D const)->ref_count_node(ptr);
    }
};

template<typename D>
struct RefCountCRTP: RefCountCRTPBase<D>
{
    size_t ref_count_node(void * ptr) const 
    { 
        D const * Pptr = CAST_THIS(D const);
        return Pptr->expr.ref_count_node(ptr);
    }
    size_t ref_count_cum(void * ptr) const 
    {
        D const * Pptr = CAST_THIS(D const);
        return ref_count_node(ptr) + Pptr->expr.ref_count_cum(ptr);
    }
};

template<typename D>
struct RefCount2CRTP: RefCountCRTPBase<D>
{
    size_t ref_count_node(void * ptr) const 
    { 
        D const * Pptr = CAST_THIS(D const);
        return Pptr->expr1.ref_count_node(ptr) + Pptr->expr2.ref_count_node(ptr);
    }
    size_t ref_count_cum(void * ptr) const 
    {
        D const * Pptr = CAST_THIS(D const);
        return ref_count_node(ptr) + Pptr->expr1.ref_count_cum(ptr)+ Pptr->expr2.ref_count_cum(ptr);
    }
};

//**** PointerQuery
template<typename T>
struct NullPointerQuery
{
    T * get(void) const { return get_sptr().get(); }
    shared_array<T>  const get_sptr(void) const { return shared_array<T>();}
    size_t use_count(void) const { return 0;}
    bool Qdo_copy(void * ptr) const { return false; }
};

template<typename D, typename T>
struct PointerQuery
{
    T * get(void) const { return get_sptr().get(); }
    shared_array<T>  const get_sptr(void) const { return CAST_THIS(D const )->expr.get_sptr();}
    size_t use_count(void) const { return CAST_THIS(D const )->expr.use_count();}
    bool Qdo_copy(void * ptr) const { return CAST_THIS(D const )->expr.Qdo_copy(ptr);}
    
    shared_array<T> & dup(void) { return CAST_THIS(D)->expr.dup();}
    void reset(void * oldPtr, shared_array<T>  const & newPtr) 
    { 
        expr_reset(CAST_THIS(D)->expr, CAST_THIS(D)->ptr_expr, oldPtr, newPtr);
    }

    shared_array<T> & resize(Integer _nrows, Integer _ncols=1)
    {   
        if(CAST_THIS(D)->ptr_expr) CAST_THIS(D)->ptr_expr->resize(_nrows, _ncols);
        return CAST_THIS(D)->expr.resize(_nrows, _ncols);
    }

    void setsize(Integer len) { CAST_THIS(D)->expr.setsize(len);}
};

#define EXPOSE_PTR_QUERY(TPtrQuery) \
    using TPtrQuery::get;\
    using TPtrQuery::get_sptr;\
    using TPtrQuery::use_count;\
    using TPtrQuery::Qdo_copy;\
    using TPtrQuery::dup;\
    using TPtrQuery::reset;\
    using TPtrQuery::resize;\
    using TPtrQuery::setsize;


template<typename D, typename T>
struct MPointerQuery: PointerQuery<D,T>
{
    shared_array<T> & resize(Integer _nrows, Integer _ncols=1)
    {   
        CAST_THIS(D)->nrows=_nrows;
        CAST_THIS(D)->ncols=_ncols;
        if(CAST_THIS(D)->ptr_expr) CAST_THIS(D)->ptr_expr->resize(_nrows, _ncols);
        return CAST_THIS(D)->expr.resize(_nrows, _ncols);
    }
};

template<typename D, typename T>
struct PointerQuery2
{
    T * get(void) const { return get_sptr().get(); }
    shared_array<T>  const get_sptr(void) const { return CAST_THIS(D const )->expr1.get_sptr();}
    size_t use_count(void) const { return CAST_THIS(D const )->expr1.use_count();}
    bool Qdo_copy(void * ptr) const { return CAST_THIS(D const )->expr1.Qdo_copy(ptr) || 
                                             CAST_THIS(D const )->expr2.Qdo_copy(ptr);}

    shared_array<T> & dup(void) { return CAST_THIS(D)->expr1.dup();}

    void reset(void * oldPtr, shared_array<T>  const & newPtr) 
    { 
        expr_reset(CAST_THIS(D)->expr1, CAST_THIS(D)->ptr_expr1, oldPtr, newPtr);
    }

    shared_array<T> & resize(Integer _nrows, Integer _ncols=1)
    {
        if(CAST_THIS(D)->ptr_expr1) CAST_THIS(D)->ptr_expr1->resize(_nrows, _ncols);
        return CAST_THIS(D)->expr1.resize(_nrows, _ncols);
    }
};


//**** Binary Operator

template<typename D, typename E1, typename E2, typename T>
struct BinOpBase: RefCount2CRTP<BinOpBase<D,E1,E2,T> >, NullPointerQuery<T>
{
    BinOpBase(E1 const  & _expr1, E2 const  &_expr2): expr1(_expr1), expr2(_expr2) {}
    Integer size(void) const { return std::max(expr1.size(),expr2.size());}
        
    typedef T value_type;
    typedef Const_Iterator<T, D> const_iterator;

    const_iterator begin(void) const { return const_iterator(*CAST_THIS(D const ), 0);}
    const_iterator end(void) const { return const_iterator(*CAST_THIS(D const ), size());}

    bool Qdo_copy(void * ptr) const { return expr1.Qdo_copy(ptr) || expr2.Qdo_copy(ptr);}

    E1 const  expr1;
	E2 const  expr2;
};


template<typename T, typename E1, typename E2, typename F>
struct BinOp: BinOpBase<BinOp<T,E1,E2,F>, E1, E2, T>
{
    BinOp(E1 const  & _expr1, E2 const  &_expr2, F const  & _func): 
            BinOpBase<BinOp<T,E1,E2,F>,E1,E2,T>(_expr1, _expr2), func(_func)
    { 
        assert(this->expr1.size()==0 || this->expr2.size()==0 || this->expr1.size()==this->expr2.size());
    }
    T operator [](Integer idx) const { return func(this->expr1[idx], this->expr2[idx]);}
        
    F func;
};


//**************************************************

template<typename D, typename E, typename T, typename R=T>
struct UnaryOpBase: RefCountCRTP<UnaryOpBase<D,E,T,R> >, NullPointerQuery<T>
{
    UnaryOpBase(E const  & _expr): expr(_expr) {}
    Integer size(void) const { return expr.size();}

    typedef R value_type;
    typedef Const_Iterator<T, D> const_iterator;

    const_iterator begin(void) const { return const_iterator(CAST_OBJ(D const ,*this), 0);}
    const_iterator end(void) const { return const_iterator(CAST_OBJ(D const ,*this), size());}

    bool Qdo_copy(void * ptr) const { return expr.Qdo_copy(ptr);}

    E const  expr;
};


//******** check_dup

template<typename T, typename E>
void expr_reset(E & expr, E * & ptr_expr, void *oldPtr, shared_array<T>  const & newPtr)
{
    expr.reset(oldPtr, newPtr);
    if(ptr_expr == NULL) return;
    ptr_expr->reset(oldPtr, newPtr);
}

template<typename T1, typename T2>
size_t extern_count(T1 & expr1, T2 const  & expr2)
{
    void * ptr = expr1.get_sptr().get();
    size_t UseCnt = expr1.use_count();
    size_t RefCnt1 = expr1.ref_count(ptr);
    size_t RefCnt2 = expr2.ref_count(ptr);
    size_t ExtCnt = UseCnt - (RefCnt1+RefCnt2+1);

    return ExtCnt;
}

template<typename T1, typename T2>
void check_dup(T1 & expr1, T2 const  & expr2)
{
    void * ptr = expr1.get_sptr().get();
    size_t ExtCnt = extern_count(expr1, expr2);
    if(ExtCnt > 0) 
    {
        expr1.reset(ptr, expr1.dup());
    }
}

//**********************************************************************

namespace INDEX_detail
{
    template<typename T, typename E1, typename E2>
    struct _assert
    {
        static void call(E1 const  & expr1, E2 const  & expr2)
        {
            Array<T,E2> arr(expr2);

            if(expr1.size() && expr2.size()) 
                assert( 0 <= min(arr) && max(arr) < expr1.size());
        }
    };
    template<typename T, typename E1, typename T1, typename R1, typename T2, typename R2>
    struct _assert<T,E1,ZIP<T1,R1,T2,R2> >
    {
        typedef ZIP<T1,R1,T2,R2> E2;
        static void call(E1 const  & expr1, E2 const  & expr2)
        {
            Array<T1,R1> arr1(expr2.expr1);
            Array<T2,R2> arr2(expr2.expr2);

            assert( 0 <= min(arr1) && max(arr1) < expr1.nrows);
            assert( 0 <= min(arr2) && max(arr2) < expr1.ncols);
        }
    };

}
template<typename T, typename E1, typename E2>
struct INDEX: RefCount2CRTP<INDEX<T,E1,E2> >, PointerQuery2<INDEX<T,E1,E2>, T>
{
    typedef INDEX TSelf;
    typedef T value_type;
	typedef Iterator<T, TSelf> iterator;
    typedef Const_Iterator<T, TSelf> const_iterator;

	INDEX(E1 & _expr1, E2 const  &_expr2)
        : expr1(_expr1), ptr_expr1(LVALUE_PTR(E1,_expr1)), expr2(do_adjust(_expr2, _expr1.size()))  
    {   
        INDEX_detail::template _assert<T,E1,E2>::call(expr1, expr2);
    }

    T & operator [](Integer idx) { return expr1[expr2[idx]];}
	T const  operator [](Integer idx) const { return expr1[expr2[idx]];}

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, expr2.size());}

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, expr2.size());}

    Integer size(void) const { return std::min(expr1.size(), expr2.size());}
    void setsize(Integer len) {expr1.setsize(len);}

    E1  expr1;
    E1 * ptr_expr1;
	E2 expr2;
};



template<typename T, typename E>
struct REPEAT: RefCount2CRTP<REPEAT<T,E> >, PointerQuery<REPEAT<T,E>, T>
{
    typedef REPEAT TSelf;
    typedef T value_type;
	typedef Iterator<T, TSelf> iterator;
    typedef Const_Iterator<T, TSelf> const_iterator;
    
    REPEAT(E const  & _expr, Integer _repeat): expr(_expr), repeat(_repeat), ptr_expr(LVALUE_PTR(E,_expr)) {}   

    T & operator [](Integer idx) { return expr[idx/repeat];}
	T const  operator [](Integer idx) const { return expr[idx/repeat];}

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, size());}

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, size());}

    Integer size(void) const { return expr.size()*repeat; }

    E  expr;
    E * ptr_expr;
    Integer repeat;
};

struct Shape
{
    Shape(Integer const * _len, Integer _rank): len(_len), rank(_rank) {}
    
    Integer operator[](Integer idx) const { return *(len+idx);}
    size_t size() const { return rank;}

private:
    Integer const * len;
    Integer rank;
};

template<int n, typename T, typename E>
struct _ndarray: RefCountCRTP<_ndarray<n,T,E> >, PointerQuery<_ndarray<n,T,E>, T>
{
    _ndarray(E & _expr): expr(_expr), ptr_expr(LVALUE_PTR(E,_expr)) {}

    void reset(Integer (&_len)[n])
    {
        std::copy(_len, _len+n, len);
        prod[0]=1;
        for(int ii=1; ii < n; ++ii)
            prod[ii] = prod[ii-1]*len[ii-1];
    }
    
    T & operator [](Integer idx) { return expr[idx];}
    const T  operator [](Integer idx) const { return expr[idx];}
    Integer size(void) const {return len[n-1]*prod[n-1];}

    typedef Const_Iterator<T,_ndarray<n,T,E> > const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0); }
    const_iterator end(void) const { return const_iterator(*this, size()); }

    typedef Iterator<T,_ndarray<n,T,E> > iterator;
    iterator begin(void) { return iterator(*this, 0); }
    iterator end(void) { return iterator(*this, size()); }

    template<int k>
    void nzRange(Integer idx, Integer & start, Integer & stop) const { start=0; stop=len[k];}

    Integer rank(void) const { return n;}
    Shape shape(void) const { return Shape(&len[0],n);}

    E expr;
    E * ptr_expr;
    Integer len[n];
    Integer prod[n];
};

template<int n, class R> struct SumProd;

#define TAG_arr_xn(z,N,_) BOOST_PP_IF(N,+,) arr[N]*x ## N
#define TAG_T_xn(z,N,_)   BOOST_PP_COMMA_IF(N) T x ## N

#define DEF_SumProd(z,N,_)\
    template<class R>\
    struct SumProd<BOOST_PP_INC(N), R>\
    {\
        template<typename T>\
        static R eval(T (&arr)[BOOST_PP_INC(N)], MYPP_ENUM0(N, TAG_T_xn,_) ) { return MYPP_ENUM0(N,TAG_arr_xn,_); }\
    };

BOOST_PP_REPEAT(NPARAMS, DEF_SumProd, _)
#undef TAG_arr_xn
#undef TAG_T_xn
//
//template<class R>
//struct SumProd<3, R>
//{
//    template<typename T>
//    static R eval(T (&arr)[3], T x0, T x1, T x2) { return a[0]*x0+a[1]*x1+a[2]*x2; }
//};

template<int n, typename T, typename E=_Array<T> > struct ndarray;

#define TAG_Int_xn(z,N,_) BOOST_PP_COMMA_IF(N) Integer x ## N
#define TAG_lenn_xn(z,N,_) len[N]=x ## N;

#define DEF_NDARRAY(z,N,_) \
    template<typename T, typename E> \
    struct ndarray<BOOST_PP_INC(N),T,E>: _ndarray<BOOST_PP_INC(N),T,E>\
    {\
        typedef _ndarray<BOOST_PP_INC(N),T,E> TBase;\
        typedef T value_type;\
        \
        ndarray(E expr, MYPP_ENUM0(N,TAG_Int_xn,_)): TBase(expr)\
        {\
            Integer len[BOOST_PP_INC(N)];\
            MYPP_ENUM0(N, TAG_lenn_xn,_) \
            TBase::reset(len);\
        }\
        T & operator() (MYPP_ENUM0(N,TAG_Int_xn,_)) { return TBase::expr[SumProd<BOOST_PP_INC(N),Integer>::eval(TBase::prod, MYPP_ENUM0(N,TAG_Xn,_))];}\
        T  operator() (MYPP_ENUM0(N,TAG_Int_xn,_)) const { return TBase::expr[SumProd<BOOST_PP_INC(N),Integer>::eval(TBase::prod, MYPP_ENUM0(N,TAG_Xn,_))];}\
    };

BOOST_PP_REPEAT(NPARAMS, DEF_NDARRAY, _)

#undef TAG_lenn_xn

template<typename T, typename E>
std::ostream & operator << (std::ostream & os, const ndarray<1,T,E> & ndarr)
{
    Shape sh = ndarr.shape();

    char const  * delimPtr="[";
    os << delimPtr; delimPtr="";
    for(Integer i0=0; i0 < sh[0]; ++i0)
	{
		os << delimPtr;
        StreamOut(os, ndarr(i0));
        delimPtr = ", ";
	}
	os << "]";

	return os;
}

//template<typename T, typename E>
//std::ostream & operator << (std::ostream & os, const Array<T, ndarray<2,T,E> > & ndarr)
//{
//    Shape sh = ndarr.shape();
//
//    char const  * delimPtr="[";
//    os << delimPtr; delimPtr="";
//    for(Integer i0=0; i0 < sh[0]; ++i0)
//    {
//        os << delimPtr << ndarr(i0, R(0));
//	}
//	os << "]";
//
//	return os;
//}

//
//template<typename T, typename E> 
//struct ndarray<3,T,E>: _ndarray<3,T,E>
//{
//    typedef _ndarray<3,T,E> TBase;
//
//    ndarray(Array<T,E>& arr, Integer x0, Integer x1, Integer x2): TBase(arr.expr)
//    {
//        Integer len[3];
//        MYPP_ENUM0(2, TAG_lenn_xn,_) 
//        TBase::reset(len);
//    }
//    T & operator() (Integer x0, Integer x1, Integer x2) { return TBase::expr[SumProd<3,Integer>::eval(TBase::prod, x0, x1, x2)];}
//    T  operator() (Integer x0, Integer x1, Integer x2) const { return TBase::expr[SumProd<3,Integer>::eval(TBase::prod,x0, x1, x2)];}
//};

template<typename D, typename T> struct EXPOSE_NDARRAY {};

#define DEF_EXPOSE_NDARRAY(z,N,_) \
    template<typename D, typename T, typename E>\
    struct EXPOSE_NDARRAY<D, ndarray<BOOST_PP_INC(N),T,E> >\
    {\
        T & operator() (MYPP_ENUM0(N,TAG_Int_xn,_)) { return CAST_THIS(D)->expr(MYPP_ENUM0(N,TAG_Xn,_));}\
        T operator() (MYPP_ENUM0(N,TAG_Int_xn,_)) const { return CAST_THIS(D)->expr(MYPP_ENUM0(N,TAG_Xn,_))};\
    };

BOOST_PP_REPEAT(NPARAMS, DEF_EXPOSE_NDARRAY, _)

//
//template<typename D, typename T, typename E>
//struct EXPOSE_NDARRAY<D, ndarray<3,T,E> >
//{
//    T & operator() (Integer x0, Integer x1, Integer x2) { return CAST_THIS(D)->expr(x0,x1,x2);}
//    T operator() (Integer x0, Integer x1, Integer x2) const { return CAST_THIS(D)->expr(x0,x1,x2)};
//};

#define TAG_xn_star(z,N,_) BOOST_PP_IF(N,*,) x ## N

#define DEF_reshape(z,N,_)\
    template<typename T, typename E>\
    ndarray<BOOST_PP_INC(N),T,E> reshape(Array<T,E>& arr, MYPP_ENUM0(N,TAG_Int_xn,_))\
    {\
        assert(arr.size() == MYPP_ENUM0(N,TAG_xn_star,_));\
        return ndarray<BOOST_PP_INC(N),T,E>(arr.expr,MYPP_ENUM0(N,TAG_Xn,));\
    }

BOOST_PP_REPEAT(NPARAMS, DEF_reshape, _)
#undef TAG_xn_star
#undef TAG_Int_xn

//template<typename T, typename E>
//Array<T, ndarray<3,T,E> > reshape(Array<T,E>& arr, Integer x0, Integer x1, Integer x2)
//{
//    assert(arr.size() == x0*x1*x2);
//    return Array<T, ndarray<3,T,E> >(ndarray<3,T,E>(arr,x0,x1,x2));
//}

template<typename T1, typename E1, typename T2, typename E2>
struct ZIP: RefCount2CRTP<ZIP<T1,E1,T2,E2> >
{
    typedef tuple<T1,T2> value_type;
	typedef Iterator<value_type, ZIP> iterator;
    typedef Const_Iterator<value_type, ZIP> const_iterator;

	ZIP(Array<T1,E1>  const & _expr1, Array<T2,E2>  const &_expr2): expr1(_expr1), expr2(_expr2) 
    {
        assert(expr1.size() == expr2.size());
    }

    value_type  operator [](Integer idx) { return tuple<T1,T2>(expr1[idx], expr2[idx]);}
	value_type const  operator [](Integer idx) const { return tuple<T1,T2>(expr1[idx], expr2[idx]);}

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, expr1.size());}

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, expr2.size());}

    Integer size(void) const { return expr1.size();}

    bool Qdo_copy(void * prt) const { return expr1.Qdo_copy(this->ptr) || expr2.Qdo_copy(this->ptr);}

    Array<T1,E1> expr1;
	Array<T2,E2> expr2;
};


template<typename E>
struct MASK2ARRAY: RefCountCRTP<MASK2ARRAY<E> >, PointerQuery<MASK2ARRAY<E>, typename E::value_type>
{
    typedef MASK2ARRAY TSelf;
    typedef Integer value_type;
	typedef Const_Iterator<Integer, TSelf> const_iterator;

	MASK2ARRAY(E const  & _expr): expr(_expr), len(std::accumulate(expr.begin(), expr.end(), 0)), cache(new Integer[len]) 
    {
        std::fill(cache.get(), cache.get()+len, -1);
    }

	Integer const  operator [](Integer idx) const 
    { 
        Integer cnt = idx;
        Integer offset = cache[idx];
        if(offset >=0) return offset;

        do
        {
            ++offset;
            while(offset < expr.size() && !expr[offset]) ++offset;
            --cnt;
        }while(cnt >= 0);
        
        cache[idx]=offset;

        return offset;
    }

    const_iterator begin(void) const { return const_iterator(expr, 0);}
    const_iterator end(void) const { return const_iterator(expr, len);}

    Integer size(void) const { return len;}

    E expr;
    Integer len;
private:
    mutable shared_array<Integer> cache;
};

//
//************************************************************

template<typename T>
class _Array: LRValue
{
public:
    typedef _Array TSelf;
    typedef T value_type;
    _Array(): arr((T*)NULL), len(0) { SET_NAMED_LVALUE;}
	_Array(T * _arr, Integer _len): arr(_arr), len(_len) { rank_init(); SET_NAMED_LVALUE; }
    _Array(T * _arr, Integer _len,  T (*rndFunc)(void) ): arr(_arr), len(_len)
    {
        rank_init(); 
        SET_NAMED_LVALUE;
        std::generate(get(), get()+len, rndFunc);
    }
    template<ENUM_RAND E>
    _Array(T * _arr, Integer _len, random<E>  const & rndFunc): arr(_arr), len(_len) 
    {
        SET_NAMED_LVALUE;
        std::generate(get(), get()+len, rndFunc);
    }

    template<unsigned n>
    _Array(T (&_arr)[n]): arr(new T[n]), len(n) { std::copy(_arr, _arr+n, arr.get());}

    typedef Const_Iterator<T, TSelf> const_iterator;
    typedef Iterator<T, TSelf> iterator;
    
    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, size());}

    iterator begin(void) { return iterator(*this,0); }
    iterator end(void)   { return iterator(*this,size()); }
        
	T const  & operator [](Integer idx) const {return get()[idx];}
	T & operator [](Integer idx) {return get()[idx];}

    Integer size(void) const {return len;}

    T * get(void) {return arr.get();}
    T const  * get(void) const {return arr.get();}

    shared_array<T>  const get_sptr(void) const { return arr;}
    size_t use_count(void) const { return arr.use_count();}
    bool Qdo_copy(void * ptr) const { return false;}

    size_t ref_count(void *ptr) const { return ref_count_cum(ptr)-ref_count_node(ptr);}
    size_t ref_count_node(void * ptr) const { return (ptr==(void *)get())?1:0;}
    size_t ref_count_cum(void * ptr) const { return ref_count_node(ptr);}


    shared_array<T> & dup(void)
    { 
        size_t ucnt = arr.use_count();
        if(ucnt > 1)
        {
            T * ptr = new T[this->size()];
            std::copy(arr.get(), arr.get()+len, ptr);
            arr.reset(ptr, memory::ArrayDeleter<T>());
        }
        ucnt = arr.use_count();
        return arr;
    }
    void reset(void * oldPtr, shared_array<T>  const & newPtr)  
    { 
        T * ptr = arr.get();
        if(ptr==oldPtr) arr.reset(newPtr);
    }
    shared_array<T> & resize(Integer _nrows, Integer _ncols=1)
    {
        Integer _len = _nrows*_ncols;
        if(size() != _len)
        {
            Array<T> tmp(_len);
            std::copy(begin(), begin()+std::min(len,_len), tmp.begin());
            len = _len;
            arr.reset(tmp.expr.get_sptr());
        }
        return arr;
    }

	template<typename E>
	TSelf & operator = (E const  & other)
	{
        dup();

		for(Integer ii=0; ii < this->size(); ++ii)
		{
			this->arr.get()[ii]=other[ii];
		}
		return *this;
	}
    void setsize(Integer _len) { len=_len; }
    
    Integer rank(void) const {return 1;}

    Shape shape(void) const { return Shape(&len,1); }

    void rank_init(void)
    {
        //for(int i = 1; i < sizeof(Dim)/sizeof(Dim[0]); ++i)
        //    Dim[i]=1;
        //nDim=1;
        //Dim[0]=len;
    }
    //Integer Dim[10];
    //Integer nDim;
private:
    Integer len;
	shared_array<T> arr;
};

class _Range: public NullPointerQuery<Integer>
{
public:
    typedef _Range TSelf;
    typedef Integer value_type;

    _Range(Integer const  _start, Integer const  _stop, Integer const  _step): start(_start), stop(_stop), step(_step) {}
    Integer & operator[] (Integer idx) { assert(idx < size()); tmp=start+step*idx; return tmp; }
    Integer const  operator [] (Integer idx) const { assert(idx < size()); return start+step*idx; }

    typedef Const_Iterator<Integer, TSelf> const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void)  const { return const_iterator(*this, size());}

    Integer size(void) const {return static_cast<Integer>(std::max(0, (Integer)std::ceil((stop-start)*1.0/step)));}
    
    TSelf & adjust(Integer _len)
    {
        if(step == _N) step=1;
        if(step > 0 )
        {
            if(start == _N) start = 0;
            _adjust(start, _len);
            if(stop == _N) stop = _len;
            _adjust(stop, _len, -1);
        }
        else
        {
            if(start == _N) start = _len-1;
            _adjust(start, _len);
            if(stop == _N) 
            {
                stop = -1;
            }
            else
            {
                _adjust(stop, _len, -1);
            }
        }
        return *this;
    }

    size_t ref_count(void *ptr) const { return ref_count_cum(ptr)-ref_count_node(ptr);}
    size_t ref_count_node(void * ptr) const { return 0;}
    size_t ref_count_cum(void * ptr) const { return 0;}

private:
    void _adjust(Integer & val, Integer _len, Integer min_val=0)
    {
        Integer max_val = _len-(1+min_val);

        if(val >= 0)
        {
            val = std::min(val, max_val);
        }
        else
        {
            if(val < 0 ) val += _len;
            val = std::max(val, min_val);
        }
    }

    Integer start, stop, step;
    Integer tmp;
};


template<typename T>
class _Linspace: public NullPointerQuery<Integer>
{
public:
    typedef _Linspace TSelf;
    typedef T value_type;

    _Linspace():start(), stop(), nsteps(), divisor() {}
    _Linspace(T const  _start, T const  _stop, Integer const  _nsteps): start(_start), stop(_stop), nsteps(_nsteps) 
    { 
        divisor=nsteps-1; if(divisor < 1) divisor=1; 
    }
    T const  operator [] (Integer idx) const { assert(idx < size()); return start+((stop-start)/divisor)*idx; }

    typedef Const_Iterator<T, TSelf> const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void)  const { return const_iterator(*this, size());}

    Integer size(void) const {return nsteps;}

    size_t ref_count(void *ptr) const { return ref_count_cum(ptr)-ref_count_node(ptr);}
    size_t ref_count_node(void * ptr) const { return 0;}
    size_t ref_count_cum(void * ptr) const { return 0;}

private:
    T   start, stop;
    Integer nsteps;
    Integer divisor;
};

template<typename T>
class _SArray: public NullPointerQuery<Integer>
{
public:
    typedef _SArray TSelf;
    typedef T value_type;
    struct const_iterator
    {
        const_iterator(T _val): val(_val) {}
        T const  operator *(void) {return val;}
        T const  * operator ->(void) {return &val;}
        const_iterator & operator ++(void) { return *this;}
        const_iterator & operator --(void) { return *this;}
        bool operator == (TSelf const  &) const { return false;}
        bool operator != (TSelf const  &) const { return true;}
        T val;
    };

    const_iterator begin(void) const { return const_iterator(val); }
    const_iterator end(void) const { return   const_iterator(val); }

	_SArray(T const  & _val): val(_val) {};
	T const  & operator [](Integer idx) const {return val;}
	Integer size(void) const {return 0;}

    size_t ref_count(void *ptr) const { return ref_count_cum(ptr)-ref_count_node(ptr);}
    size_t ref_count_node(void * ptr) const { return 0;}
    size_t ref_count_cum(void * ptr) const { return 0;}

private:
	T const  val;
};

template<typename T>
std::ostream & operator << (std::ostream & os, _SArray<T>  const & arr)
{
    std::ios::fmtflags flags = os.flags();
	os << std::left << std::setw(9) << arr.val;
    os.flags(flags);
    return os;
}


//**** Array class definition
template <typename T>
struct array_len {enum {len=0, value=0};};

template <typename T, unsigned n>
struct array_len<T[n]> { enum {len=n, value=1};};

#define DEF_ASSIGN(NAME, OP) \
    template<typename T>\
    struct NAME { bool IsAssign() { return false;} ; T operator()(T & x, T const  & y) { x OP y; return x;} };
#define DEF_ASSIGN_(NAME, OP) \
    template<typename T>\
    struct NAME { bool IsAssign() { return true;} ; T operator()(T & x, T const  & y) { x OP y; return x;} };

DEF_ASSIGN_(ASSIGN,      =);
DEF_ASSIGN(PLUS_ASSIGN, +=);
DEF_ASSIGN(SUB_ASSIGN,  -=);
DEF_ASSIGN(MULT_ASSIGN, *=);
DEF_ASSIGN(DIV_ASSIGN,  /=);
DEF_ASSIGN(MOD_ASSIGN,  %=);
DEF_ASSIGN(BIT_OR_ASSIGN, |=);
DEF_ASSIGN(BIT_XOR_ASSIGN, ^=);
DEF_ASSIGN(BIT_AND_ASSIGN, &=);
DEF_ASSIGN(SHL_ASSIGN,  <<=);
DEF_ASSIGN(SHR_ASSIGN,  >>=);


#define OVERLOAD_ASSIGN(D, E, ASS_FUNC, OP) \
	        inline D & operator OP (D const  & other)  \
	        {  \
                return Array_Assign<ASS_FUNC<T> >()(*CAST_THIS(D), other); \
	        } \
            template<typename S, typename E1> \
	        inline D & operator OP (Array<S,E1>  const & other)  \
	        {  \
                return Array_Assign<ASS_FUNC<T> >()(*CAST_THIS(D), other); \
	        } \
            template<typename S>\
            inline D & operator OP (S const  & val)\
            { \
                return Array_Assign<ASS_FUNC<T> >()(*CAST_THIS(D), asarray(val)); \
            }

#define OVERLOAD_MASSIGN(D, E, ASS_FUNC, OP) \
            template<typename S, typename M> \
            inline D & operator OP(Array<S,M>  const & other) \
            { \
                static_cast<Array<T,E> &>(*this) OP other; \
                return *CAST_THIS(D);\
            }\
            inline D & operator OP (D const  & other)  \
	        {  \
                return Matrix_Assign<ASS_FUNC<T> >()(*CAST_THIS(D), other); \
	        } \
            template<typename S, typename E1> \
	        inline D & operator OP (Matrix<S,E1>  const & other)  \
	        {  \
                return Matrix_Assign<ASS_FUNC<T> >()(*CAST_THIS(D), other); \
	        }\
	        inline D & operator OP (T const  & val)  \
	        {  \
                static_cast<Array<T,E> &>(*this) OP val; \
                return *CAST_THIS(D);\
	        }


template<typename D, typename T, typename E>
struct Array_Base: LRValue, PointerQuery<D, T>, EXPOSE_NDARRAY<D,E>
{
    typedef Array_Base TSelf;

    Array_Base(void):ptr_expr(NULL) {}
	explicit Array_Base(E const  & _expr): expr(_expr), ptr_expr(LVALUE_PTR(E,_expr)) {}
    explicit Array_Base(Integer _len): expr(new T[_len], _len) , ptr_expr(NULL) {}
    Array_Base(Integer _len, T _val): expr(new T[_len], _len) , ptr_expr(NULL) {std::fill(expr.get(), expr.get()+_len, _val);}
    Array_Base(Integer _len, T * ptr): expr(ptr, _len), ptr_expr(NULL) {}
    
    Array_Base(Integer _len, T (*rndFunc)(void) ): expr(new T[_len], _len, rndFunc), ptr_expr(NULL) {}
    template<ENUM_RAND R>
    Array_Base(Integer _len, random<R>  const & rndFunc): expr(new T[_len], _len, rndFunc), ptr_expr(NULL) {}
    template<unsigned n>
    Array_Base(T (&arr)[n]): expr(arr), ptr_expr(NULL) {}

    typedef typename E::const_iterator const_iterator;
    const_iterator begin(void) const { return static_cast<E const  &>(expr).begin();}
    const_iterator end(void) const { return static_cast<E const  &>(expr).end();}

    T & operator[](Integer idx) {return expr[idx];}
    T const operator[](Integer idx) const {return expr[idx];}
    
    template<typename E1>
    Array<T, INDEX<T, E, E1> >operator[](Array<Integer,E1>  const & Idx)
    {
        return Array<T, INDEX<T, E, E1> >(INDEX<T, E, E1>(expr, Idx.expr));
    }
    template<typename E1>
    Array<T, INDEX<T, E, E1> > const operator[](Array<Integer,E1>  const & Idx) const
    {
         return Array<T, INDEX<T, E, E1> >(INDEX<T, E, E1>(CONST_CAST_OBJ(E,expr), Idx.expr));
    }

    template<typename E1>
    Array<T, INDEX<T, E, MASK2ARRAY<E1> > >operator[](Array<bool,E1>  const & Idx)
    {
        return (*this)[Array<Integer, MASK2ARRAY<E1> >(MASK2ARRAY<E1>(Idx.expr))];
    }

    template<typename E1>
    Array<T, INDEX<T, E, MASK2ARRAY<E1> > > const operator[](Array<bool,E1>  const & Idx) const
    {
        return (*this)[Array<Integer, MASK2ARRAY<E1> >(MASK2ARRAY<E1>(Idx.expr))];
    }

    //**** ASSIGNMENT
    OVERLOAD_ASSIGN(D, E, PLUS_ASSIGN, +=);
    OVERLOAD_ASSIGN(D, E, SUB_ASSIGN,  -=);
    OVERLOAD_ASSIGN(D, E, MULT_ASSIGN, *=);
    OVERLOAD_ASSIGN(D, E, DIV_ASSIGN,  /=);
    OVERLOAD_ASSIGN(D, E, MOD_ASSIGN,  %=);
    OVERLOAD_ASSIGN(D, E, BIT_OR_ASSIGN,   |=);
    OVERLOAD_ASSIGN(D, E, BIT_AND_ASSIGN,  &=);
    OVERLOAD_ASSIGN(D, E, BIT_XOR_ASSIGN,  ^=);
    OVERLOAD_ASSIGN(D, E, SHL_ASSIGN,  <<=);
    OVERLOAD_ASSIGN(D, E, SHR_ASSIGN,  >>=);

    //**** item
    Array<T, INDEX<T,E,_Range> > item(Integer idx)
    {
        return Array<T, INDEX<T,E,_Range> >(INDEX<T,E,_Range>(expr, INT_RANGE(idx)));
    }

    Array<T, INDEX<T,E,_Range> >  const item(Integer idx) const
    {
        return Array<T, INDEX<T,E,_Range> >(INDEX<T,E,_Range>(expr, INT_RANGE(idx)));
    }

    template<typename E1>
    Array<T, INDEX<T, E, E1> >item(Array<Integer,E1>  const & Idx)
    {
         return (*this)[Idx];
    }

    template<typename E1>
    Array<T, INDEX<T, E, E1> > const item(Array<Integer,E1>  const & Idx) const
    {
         return (*this)[Idx];
    }

    template<typename E1>
    Array<T, INDEX<T, E, MASK2ARRAY<E1> > >item(Array<bool,E1>  const & Idx)
    {
        return (*this)[Idx];
    }

    template<typename E1>
    Array<T, INDEX<T, E, MASK2ARRAY<E1> > > const item(Array<bool,E1>  const & Idx) const
    {
        return (*this)[Idx];
    }

    Matrix<T,E> reshape(Integer _nrows, Integer _ncols) const
    {
        return Matrix<T,E>(_nrows, _ncols, expr);
    }

    size_t ref_count(void *ptr) const { return expr.ref_count(ptr);}
    size_t ref_count_node(void * ptr) const { return expr.ref_count_node(ptr);}
    size_t ref_count_cum(void * ptr) const { return expr.ref_count_cum(ptr);}

    void sort(bool reverse=false)
    {
        sort(Identity<T>(), reverse);
    }

    template<typename K>
    void sort(K const  & key, bool reverse=false)
    {
        typename remove_cv<E>::type * ptr = static_cast<typename remove_cv<E>::type *>(&expr);
        typename remove_cv<E>::type::iterator itbeg = ptr->begin();
        typename remove_cv<E>::type::iterator itend = ptr->end();
        std::sort(itbeg, itend, KeyComp<K>(key,reverse));
    }


    template<typename T1>
    Array<T1, UnaryOp<T1,E,_view<T1,T> > > view(void) const
    {
        return  Array<T1, UnaryOp<T1,E,_view<T1,T> > >(
                          UnaryOp<T1,E,_view<T1,T> > (expr, _view<T1,T>()));
    }
    
    Matrix<T,E> asRowMat(void) const
    {
        return Matrix<T,E>(1, expr.size(), expr);
    }

    Matrix<T,E> asColMat(void) const
    {
        return Matrix<T,E>(expr.size(), 1, expr);
    }

	Integer size(void) const {return expr.size();}
    Integer rank(void) const {return expr.rank();}
    Shape shape(void) const { return expr.shape();}

    //** internal data structure
    E * ptr_expr;
	E expr;
};

template<typename ASS_FUNC>
struct Array_Assign;

template<typename ASS_FUNC>
struct Matrix_Assign
{
    Matrix_Assign(void): func(ASS_FUNC()) {}
    
    template<typename T, typename E, typename S, typename E1>
    Matrix<T,E> & operator ()(Matrix<T,E> & mat, Matrix<S,E1>  const & other)
    {
        Array_Assign<ASS_FUNC>()(mat.asarray(), other.asarray());
        if(other.size() > 0)
        {
            mat.nrows = other.nrows;
            mat.ncols = other.ncols;
        }

        return mat;
    }

    ASS_FUNC func;
};


template<typename T, typename E >
struct Array: Array_Base<Array<T,E>, T, E>
{
    typedef Array TSelf;
    typedef Array_Base<Array<T,E>, T, E> TBase;

    EXPOSE_CONST_ITER(TBase);

    Array(Array const & arr): Array_Base<Array<T,E>, T, E>(arr.expr) {}
    Array(E const  & _expr): Array_Base<Array<T,E>, T, E>(_expr) {}

    OVERLOAD_ASSIGN(TSelf, E, ASSIGN,      =);
};


template<typename T>
struct Array<T,_Array<T> >
    : Array_Base<Array<T,_Array<T> >, T, _Array<T> >, 
      DefBothIter<Array<T,_Array<T> >, _Array<T> >
{
    typedef Array TSelf;
    typedef _Array<T> E;
    typedef Array_Base<Array<T,E>,T,E> TBase;
    typedef DefBothIter<Array<T,_Array<T> >, _Array<T> > TIter;
    EXPOSE_ITER(TIter);

    Array(void): TBase(0) {SET_NAMED_LVALUE;}
    template<typename S, typename E1>
    Array(Array<S,E1>  const & other, LRValue const  & lrvalue): TBase(other.size())
    {
        SET_LRVALUE(lrvalue);
        std::copy(other.begin(), other.end(), this->expr.get());
    }
    template<typename S, typename E1>
    Array(Array<S,E1>  const & other): TBase(other.size())
    {
        SET_NAMED_LVALUE;
        std::copy(other.begin(), other.end(), this->expr.get());
    }
    Array(E const   & _expr): TBase(_expr) { SET_NAMED_LVALUE; }
    Array(Integer _len, LRValue const  & lrvalue): TBase(_len,T()) {SET_LRVALUE(lrvalue);}
	Array(Integer _len, T const  & _val=T()): TBase(_len,_val) { SET_NAMED_LVALUE; }
    Array(Integer _len, T (*rndFunc)(void)): TBase(_len, rndFunc) { SET_NAMED_LVALUE; }
    template<ENUM_RAND R>
    Array(Integer _len, random<R>  const & rndFunc): TBase(_len, rndFunc) { SET_NAMED_LVALUE; }
    template<unsigned n>
    Array(T (&arr)[n]): TBase(n, new T[n]) { SET_NAMED_LVALUE; std::copy(arr, arr+n, this->expr.get());}
    Array(Integer _len, T * ptr): TBase(_len, ptr) { SET_NAMED_LVALUE; }


    void fill(T x) { std::fill(begin(), end(), x); }
    
    OVERLOAD_ASSIGN(TSelf, E, ASSIGN,      =);
};


#ifdef __GNUC__

#else
    #pragma warning(pop)
#endif


template<typename T>
struct Array<T, _SArray<T> >: LRValue, NullPointerQuery<T>
{
    Array(T const  & _val): expr(_val) {}
    T const  & operator[](Integer idx) const {return expr[idx];}
    Integer size(void) const {return expr.size();}

    typedef typename _SArray<T>::const_iterator const_iterator;
    const_iterator begin(void) const { return expr.begin();}
    const_iterator end(void) const { return expr.end();}

    size_t ref_count(void *ptr) const { return 0;}
    size_t ref_count_node(void * ptr) const { return 0;}
    size_t ref_count_cum(void * ptr) const { return 0;}
 
    //** internal data structure
    _SArray<T> expr;
};


typedef Array<Integer, _Range> Range;

//**** Wrap Range object to handle range-based indexing for ndarray

template<int n, class T>
struct WrapRange 
{
    WrapRange(T v): obj(v) {}

    T operator() () const {return obj;}

    template<class T1>
    T operator() (T1) const {return obj;}

    template<class T1,class T2>
    T operator() (T1,T2) const {return obj;}

    Integer size() const {return 0;}

    T obj;
};

template<>
struct WrapRange<0, Range>
{
    WrapRange(Range const & r): obj(r) {}

    Integer operator() (Integer a0) const { return obj[a0];}
    Integer operator() (Integer a0, Integer a1) const { return obj[a0]; }

    Integer size() const {return obj.size();}

    Range const obj;
};


template<>
struct WrapRange<1, Range>
{
    WrapRange(Range const & r): obj(r) {}

    Integer operator() (Integer a0, Integer a1) const { return obj[a1]; }
    
    Integer size() const {return obj.size();}

    Range const obj;
};

//-----------------------------------------

template<class T, class R, int n=0> struct ClassCount;

template<class T1, class T2, class R, int n>
struct ClassCount<_pair<T1,T2>,R,n>
{
    enum {  count = is_same<T1,R>::value + ClassCount<T2,R,0>::count,
            _index = ClassCount<T2,R,n-1>::_index, 
            _sum = _index + ClassCount<_pair<T1,T2>,R,n-1>::_sum,
            index = _index*_sum - 1};
};

template<class T1, class T2, class R>
struct ClassCount<_pair<T1,T2>,R,0>
{
    enum {  count = is_same<T1,R>::value + ClassCount<T2,R,0>::count,
            _index = is_same<T1,R>::value,
            _sum = _index, 
            index = _index*_sum - 1};
};

template<class R, int n>
struct ClassCount<tNULL,R,n>
{
    enum {  count = 0,
            _index = 0,
            _sum = 0, 
            index = _index*_sum - 1};
    typedef tNULL type;
};


template<class T, class R, int k> struct ClassIndex;

template<class T1, class T2, class R, int k>
struct ClassIndex<_pair<T1,T2>,R,k>
{
    enum { index = is_same<T1,R>::value ? 
                        ( (k==0)? 0 : 1 + ClassIndex<T2,R,k-1>::index ) : 
                        1+ClassIndex<T2,R,k>::index };
};

template<class R, int k>
struct ClassIndex<tNULL,R,k>
{
    enum { index = -1 };
};


template<class T, int n> struct IdxDecompose;

template<class T> 
struct IdxDecompose<T,2>
{
    IdxDecompose(T ii, T (&prod)[2], T (&idx)[2])
    {
        idx[0]=ii;
        divmod(idx[0],prod[0],idx[0],idx[1]);
    }
};

template<class T> 
struct IdxDecompose<T,3>
{
    IdxDecompose(T ii, T (&prod)[3], T (&idx)[3])
    {
        idx[0]=ii;
        divmod(idx[0],prod[0],idx[0],idx[1]);
        divmod(idx[1],prod[1],idx[1],idx[2])
    }
};


template<class D, class T> struct NDINDEX_BaseBase
{
    typedef T value_type;
    typedef Const_Iterator<T, D> const_iterator;
    typedef Iterator<T,D> iterator;

    const_iterator begin(void) const { return const_iterator(*CAST_THIS(D const ), 0);}
    const_iterator end(void) const { return const_iterator(*CAST_THIS(D const ), size());}

    iterator begin(void) { return iterator(*CAST_THIS(D), 0); }
    iterator end(void) { return iterator(*CAST_THIS(D), size()); }

    T & operator [](Integer idx) { return expr[idx];}
    const T  operator [](Integer idx) const { return expr[idx];}
    Integer size(void) const {return len[n-1]*prod[n-1];}

    template<int k>
    void nzRange(Integer idx, Integer & start, Integer & stop) const { start=0; stop=len[k];}

    Integer rank(void) const { return sizeof(CAST_THIS(D)->len)/sizeof(CAST_THIS(D)->len[0]);}
    Shape shape(void) const { return Shape(&(CAST_THIS(D)->len[0]), rank());}

};

template<class D, class T, class E, int n, int m> struct NDINDEX_Base;

template<class D, class T, class E>
struct NDINDEX_Base<D,T,E,2,0>: NDINDEX_BaseBase<D,Integer>
{
    T &operator() ()
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(),CAST_THIS(D)->a1()); 
    }

    T operator() () const
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(),CAST_THIS(D)->a1()); 
    }

    Integer rank(void) const { return 0;}
};

template<class D, class T, class E>
struct NDINDEX_Base<D,T,E,2,1>: NDINDEX_BaseBase<D,Integer>
{
    void reset()
    {
        len[0]=CAST_THIS(D)->args.get<ClassIndex<typename D::TplBase, Range, 0>::index>().size();
    }

    T & operator() (Integer i0)
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(i0),CAST_THIS(D)->a1(i0)); 
    }

    T operator() (Integer i0) const
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(i0),CAST_THIS(D)->a1(i0)); 
    }
    
    ndarray<1, T, D> 
    create_ndarray()
    {
        return ndarray<1, T, D>(*CAST_THIS(D), len[0]);
    }
    
    T & operator[] (Integer idx) { return operator()(idx);}

    T operator[] (Integer idx) const { return operator()(idx);}

    Integer rank(void) const { return 1;}
    Shape shape(void) const { return Shape(&len[0],0);}

    Integer len[1];
};

template<class T>
void divmod(T num, T denom, T & quot, T & rem)
{
    quot = num/denom;
    rem = num % denom;
}

template<class D, class T, class E>
struct NDINDEX_Base<D,T,E,2,2>: NDINDEX_BaseBase<D,Integer>
{
    void reset()
    {
        len[0]=CAST_THIS(D)->args<ClassIndex<typename D::TplBase, Range, 0>::index>().size();
        len[1]=CAST_THIS(D)->args<ClassIndex<typename D::TplBase, Range, 1>::index>().size();

        prod[0]=1;
        for(int ii=1; ii < 2; ++ii)
            prod[ii] = prod[ii-1]*len[ii-1];
    }

    T & operator() (Integer i0, Integer i1)
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(i0,i1),CAST_THIS(D)->a1(i0,i1)); 
    }

    T operator() (Integer i0, Integer i1) const
    {
        return CAST_THIS(D)->expr(CAST_THIS(D)->a0(i0,i1),CAST_THIS(D)->a1(i0,i1)); 
    }
    
    T & operator[] (Integer idx) { IdxDecompose<Integer,2>(idx, prod, idx); return operator()(idx[0],idx[1]);}

    T operator[] (Integer idx) const { IdxDecompose<Integer,2>(idx, prod, idx); return operator()(idx[0],idx[1]);}

    ndarray<2, T, D>
    create_ndarray()
    {
        return ndarray<2, T, D> (*CAST_THIS(D), len[0], len[1])
    }

    Integer len[2];
    Integer prod[2];
    Integer idx[2];
};

template<int n, class T, class E, class T0, class T1> struct NDINDEX;

template<class T, class E, class T0, class T1>
struct NDINDEX<2,T,E,T0,T1>
    : NDINDEX_Base<NDINDEX<2,T,E,T0,T1>, T, E, 2, ClassCount<typename tuple<T0,T1>::TBase,Range >::count>
{
    typedef typename tuple<T0,T1>::TBase TplBase;
    typedef Range TArrRange;
    typedef T value_type;
    typedef E expr_type;

    typedef NDINDEX_Base<NDINDEX<2,T,E,T0,T1>, T, E, 2, ClassCount<typename tuple<T0,T1>::TBase,Range >::count> TBase;
    typedef ClassCount<TplBase,TArrRange> TClassCount;
    
    NDINDEX(E const &_expr, T0 _a0, T1 _a1): expr(_expr), a0(_a0), a1(_a1), args(a0,a1) { TBase::reset(); }

    typedef WrapRange<ClassCount<TplBase, Range, 0>::index, T0> TWrapRange0;
    typedef WrapRange<ClassCount<TplBase, Range, 1>::index, T1> TWrapRange1;
    
    TWrapRange0 a0;
    TWrapRange1 a1;

    tuples::tuple<TWrapRange0 &, TWrapRange1 &> args;
    
    E expr;
};


template<class E, class T0, class T1>
ndarray< ClassCount<typename tuples::tuple<T0,T1>::TBase, Range>::count, 
         typename E::value_type, NDINDEX<2,typename E::value_type,E,T0,T1> >  
    make_ndarray(E const & arr, T0 a0, T1 a1)
{
    return NDINDEX<2,typename E::value_type,E,T0,T1>(arr,a0,a1).create_ndarray();
}


template<typename T, typename E>
std::ostream & operator << (std::ostream & os, Array<T,E>  const & arr)
{
    char const  * delimPtr="[";
    os << delimPtr; delimPtr="";
    for(typename Array<T,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)
	{
        T val = *it;

		os << delimPtr;// << std::left << std::setw(10) << val;
        StreamOut(os, val);
        delimPtr = ", ";
	}
	os << "]";

	return os;
}

template<typename T>
inline std::ostream & StreamOut(std::ostream & os, T const  val)
{
    return os << std::left << std::setw(DEFAULT_WIDTH) << val;
}

template<typename T>
inline std::ostream & StreamOut(std::ostream & os, std::complex<T>  const & val)
{
    if(val.real()==T() && val.imag()==T())
        return StreamOut(os, T());
    else
        return os << std::left << std::setw(DEFAULT_WIDTH) << val;
}


template<>
inline std::ostream & StreamOut(std::ostream & os, bool const  val)
{
    return os << std::boolalpha << val;
}

template<>
inline std::ostream & StreamOut(std::ostream & os, Integer const  val)
{
    return os << val;
}

template<typename T1, typename T2>
inline std::ostream & StreamOut(std::ostream & os, tuple<T1,T2>  const & obj)
{
    return os << "(" << obj.get<0>() << "," << obj.get<1>() << ")";
}

#define OP_Overload(FUNC, OPFUNC)\
        template<typename T, typename E1, typename E2>\
        inline Array<T, BinOp<T,E1,E2,FUNC<T> > > OPFUNC (Array<T,E1>  const & expr1, Array<T,E2>  const & expr2) \
        { \
            return Array<T, BinOp<T,E1,E2,FUNC<T> > > (\
                        BinOp<T,E1,E2,FUNC<T> >(expr1.expr, expr2.expr, FUNC<T>()));\
        }\
        \
        template<typename T, typename E1>\
        inline Array<T, BinOp<T,E1,_SArray<T>,FUNC<T> > > OPFUNC (Array<T,E1>  const & expr1, T const  & expr2) \
        {   \
            return OPFUNC(expr1 , Array<T, _SArray<T> >(expr2));\
        }\
        \
        template<typename T, typename E2>\
        inline Array<T, BinOp<T,_SArray<T>,E2,FUNC<T> > > OPFUNC (T const  & expr1, Array<T,E2>  const & expr2) \
        { \
            return OPFUNC(Array<T, _SArray<T> >(expr1) , expr2);\
        }

#define MOP_Overload(FUNC,OPFUNC)\
        template<typename T, typename E1, typename E2>\
        inline Matrix<T, MBinOp<T,Matrix<T,E1>,Matrix<T,E2>,FUNC<T> > > OPFUNC( \
                        Matrix<T,E1>  const & expr1, Matrix<T,E2>  const & expr2) \
        { \
            return Matrix<T, MBinOp<T,Matrix<T,E1>,Matrix<T,E2>,FUNC<T> > >(\
                        MBinOp<T,Matrix<T,E1>,Matrix<T,E2>,FUNC<T> >(expr1, expr2, FUNC<T>()));\
        }\
        template<typename T, typename E1>\
        inline Matrix<T, MBinOp<T,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> > >  OPFUNC(\
                        Matrix<T,E1>  const & expr1, T const  & expr2) \
        { \
            return Matrix<T, MBinOp<T,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> > >(\
                        MBinOp<T,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> >(\
                            expr1, Matrix<T,_SArray<T> >(expr2), FUNC<T>()));\
        }\
        template<typename T, typename E2>\
        inline Matrix<T, MBinOp<T,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> > >  OPFUNC(\
                        T const  & expr1, Matrix<T,E2>  const & expr2) \
        { \
            return Matrix<T, MBinOp<T,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> > > (\
                        MBinOp<T,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> >(\
                            Matrix<T,_SArray<T> >(expr1), expr2, FUNC<T>()));\
        }\
        template<typename T, typename E1, typename E2>\
        inline Matrix<T, MBinOp<T,Matrix<T,E1>, Matrix<T,MREPEAT<T,Matrix<T,E2> > >,FUNC<T> > > OPFUNC (\
                Matrix<T,E1>  const & expr1, Array<T,E2>  const & expr2) \
        { \
            return OPFUNC(expr1, repeat(Matrix<T,E2>(1,expr2.size(), expr2.expr), expr1.nrows,0));\
        }\
        template<typename T, typename E1, typename E2>\
        inline Matrix<T, MBinOp<T,Matrix<T,MREPEAT<T,Matrix<T,E1> > >, Matrix<T,E2>, FUNC<T> > >OPFUNC (\
                Array<T,E1>  const & expr1, Matrix<T,E2>  const & expr2) \
        { \
            return OPFUNC(repeat(Matrix<T,E2>(1, expr1.size(), expr1.expr), expr2.nrows,0), expr2);\
        }

#define OP_Overload2(FUNC,OPFUNC)  OP_Overload(FUNC,OPFUNC); MOP_Overload(FUNC,OPFUNC)

OP_Overload2(std::plus,  operator +);  
OP_Overload2(std::minus,  operator -); 
OP_Overload2(std::multiplies, operator *);
OP_Overload2(std::divides,  operator /);
OP_Overload2(std::modulus,  operator %); 

template<typename T>
struct shift_left: std::binary_function<T,T,T> { T operator()(T const  & expr1, T const  & expr2) const { return expr1 << expr2;}};
template<typename T>
struct shift_right: std::binary_function<T,T,T> { T operator()(T const  & expr1, T const  & expr2) const { return expr1 >> expr2;}};
template<typename T>
struct bit_and: std::binary_function<T,T,T> { T operator()(T const  & expr1, T const  & expr2) const { return expr1 & expr2;}};
template<typename T>
struct bit_or: std::binary_function<T,T,T> { T operator()(T const  & expr1, T const  & expr2) const { return expr1 | expr2;}};
template<typename T>
struct bit_xor: std::binary_function<T,T,T> { T operator()(T const  & expr1, T const  & expr2) const { return expr1 ^ expr2;}};

OP_Overload2(shift_left,  operator <<);
OP_Overload2(shift_right, operator >>);
OP_Overload2(bit_and,  operator &);
OP_Overload2(bit_or,   operator |);
OP_Overload2(bit_xor,  operator ^);


#define boolMOP_Overload(FUNC, OPFUNC)\
        template<typename T, typename E1, typename E2>\
        inline Matrix<bool, MBinOp<bool,Matrix<T,E1>,Matrix<T,E2>,FUNC<T> > >  OPFUNC (\
                Matrix<T,E1>  const & expr1, Matrix<T,E2>  const & expr2) \
        { \
            return Matrix<bool, MBinOp<bool,Matrix<T,E1>,Matrix<T,E2>,FUNC<T>  > >(\
                                MBinOp<bool,Matrix<T,E1>,Matrix<T,E2>,FUNC<T>  >(\
                                    expr1, expr2, FUNC<T>()));\
        }\
        template<typename T, typename E1>\
        inline Matrix<bool, MBinOp<bool,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> > >  OPFUNC (\
                Matrix<T,E1>  const & expr1, T const  & expr2) \
        { \
            return Matrix<bool, MBinOp<bool,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> > >(\
                                MBinOp<bool,Matrix<T,E1>,Matrix<T,_SArray<T> >,FUNC<T> >(\
                                    expr1, Matrix<T,_SArray<T> >(expr2), FUNC<T>()));\
        }\
        template<typename T, typename E2>\
        inline Matrix<bool, MBinOp<bool,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> > >  OPFUNC (\
                T const  & expr1, Matrix<T,E2>  const & expr2) \
        { \
            return Matrix<bool, MBinOp<bool,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> > >(\
                                MBinOp<bool,Matrix<T,_SArray<T> >,Matrix<T,E2>,FUNC<T> >(\
                                    Matrix<T,_SArray<T> >(expr1), expr2, FUNC<T>()));\
        }

#define boolOP_Overload(FUNC, OPFUNC)\
    template<typename T, typename E1, typename E2>\
        inline Array<bool, BinOp<bool,E1,E2,FUNC<T> > > OPFUNC (Array<T,E1>  const & expr1, Array<T,E2>  const & expr2) \
        { \
            return Array<bool, BinOp<bool,E1,E2,FUNC<T> > > (\
                        BinOp<bool,E1,E2,FUNC<T> >(expr1.expr, expr2.expr, FUNC<T>()));\
        }\
        \
        template<typename T, typename E1>\
        inline Array<bool, BinOp<bool,E1,_SArray<T>,FUNC<T> > > OPFUNC (Array<T,E1>  const & expr1, T const  & expr2) \
        {   \
            return OPFUNC(expr1, Array<T, _SArray<T> >(expr2));\
        }\
        \
        template<typename T, typename E2>\
        inline Array<bool, BinOp<bool,_SArray<T>,E2,FUNC<T> > > OPFUNC (T const  & expr1, Array<T,E2>  const & expr2) \
        { \
            return OPFUNC(Array<T, _SArray<T> >(expr1) , expr2);\
        }\
        boolMOP_Overload(FUNC, OPFUNC);

boolOP_Overload(std::less,   operator <  );
boolOP_Overload(std::less_equal,  operator <= );
boolOP_Overload(std::greater,   operator >  );
boolOP_Overload(std::greater_equal,  operator >= );
boolOP_Overload(std::equal_to,   operator == );
boolOP_Overload(std::not_equal_to,   operator != );


template<typename T, typename E, typename F>
struct UnaryOp: UnaryOpBase<UnaryOp<T,E,F>,E,T>
{
    typedef UnaryOpBase<UnaryOp<T,E,F>,E,T> TBase;

    UnaryOp(E const  & _expr, F const  & _func): TBase(_expr), func(_func) {}

    T const  operator[](Integer idx) const { return func(this->expr[idx]);}

    F const  func;
};

#define MUOP_Overload(FUNC, OPFUNC, R)\
    template<typename T, typename E1>\
    inline Matrix<R, MUnaryOp<R,Matrix<T,E1>,FUNC<T> > > OPFUNC (Matrix<T,E1>  const & expr) \
    { \
        return Matrix<R, MUnaryOp<R,Matrix<T,E1>,FUNC<T> > >(\
                    expr.nrows, expr.ncols, MUnaryOp<R,Matrix<T,E1>,FUNC<T> >(expr, FUNC<T>()));\
    }

#define UOP_Overload(FUNC, OPFUNC, R)\
    template<typename T, typename E1>\
    inline Array<R, UnaryOp<R,E1,FUNC<T> > > OPFUNC (Array<T,E1>  const & expr) \
    { \
        return  Array<R, UnaryOp<R,E1,FUNC<T> > >(\
                         UnaryOp<R,E1,FUNC<T> >(expr.expr, FUNC<T>()));\
    }\
    MUOP_Overload(FUNC,OPFUNC, R)


template<typename T>
struct bit_not: std::unary_function<T,T> { T operator() (T const  & expr) const { return ~expr;}};
template<>
struct bit_not<bool>: std::unary_function<bool,bool> { bool operator() (bool const  & expr) const { return !expr;}};

UOP_Overload(bit_not, operator ~, T);
UOP_Overload(std::negate, operator -, T);

#define MERGE_TWO(x,y) x ## y
#define UFUNC_Overload(FUNC,NAME,R)\
    template<typename T>\
    struct MERGE_TWO(_,NAME) { R operator () ( T const  & x) const {return FUNC(x);}};\
    UOP_Overload(MERGE_TWO(_,NAME),NAME,R);

UFUNC_Overload(std::abs, abs, T);  
UFUNC_Overload(std::acos, acos, T); UFUNC_Overload(std::acos, arccos, T);
UFUNC_Overload(std::asin, asin, T); UFUNC_Overload(std::asin, arcsin, T);
UFUNC_Overload(std::atan, atan, T); UFUNC_Overload(std::atan, arctan, T);
UFUNC_Overload(std::ceil, ceil, T);
UFUNC_Overload(std::cos, cos, T);
UFUNC_Overload(std::cosh, cosh, T);
UFUNC_Overload(std::exp, exp, T);
UFUNC_Overload(std::floor, floor, T);
// frexp
// ldexp
UFUNC_Overload(std::log, log, T);
UFUNC_Overload(std::log10, log10, T);
// modf
UFUNC_Overload(std::sin, sin, T);
UFUNC_Overload(std::sinh, sinh, T);
UFUNC_Overload(std::sqrt, sqrt, T);
UFUNC_Overload(std::tan, tan, T);
UFUNC_Overload(std::tanh, tanh, T);
template<typename T>
    inline int trunc(T const  &x) { return int(x); }
UFUNC_Overload(trunc, trunc, T);
template<typename T>
    inline T sign(T const  &x) { return copysign(T(1),x); }
UFUNC_Overload(sign, sign, T);

// real/imag for complex array
template<typename T> struct complex_traits { typedef T TReImag; };
template<typename T> struct complex_traits<std::complex<T> > { typedef T TReImag; };

template<typename T>
    inline T & real_(T &x) {return x;}
template<typename T>
    inline T real_(std::complex<T> const &z) {return z.real();}
UFUNC_Overload(real_, real, typename complex_traits<T>::TReImag );

template<typename T>
    inline T imag_(T const  &x) {return T(0);}
template<typename T>
    inline T imag_(std::complex<T> const &z) {return z.imag();}
UFUNC_Overload(imag_, imag, typename complex_traits<T>::TReImag );

enum ENUM_DEG { RADIAN, DEGREE };
template<typename T>
    inline double angle_rad(T const  &x) {return T(0);}
template<typename T>
    inline double angle_rad(std::complex<T> const &z) {return std::arg(z);}
template<typename T>
    inline double angle_deg(T const  &x) { return 180/M_PI*angle_rad(x);}
UFUNC_Overload(angle_rad, angle, double);
UFUNC_Overload(angle_deg, angle_deg, double);

// view
template<typename T, typename S>
struct _view
{
    T operator()(S const  & x) const { return (T)(x); }
};

template<typename T, typename T1, typename E1>
    inline Array<T, UnaryOp<T,E1,_view<T,T1> > > view (Array<T1,E1>  const & expr) 
    { 
        return expr.view<T>(); /*Array<T, UnaryOp<T,E1,_view<T,T1> > >(
                         UnaryOp<T,E1,_view<T,T1> > (expr.expr, _view<T,T1>()));*/
    }

template<typename T, typename T1, typename E1>
    inline Matrix<T, MUnaryOp<T,Matrix<T1,E1>,_view<T,T1> > > view (Matrix<T1,E1>  const & mat) 
    { 
        return mat.view<T>(); /*Matrix<T, MUnaryOp<T,Matrix<T1,E1>,_view<T,T1> > >(
                    mat.nrows, mat.ncols, MUnaryOp<T,Matrix<T1,E1>,_view<T,T1> >(mat, _view<T,T1>()));*/
    }


/*** clip **/
template<typename T>
    struct _clip
    { 
        _clip(T _min, T _max): a_min(_min), a_max(_max) {};
        T operator () ( T const  & x) const {return std::max(std::min(x, a_max),a_min);}
        T a_min;
        T a_max;
    };
template<typename T, typename E1>
    inline Array<T, UnaryOp<T,E1,_clip<T> > > clip (Array<T,E1>  const & expr, T _min, T _max) 
    { 
        return  Array<T, UnaryOp<T,E1,_clip<T> > >(
                        UnaryOp<T,E1,_clip<T> >(expr.expr, _clip<T>(_min,_max)));
    }
template<typename T, typename E1>
    inline Matrix<T, MUnaryOp<T,Matrix<T,E1>,_clip<T> > > clip (Matrix<T,E1>  const & expr, T _min, T _max)
    { 
        return Matrix<T, MUnaryOp<T,Matrix<T,E1>,_clip<T> > >(
                    expr.nrows, expr.ncols, MUnaryOp<T,Matrix<T,E1>,_clip<T> >(expr, _clip<T>(_min,_max)));
    }    

template<typename T> T clip(T x, T _min, T _max) { return std::max(std::min(x, _min),_max); }

//============= General binary functions
template<typename T> T hypot(T const  & a, T const  & b) { return std::sqrt(a*a + b*b);}

#define BFUNC_Overload(FUNC,NAME)\
    template<typename T>\
    struct MERGE_TWO(_,NAME) { T operator () ( T const  & x, T const  & y) const {return FUNC(x,y);}};\
    OP_Overload2(MERGE_TWO(_,NAME),NAME);

BFUNC_Overload(std::atan2, atan2); BFUNC_Overload(std::atan2, arctan2);
BFUNC_Overload(std::fmod, fmod);
BFUNC_Overload(std::pow, pow);
BFUNC_Overload(std::max, maximum);
BFUNC_Overload(std::min, minimum);
BFUNC_Overload(hypot,    hypot);
/*** round **/
template<typename T> 
inline T round_(T const  & a, int ndigits) 
{ 
    T p = std::pow(10.0, ndigits);
    return std::floor(a*p+0.5)/p;
}

template<typename T>
    struct _round
    { 
        _round(int _ndigits): ndigits(_ndigits)  {};
        T operator () ( T const  & x) const {return round_(x, ndigits);}
        int ndigits;
    };
template<typename T, typename E1>
    inline Array<T, UnaryOp<T,E1,_round<T> > > round(Array<T,E1>  const & expr, int ndigits) 
    { 
        return  Array<T, UnaryOp<T,E1,_round<T> > >(
                        UnaryOp<T,E1,_round<T> >(expr.expr, _round<T>(ndigits)));
    }
template<typename T, typename E1>
    inline Matrix<T, MUnaryOp<T,Matrix<T,E1>,_round<T> > > round(Matrix<T,E1>  const & expr, int ndigits)
    { 
        return Matrix<T, MUnaryOp<T,Matrix<T,E1>,_round<T> > >(
                    expr.nrows, expr.ncols, MUnaryOp<T,Matrix<T,E1>,_round<T> >(expr, _round<T>(ndigits)));
    }

//***** Slice definition
template<typename T, typename E1>
struct Slice: RefCountCRTP<Slice<T,E1> >, PointerQuery<Slice<T,E1>, T>
{
    typedef T value_type;

    Slice(E1 const  & _expr, Integer _beg, Integer _end, Integer _incr=1): 
        expr(const_cast<E1 &>(_expr)), ptr_expr(NULL), 
        start(_beg), stop(_end), step(_incr) {}

    T & operator[] (Integer idx) { return expr[start+idx*step]; }
    T const  operator[] (Integer idx) const { return expr[start+idx*step]; }

    typedef Const_Iterator<T,Slice> const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, size());}

    typedef Iterator<T,Slice> iterator;
    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, size());}

    Integer size(void) const {return std::max(0, (stop-start+step-1)/step);}
    
    Slice & operator=(Slice const  & other) { start=other.start; stop=other.stop; step=other.step; expr=other.expr; return *this;}

    Integer start, stop, step;
    E1  expr;
    E1 * ptr_expr;
};


template<typename T, typename E>
std::ostream & operator << (std::ostream & os, Slice<T, E> S)
{
    return os << Array<T,Slice<T,E> >(S);
}

template<typename T, typename S>
std::ostream & operator << (std::ostream & os, const tuple<T,S> & p)
{
    return os << "(" << p.get<0>() << "," << p.get<1>() << ")";
}


//************** Index Array

template<typename E>
inline E & do_adjust(E & expr, Integer _len) { return expr;}

template<>
inline const _Range & do_adjust(const _Range & expr, Integer _len) { return const_cast<_Range &>(expr).adjust(_len);}

template<typename T, typename E, typename E1, typename E2>
struct INDEX2: BinOpBase<INDEX2<T,E,E1,E2>,E1,E2,T>, PointerQuery<INDEX2<T,E,E1,E2>,T>
{
    typedef INDEX2 TSelf;
    typedef PointerQuery<TSelf,T> TPtrQuery;
    typedef T value_type;

    typedef Iterator<T, TSelf> iterator;
    typedef Const_Iterator<T, TSelf> const_iterator;

	INDEX2(E & _expr, const E1 & _expr1, const E2 &_expr2): 
        BinOpBase<TSelf,E1,E2,T>(do_adjust(_expr1, _expr.nrows), do_adjust(_expr2, _expr.ncols)),
        expr(_expr), ptr_expr(LVALUE_PTR(E,_expr)), nrows(_expr1.size()), ncols(_expr2.size()) {}

	T & operator ()(Integer ridx, Integer cidx) { return this->expr[this->expr1[ridx]+this->expr.nrows*this->expr2[cidx]]; }
    const T operator ()(Integer ridx, Integer cidx) const { return this->expr[this->expr1[ridx]+this->expr.nrows*this->expr2[cidx]]; }

    T & operator [](Integer idx) { return operator()(idx%nrows, idx/nrows); }
    const T operator [](Integer idx) const { return operator()(idx%nrows, idx/nrows); }

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, size());}

    // Expose the following routines from TPtrQuery and hide them from BinOpBase
    EXPOSE_PTR_QUERY(TPtrQuery);

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, size());}

    Integer size(void) const {return nrows*ncols;}

    E  expr;
    E * ptr_expr;
    Integer nrows;
    Integer ncols;
};


template<typename T, typename E>
struct MREPEAT: RefCountCRTP<MREPEAT<T,E> >, MPointerQuery<MREPEAT<T,E>,T>
{
    typedef MREPEAT TSelf;
    typedef T value_type;
	typedef Iterator<T, TSelf> iterator;
    typedef Const_Iterator<T, TSelf> const_iterator;
    
    MREPEAT(const E & _expr, Integer _repeat, Integer _axis)
        : expr(_expr), repeat(_repeat), rrepeat(1), crepeat(1), ptr_expr(LVALUE_PTR(E,_expr))
    {
        if(_axis==0)
            rrepeat = repeat;
        else
            crepeat = repeat;
        nrows = expr.nrows * rrepeat;
        ncols = expr.ncols * crepeat;
    }   

    T & operator [](Integer idx) { return operator()(idx%nrows, idx/nrows); }
    const T operator [](Integer idx) const { return operator()(idx%nrows, idx/nrows); }

    T & operator ()(Integer ridx, Integer cidx) { return expr[cidx/crepeat+expr.nrows*(ridx/rrepeat)]; }
    const T operator ()(Integer ridx, Integer cidx) const { return expr[cidx/crepeat+expr.nrows*(ridx/rrepeat)]; }

    void nzCols(Integer row, Integer & start, Integer & stop) const 
    { 
        expr.nzCols(row/rrepeat, start, stop);
        start *= crepeat;
        stop *= crepeat;
    }
    void nzRows(Integer col, Integer & start, Integer & stop) const 
    { 
        expr.nzRows(col/crepeat, start, stop);
        start *= rrepeat;
        stop *= rrepeat;
    }

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, size());}

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, size());}

    Integer size(void) const { return expr.size()*repeat; }

    E  expr;
    E * ptr_expr;
    Integer repeat, rrepeat, crepeat;
    Integer nrows;
    Integer ncols;
};

//**** Matrix definition
template<typename T, typename E1>
    inline Matrix<T,E1> & toMatrix(Matrix<T,E1> & obj) { return obj;}
template<typename T, typename E1>
    inline Matrix<T,E1> toMatrix(Array<T,E1> & obj) { return Matrix<T,E1>(obj);}
template<typename T>
    inline Matrix<T,_SArray<T> > toMatrix(T & val) { return Matrix<T,_SArray<T> >(val);}
template<typename T, typename E1>
    inline const Matrix<T,E1> & toMatrix(const Matrix<T,E1> & obj) { return obj;}
template<typename T, typename E1>
    inline const Matrix<T,E1> toMatrix(const Array<T,E1> & obj) { return Matrix<T,E1>(obj);}
template<typename T>
    inline const Matrix<T,_SArray<T> > toMatrix(const T & val) { return Matrix<T,_SArray<T> >(val);}

template<typename D, typename T, typename E>
struct Matrix_Base: Array<T, E>, MPointerQuery<Matrix_Base<D,T,E>,T>
{
    typedef Matrix_Base TSelf;
    typedef Array<T,E> TBase;
    typedef E  TExpr;
    typedef MPointerQuery<Matrix_Base,T> TPtrQuery;

    Matrix_Base(Integer _nrows, Integer _ncols): nrows(_nrows), ncols(_ncols), TBase(_nrows*_ncols) {}
    Matrix_Base(Integer _nrows, Integer _ncols, const Array<T,E> & _expr): nrows(_nrows), ncols(_ncols), TBase(_expr) {}
    Matrix_Base(const Array<T,E> & _expr): TBase(_expr), nrows(TBase::size()), ncols(1) {}
    Matrix_Base(const Matrix<T,E> & _expr): TBase(_expr), nrows(_expr.nrows), ncols(_expr.ncols) {}
    Matrix_Base(Integer _nrows, Integer _ncols, const E & _expr): nrows(_nrows), ncols(_ncols), TBase(_expr) {}
    Matrix_Base(Integer _nrows, Integer _ncols, const T & _val): nrows(_nrows), ncols(_ncols), TBase(_nrows*_ncols, _val) {}
    Matrix_Base(const T & _val): nrows(0), ncols(0), TBase(_val) {}

    template<typename S, typename E1>
    Matrix_Base(const Matrix<S,E1>& mat): TBase(mat), nrows(mat.nrows), ncols(mat.ncols) {}
    
    Matrix_Base(const E & _expr)
        : TBase(_expr), nrows(_expr.nrows), ncols(_expr.ncols) {}

    Matrix_Base(Integer _nrows, Integer _ncols, T (*rndFunc)(void) )
        : nrows(_nrows), ncols(_ncols), TBase(_nrows*_ncols, rndFunc) {}

    template<ENUM_RAND R>
    Matrix_Base(Integer _nrows, Integer _ncols, const random<R> & rndFunc)
        : nrows(_nrows),ncols(_ncols),TBase(_nrows*_ncols, rndFunc) {}

    Matrix_Base(Integer _nrows, Integer _ncols, T *ptr)
        : nrows(_nrows), ncols(_ncols), TBase(_nrows*_ncols, ptr) {}
    
    OVERLOAD_MASSIGN(D, TExpr, PLUS_ASSIGN, +=);
    OVERLOAD_MASSIGN(D, TExpr, SUB_ASSIGN,  -=);
    OVERLOAD_MASSIGN(D, TExpr, MULT_ASSIGN, *=);
    OVERLOAD_MASSIGN(D, TExpr, DIV_ASSIGN,  /=);
    OVERLOAD_MASSIGN(D, TExpr, MOD_ASSIGN,  %=);
    OVERLOAD_MASSIGN(D, TExpr, BIT_OR_ASSIGN,   |=);
    OVERLOAD_MASSIGN(D, TExpr, BIT_AND_ASSIGN,  &=);
    OVERLOAD_MASSIGN(D, TExpr, BIT_XOR_ASSIGN,  ^=);
    OVERLOAD_MASSIGN(D, TExpr, SHL_ASSIGN,  <<=);
    OVERLOAD_MASSIGN(D, TExpr, SHR_ASSIGN,  >>=);

    T  & operator() (Integer ridx, Integer cidx) { return this->expr[nrows*cidx + ridx];}
    const T operator() (Integer ridx, Integer cidx) const { return this->expr[nrows*cidx + ridx];}

    T  & operator() (const tuple<Integer,Integer> & Idx) { return this->expr[nrows*Idx.get<1>() + Idx.get<0>()];}
    const T operator() (const tuple<Integer,Integer> & Idx) const { return this->expr[nrows*Idx.get<1>() + Idx.get<0>()];}

    using Array<T,E>::operator [];
    T  & operator[] (const tuple<Integer,Integer> & Idx) { return this->expr[nrows*Idx.get<1>() + Idx.get<0>()];}
    const T operator[] (const tuple<Integer,Integer> & Idx) const { return this->expr[nrows*Idx.get<1>() + Idx.get<0>()];}

    template<typename T1, typename E1, typename T2, typename E2>
    Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > > operator[] (const Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> > & z)
    {
        return Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > >(
                        INDEX<T, D, ZIP<T1,E1,T2,E2> >(static_cast<D &>(*this), z.expr));
    }
    template<typename T1, typename E1, typename T2, typename E2>
    const Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > > operator[] (const Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> > & z) const
    {
        return Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > >(
                        INDEX<T, D, ZIP<T1,E1,T2,E2> >(static_cast<const D &>(*this), z.expr));
    }

    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, E1, E2> > operator() (const Array<Integer, E1> & Idx1, const Array<Integer, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, E1, E2> >(
                    INDEX2<T, D, E1, E2>(*CAST_THIS(D), Idx1.expr, Idx2.expr) );
    }

    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, E1, E2> > operator() (const Array<Integer, E1> & Idx1, const Array<Integer, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, E1, E2> >(
                    INDEX2<T, D, E1, E2>(CAST_OBJ(D, *CONST_CAST_PTR(Matrix_Base,this)), Idx1.expr, Idx2.expr) );
    }
    
    template<typename E1>
    Matrix<T, INDEX2<T, D, E1, _Range > > operator() (const Array<Integer, E1> & Idx1, Integer Idx2)
    {
        return Matrix<T, INDEX2<T, D, E1, _Range > >(
                    INDEX2<T, D, E1, _Range >(*CAST_THIS(D), Idx1.expr, INT_RANGE(Idx2)) );
    }

    template<typename E1>
    const Matrix<T, INDEX2<T, D, E1, _Range > > operator() (const Array<Integer, E1> & Idx1, Integer Idx2) const
    {
        return Matrix<T, INDEX2<T, D, E1, _Range > >(
                    INDEX2<T, D, E1, _Range >(CAST_OBJ(D, *CONST_CAST_PTR(Matrix_Base,this)), Idx1.expr, INT_RANGE(Idx2)) );
    }

    template<typename E2>
    Matrix<T, INDEX2<T, D, _Range, E2 > > operator() (Integer Idx1, const Array<Integer, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, _Range, E2 > >(
                    INDEX2<T, D, _Range, E2 >(*CAST_THIS(D), INT_RANGE(Idx1), Idx2.expr) );
    }
    template<typename E2>
    const Matrix<T, INDEX2<T, D, _Range, E2 > > operator() (Integer Idx1, const Array<Integer, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, _Range, E2 > >(
                    INDEX2<T, D, _Range, E2 >(CAST_OBJ(D, *CONST_CAST_PTR(Matrix_Base,this)), INT_RANGE(Idx1), Idx2.expr) );
    }
    
    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > > operator() (
        const Array<bool, E1> & Idx1, const Array<bool, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > >(
                    INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> >(
                        *CAST_THIS(D), MASK2ARRAY<E1>(Idx1.expr), MASK2ARRAY<E2>(Idx2.expr)) );
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > > operator() (
        const Array<bool, E1> & Idx1, const Array<bool, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > >(
                    INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> >(
                        *CAST_THIS(D), MASK2ARRAY<E1>(Idx1.expr), MASK2ARRAY<E2>(Idx2.expr)) );
    }

    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2 > > operator() (
        const Array<bool, E1> & Idx1, const Array<Integer, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2  > >(
                    INDEX2<T, D, MASK2ARRAY<E1>, E2>(
                        *CAST_THIS(D), MASK2ARRAY<E1>(Idx1.expr), Idx2.expr) );
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2 > > operator() (
        const Array<bool, E1> & Idx1, const Array<Integer, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2  > >(
                    INDEX2<T, D, MASK2ARRAY<E1>, E2>(
                        *CAST_THIS(D), MASK2ARRAY<E1>(Idx1.expr), Idx2.expr) );
    }
    
    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > > operator() (
        const Array<Integer, E1> & Idx1, const Array<bool, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > >(
                    INDEX2<T, D, E1, MASK2ARRAY<E2> >(
                        *CAST_THIS(D), Idx1.expr, MASK2ARRAY<E2>(Idx2.expr) ) );
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > > operator() (
        const Array<Integer, E1> & Idx1, const Array<bool, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > >(
                    INDEX2<T, D, E1, MASK2ARRAY<E2> >(
                        *CAST_THIS(D), Idx1.expr, MASK2ARRAY<E2>(Idx2.expr) ) );
    }
    
// item 
     Matrix<T, INDEX2<T, D, _Range, _Range> > item (Integer ridx, Integer cidx)
    {
        return Matrix<T, INDEX2<T, D, _Range, _Range> >(
                    INDEX2<T, D, _Range, _Range>(*CAST_THIS(D), INT_RANGE(ridx), INT_RANGE(cidx)));
    }
    const Matrix<T, INDEX2<T, D, _Range, _Range> > item (Integer ridx, Integer cidx) const
    {
        return Matrix<T, INDEX2<T, D, _Range, _Range> >(
                    INDEX2<T, D, _Range, _Range>(*CONST_CAST_THIS(D), INT_RANGE(ridx), INT_RANGE(cidx)) );
    }

    Matrix<T, INDEX2<T, D, _Range, _Range> > item (const tuple<Integer,Integer> & Idx)
    {
        return Matrix<T, INDEX2<T, D, _Range, _Range> >(
                    INDEX2<T, D, _Range, _Range>(
                        *CAST_THIS(D), INT_RANGE(Idx.get<0>()),INT_RANGE(Idx.get<1>())));
    }
    const Matrix<T, INDEX2<T, D, _Range, _Range> > item (const tuple<Integer,Integer> & Idx) const
    {
        return Matrix<T, INDEX2<T, D, _Range, _Range> >(
                    INDEX2<T, D, _Range, _Range>(
                        *CONST_CAST_THIS(D), INT_RANGE(Idx.get<0>()),INT_RANGE(Idx.get<1>())));
    }

    template<typename T1, typename E1, typename T2, typename E2>
    Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > > item (const Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> > & z)
    {
        return operator[](z);
    }
    template<typename T1, typename E1, typename T2, typename E2>
    const Array<T, INDEX<T, D, ZIP<T1,E1,T2,E2> > > item (const Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> > & z) const
    {
        return operator[](z);
    }

    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, E1, E2> > item (const Array<Integer, E1> & Idx1, const Array<Integer, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, E1, E2> >(
                    INDEX2<T, D, E1, E2>(*CAST_THIS(D), Idx1.expr, Idx2.expr) );
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, E1, E2> > item (const Array<Integer, E1> & Idx1, const Array<Integer, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, E1, E2> >(
                    INDEX2<T, D, E1, E2>(*CAST_THIS(D), Idx1.expr, Idx2.expr) );
    }
    
    template<typename E1>
    Matrix<T, INDEX2<T, D, E1, _Range > > item (const Array<Integer, E1> & Idx1, Integer Idx2)
    {
        return Matrix<T, INDEX2<T, D, E1, _Range > >(
                    INDEX2<T, D, E1, _Range >(*CAST_THIS(D), Idx1.expr, INT_RANGE(Idx2)) );
    }
    template<typename E1>
    const Matrix<T, INDEX2<T, D, E1, _Range > > item (const Array<Integer, E1> & Idx1, Integer Idx2) const
    {
        return Matrix<T, INDEX2<T, D, E1, _Range > >(
                    INDEX2<T, D, E1, _Range >(*CAST_THIS(D), Idx1.expr, INT_RANGE(Idx2)) );
    }

    template<typename E2>
    Matrix<T, INDEX2<T, D, _Range, E2 > > item (Integer Idx1, const Array<Integer, E2> & Idx2)
    {
        return Matrix<T, INDEX2<T, D, _Range, E2 > >(
                    INDEX2<T, D, _Range, E2 >(*CAST_THIS(D), INT_RANGE(Idx1), Idx2.expr) );
    }
    template<typename E2>
    const Matrix<T, INDEX2<T, D, _Range, E2 > > item (Integer Idx1, const Array<Integer, E2> & Idx2) const
    {
        return Matrix<T, INDEX2<T, D, _Range, E2 > >(
                    INDEX2<T, D, _Range, E2 >(*CAST_THIS(D), INT_RANGE(Idx1), Idx2.expr) );
    }
    
    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > > item (
        const Array<bool, E1> & Idx1, const Array<bool, E2> & Idx2)
    {
        return operator[](Idx1,Idx2);
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, MASK2ARRAY<E2> > > item (
        const Array<bool, E1> & Idx1, const Array<bool, E2> & Idx2) const
    {
        return operator[](Idx1,Idx2);
    }

    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2 > > item (
        const Array<bool, E1> & Idx1, const Array<Integer, E2> & Idx2)
    {
        return operator[](Idx1,Idx2);
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, MASK2ARRAY<E1>, E2 > > item (
        const Array<bool, E1> & Idx1, const Array<Integer, E2> & Idx2) const
    {
        return operator[](Idx1,Idx2);
    }
    
    template<typename E1, typename E2>
    Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > > item (
        const Array<Integer, E1> & Idx1, const Array<bool, E2> & Idx2)
    {
        return operator[](Idx1,Idx2);
    }
    template<typename E1, typename E2>
    const Matrix<T, INDEX2<T, D, E1, MASK2ARRAY<E2> > > item (
        const Array<Integer, E1> & Idx1, const Array<bool, E2> & Idx2) const
    {
        return operator[](Idx1,Idx2);
    }
    //****
    Array<T,E> & asarray(void) { return static_cast<Array<T,E> &>(*this);}
    const Array<T,E> & asarray(void) const { return static_cast<const Array<T,E> &>(*this);}

    Array<T,E> & flat(void) { return asarray();}
    const Array<T,E> & flat(void) const { return asarray();}

    Array<T, Slice<T,E> > row(Integer idx) const 
    { 
        return Array<T, Slice<T,E> >(Slice<T,E>(this->expr, idx, idx+ncols*nrows, nrows));
    }
    Array<T, Slice<T,E> > column(Integer idx) const 
    { 
        return Array<T, Slice<T,E> >(Slice<T,E>(this->expr, idx*nrows, (idx+1)*nrows, 1));
    }

    Matrix<T, MTRANS<T,E> > Tr(void) { return Matrix<T, MTRANS<T,E> >(MTRANS<T,E>(*CAST_THIS(D)));}
    
    EXPOSE_PTR_QUERY(TPtrQuery);

    Matrix<T, MRESHAPE<T,E> > reshape(Integer _nrows, Integer _ncols) 
    {
        assert(_nrows*_ncols == nrows*ncols || ( (_nrows==-1) ^ (_ncols==-1) ) );
        if(_nrows == -1) _nrows = nrows*ncols/_ncols;
        if(_ncols == -1) _ncols = nrows*ncols/_nrows;
        return Matrix<T, MRESHAPE<T,E> >(
                         MRESHAPE<T,E>(*CAST_THIS(D), _nrows, _ncols));
    }
    
    template<typename T1>
    Matrix<T1, MUnaryOp<T1,Matrix<T,E>,_view<T1,T> > > view(void) const
    {
        return  Matrix<T1, MUnaryOp<T1,Matrix<T,E>,_view<T1,T> > >(
                           nrows, ncols, MUnaryOp<T1,Matrix<T,E>,_view<T1,T> > (*this, _view<T1,T>()));
    }

    tuple<Integer,Integer> shape(void) const { return tuple<Integer,Integer>(nrows,ncols);}

    Array<T,E> flatten(void) { return this->asarray(); }
    Array<T,E> flattenF(void) { return this->asarray(); }
    Array<T, MTRANS<T,E> > flattenC(void) { return Array<T, MTRANS<T,E> >(Tr()) ; }

    Array<T,E> ravel(void) { return flatten(); }
    Array<T,E> ravelF(void) { return flattenF(); }
    Array<T, MTRANS<T,E> > ravelC(void) { return flattenC() ; }

    Array<tuple<Integer,Integer>, ZIP<Integer,_Array<Integer>, Integer, _Array<Integer> > > 
        nonzero(void) const
    { 
        return _numcpp::nonzero(static_cast<const D &>(*this));
    }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=nrows;}
      
    Integer nrows;
    Integer ncols;
};

template<typename T, typename E>
struct Matrix: Matrix_Base<Matrix<T,E>,T,E>
{
    typedef Matrix TSelf;
    typedef E      TExpr;
    typedef Matrix_Base<Matrix<T,E>,T,E> TBase;

    // Matrix(void): TBase(0,0,T()) {}
    Matrix(const Array<T,E> & _expr): TBase(_expr) {}
    Matrix(Integer _nrows, Integer _ncols, const LRValue & lrvalue): TBase(_nrows,_ncols,T()) {SET_LRVALUE(lrvalue);}
    Matrix(Integer _nrows, Integer _ncols, const Array<T,E> & _expr): TBase(_nrows,_ncols,_expr) {}
    Matrix(Integer _nrows, Integer _ncols, const E & _expr): TBase(_nrows,_ncols,_expr) {}
    Matrix(Integer _nrows, Integer _ncols, const T & _val=T()): TBase(_nrows,_ncols,_val) {}

    Matrix(const Matrix<T,E> & mat): Matrix_Base<Matrix<T,E>,T,E>(mat) {}
    //template<typename S, typename E1>
    //Matrix(const Matrix<S,E1>& mat): Matrix_Base<Matrix<T,E>,T,E>(mat) {}
    
    Matrix(const E & _expr): Matrix_Base<Matrix<T,E>,T,E>(_expr) {}
    Matrix(Integer _nrows, Integer _ncols, T (*rndFunc)(void) ): Matrix_Base<Matrix<T,E>,T,E>(_nrows,_ncols, rndFunc) {}

   
    OVERLOAD_MASSIGN(TSelf, TExpr, ASSIGN, =);
};

template<typename T>
struct Matrix<T,_Array<T> >: Matrix_Base<Matrix<T,_Array<T> >,T,_Array<T> >
{
    typedef Matrix    TSelf;
    typedef _Array<T> TExpr;
    typedef Matrix_Base<Matrix<T,_Array<T> >,T,_Array<T> > TBase;

    Matrix(void): TBase(0,0) {SET_NAMED_LVALUE;}
    Matrix(Integer _nrows, Integer _ncols): TBase(_nrows,_ncols) {SET_NAMED_LVALUE;}
    Matrix(Integer _nrows, Integer _ncols, const Array<T,_Array<T> > & _expr): TBase(_nrows,_ncols,_expr) {SET_NAMED_LVALUE;}
    Matrix(const Array<T,_Array<T> > & _expr): TBase(_expr) { SET_NAMED_LVALUE }
    Matrix(Integer _nrows, Integer _ncols, const _Array<T>  & _expr): TBase(_nrows,_ncols,_expr) {SET_NAMED_LVALUE }
    Matrix(Integer _nrows, Integer _ncols, const T & _val): TBase(_nrows,_ncols,_val) {SET_NAMED_LVALUE;}

    Matrix(const TSelf & mat): TBase(mat) {SET_NAMED_LVALUE;}
    template<typename S, typename E1>
    Matrix(const Matrix<S,E1>& mat): TBase(mat) {SET_NAMED_LVALUE;}
    
    Matrix(const _Array<T>  & _expr): TBase(_expr) {SET_NAMED_LVALUE;}
    Matrix(Integer _nrows, Integer _ncols, T (*rndFunc)(void) ): TBase(_nrows,_ncols, rndFunc) {SET_NAMED_LVALUE;}
    template<ENUM_RAND E>
    Matrix(Integer _nrows, Integer _ncols, const random<E> & rndFunc): TBase(_nrows,_ncols, rndFunc) {SET_NAMED_LVALUE;}

    template<unsigned n, unsigned m>
    Matrix(T (&arr)[n][m]): TBase(n,m, new T[n*m]) 
    {   
        SET_NAMED_LVALUE;
        for(Integer ii=0; ii < n; ++ii)
            for(Integer jj=0; jj < m; ++jj)
                (*this)(ii,jj) = arr[ii][jj];
    }
    
    void sort(Integer axis, bool reverse=false)
    {
        sort(axis, Identity<T>(), reverse);
    }
    template<typename K>
    void sort(Integer axis, const K & key, bool reverse=false)
    {
        Integer L = (axis==0 ? this->ncols: this->nrows);

        if(axis==0){
            for(Integer ii=0; ii < L; ++ii)
                this->column(ii).sort(key,reverse);
        }
        else
        {
            for(Integer ii=0; ii < L; ++ii)
                this->row(ii).sort(key,reverse);
        }
    }
    OVERLOAD_MASSIGN(TSelf, TExpr, ASSIGN, =);
};

template<typename T>
struct Matrix<T,_SArray<T> >: Matrix_Base<Matrix<T,_SArray<T> >,T,_SArray<T> >
{
    Matrix(const T & _val): Matrix_Base<Matrix<T,_SArray<T> >,T,_SArray<T> >(_val) {}
};

template<typename T>
struct Matrix<T, Array<T,_SArray<T> > >:  Array<T,_SArray<T> >
{
    Matrix(const T & _val):  Array<T,_SArray<T> >(_val) {}
    const T operator() (Integer ridx, Integer cidx) const { return (*this)[0];}
    const Array<T,_SArray<T> > & asarray(void) { return static_cast<const Array<T,_SArray<T> > &>(*this);}
    
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=this->ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=this->nrows;}
};

template<typename T, typename E>
std::ostream & operator << (std::ostream & os, Matrix<T, E> mat)
{
    const char * delimPtr="[";
    os << delimPtr; delimPtr="";
    for(Integer ii=0; ii < mat.nrows; ++ii)
    {
        os << delimPtr /*<< std::showpoint*/ << mat.row(ii);
        delimPtr=",\n ";
    }
    os << "]" << std::endl;

    return os;
}


template<typename E>
std::ostream & operator << (std::ostream & os, Matrix<Integer, E> mat)
{
    Integer mxint = max(abs(mat));
    Integer len = (Integer)(std::log10((double)mxint))+1;

    const char * delimPtr="[";
    os << delimPtr; delimPtr="";
    for(Integer ii=0; ii < mat.nrows; ++ii)
    {
        os << delimPtr << mat.row(ii);
        delimPtr=",\n ";
    }
    os << "]" << std::endl;

    return os;
}


//************** Matrix Dot product
template<typename D, typename T, typename E1, typename E2>
struct MBinOpBase: RefCount2CRTP<MBinOpBase<D,T,E1,E2> >
{
    typedef T value_type;

    MBinOpBase(const E1 & _expr1, const E2 & _expr2): 
        expr1(_expr1), expr2(_expr2), nrows(std::max(_expr1.nrows, _expr2.nrows)), ncols(std::max(_expr1.nrows,_expr2.ncols)) {}

    Integer size(void) const {return nrows*ncols;}
   
    typedef Const_Iterator<T,D> const_iterator;
    const_iterator begin(void) const { return const_iterator(static_cast<const D &>(*this), 0);}
    const_iterator end(void) const { return const_iterator(static_cast<const D &>(*this), size());}

    const T operator() (Integer ridx, Integer cidx) const { return static_cast<const D &>(*this)[nrows*cidx+ridx];}
    
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=nrows;}

    bool Qdo_copy(void * ptr) const { return expr1.Qdo_copy(ptr) || expr2.Qdo_copy(ptr);}

	const E1  expr1;
	const E2  expr2;
    Integer nrows;
    Integer ncols;
};

template<typename T, typename E1, typename E2, typename F>
struct MBinOp: MBinOpBase<MBinOp<T,E1,E2,F>,T,E1,E2>
{   MBinOp(const E1 & _expr1, const E2 & _expr2, const F &_func):
        MBinOpBase<MBinOp<T,E1,E2,F>,T,E1,E2>(_expr1,_expr2), func(_func) 
        {
            assert(_expr1.size()*_expr2.size()==0 || (_expr1.nrows == _expr2.nrows && _expr1.ncols == _expr2.ncols));
        }
    
    T operator[](Integer idx) const { return func(this->expr1[idx], this->expr2[idx]); }
    
    F func;
};

//******************  Unary Op

template<typename D, typename E, typename T>
struct MUnaryOpBase: RefCountCRTP<MUnaryOpBase<D,E,T> >
{
    MUnaryOpBase(const E & _expr): expr(_expr), nrows(_expr.nrows), ncols(_expr.ncols)  {}
    Integer size(void) const { return expr.size();}

    typedef T value_type;
    typedef Const_Iterator<T, D> const_iterator;

    const_iterator begin(void) const { return const_iterator(static_cast<const D &>(*this), 0);}
    const_iterator end(void) const { return const_iterator(static_cast<const D &>(*this), this->size());}

    const T operator() (Integer ridx, Integer cidx) const { return static_cast<const D &>(*this)[this->nrows*cidx+ridx];}
    
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=this->ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=this->nrows;}

    bool Qdo_copy(void * ptr) const { return expr.Qdo_copy(ptr);}

    const E expr;
    Integer nrows;
    Integer ncols;
};


template<typename T, typename E, typename F>
struct MUnaryOp: MUnaryOpBase<MUnaryOp<T,E,F>,E,T>
{
    MUnaryOp(const E & _expr, const F & _func): MUnaryOpBase<MUnaryOp<T,E,F>,E,T>(_expr), func(_func) {}

    const T operator[](Integer idx) const { return func(this->expr[idx]);}

    const F func;
};

//-------------- DOT expression
template<typename E1, typename E2>
void getDotRange(Integer i, const E1 & expr1, Integer & k1, Integer & k2, const E2 & expr2, Integer j)
{
    Integer t1, t2;
    expr1.nzCols(i,k1,k2); expr2.nzRows(j,t1,t2);
    k1 = std::max(k1,t1); k2 = std::min(k2,t2);
}

template<typename T, typename E1, typename E2>
struct DOT: MBinOpBase<DOT<T,E1,E2>, T, E1, E2>
{
    DOT(const E1 & _expr1, const E2 &_expr2): MBinOpBase<DOT<T,E1,E2>,T,E1,E2>(_expr1,_expr2) 
    {   
        assert(_expr1.ncols==_expr2.nrows);
        this->nrows = _expr1.nrows; this->ncols = _expr2.ncols;
    }
    
	const T operator [](Integer idx) const 
    { 
        T SS=0;

        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;

        Integer k1, k2;
        getDotRange(i, this->expr1, k1, k2, this->expr2, j);
        k1=std::max(k1,0); k2=std::min(k2,this->expr1.ncols);
        
        for(Integer k=k1; k < k2; ++k)
        {
            SS += this->expr1(i,k)*this->expr2(k,j);
        }
        return SS;
    }

    bool Qdo_copy(void * ptr) const { return this->ref_count_node(ptr) > 0; }
};

// Specialized DOT for expr1
#define E1 Matrix<T, _SArray<T> >
template<typename T, typename E2>
struct DOT<T,E1,E2>: MBinOpBase<DOT<T,E1,E2>, T, E1, E2>
{
    DOT(const E1 & _expr1, const E2 &_expr2): MBinOpBase<DOT<T,E1,E2>,T,E1,E2>(_expr1,_expr2) {this->nrows=_expr2.nrows; this->ncols=_expr2.ncols;}
    
	const T operator [](Integer idx) const 
    { 
        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;
        return this->expr1(i,j)*this->expr2(i,j);
    }
    
    bool Qdo_copy(void * ptr) const { return this->ref_count_node(ptr) > 0; }
};
#undef E1

// Specialized DOT for expr2
#define E2 Matrix<T, _SArray<T> >
template<typename T, typename E1>
struct DOT<T,E1,E2>: MBinOpBase<DOT<T,E1,E2>, T, E1, E2>
{
    DOT(const E1 & _expr1, const E2 &_expr2): MBinOpBase<DOT<T,E1,E2>,T,E1,E2>(_expr1,_expr2) {this->nrows=_expr1.nrows; this->ncols=_expr1.ncols;}
    
	const T operator [](Integer idx) const 
    { 
        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;
        return this->expr1(i,j)*this->expr2(i,j);
    }
    
    bool Qdo_copy(void * ptr) const { return this->ref_count_node(ptr) > 0; }
};
#undef E2

// Convolution
enum ENUM_CONV { FULL, SAME, VALID };
template<typename T, typename E1, typename E2, ENUM_CONV> struct CONVOLVE;
template<typename D, typename T, typename E1, typename E2> struct CONVOLVE_BASE;

template<typename D, typename T, typename E1, typename E2>
struct CONVOLVE_BASE: MBinOpBase<D, T, E1, E2>
{
    typedef CONVOLVE_BASE TSelf;
    typedef MBinOpBase<D, T, E1, E2> TBase;

    CONVOLVE_BASE(const E1 & _expr1, const E2 &_expr2): TBase(_expr1,_expr2) 
    {
        this->nrows = _expr1.nrows;
    }
    
    const T operator [](Integer idx) const 
    { 
        T SS=0;

        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;

        Integer k1 = std::max(0, j+1-this->expr1.ncols);
        Integer k2 = std::min(j+1, this->expr2.size());
        for(Integer k=k1; k < k2; ++k)
        {
            SS += this->expr1(i,j-k)*this->expr2[k];
        }
        return SS;
    }

    bool Qdo_copy(void * ptr) const { return this->ref_count_node(ptr) > 0; }
};

template<typename T, typename E1, typename E2>
struct CONVOLVE<T,E1,E2,FULL>: CONVOLVE_BASE<CONVOLVE<T,E1,E2,FULL>,T,E1,E2> 
{
    typedef CONVOLVE TSelf;
    typedef CONVOLVE_BASE<CONVOLVE<T,E1,E2,FULL>,T,E1,E2>  TBase;
    CONVOLVE(const E1 & _expr1, const E2 &_expr2): TBase(_expr1,_expr2) 
    {
        this->ncols = this->expr1.ncols + this->expr2.size()-1;
    }
};

template<typename T, typename E1, typename E2>
struct CONVOLVE<T,E1,E2,SAME>: CONVOLVE_BASE<CONVOLVE<T,E1,E2,SAME>,T,E1,E2> 
{
    typedef CONVOLVE TSelf;
    typedef CONVOLVE_BASE<CONVOLVE<T,E1,E2,SAME>,T,E1,E2>  TBase;
    CONVOLVE(const E1 & _expr1, const E2 &_expr2): TBase(_expr1,_expr2)
    {
        offset = this->expr1.ncols + this->expr2.size()-1;;
        this->ncols = std::max(this->expr1.ncols,this-> expr2.size());
        offset = (offset - this->ncols)/2;
    }
    const T operator [](Integer idx) const 
    {
        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;
        return TBase::operator[](i+this->nrows*(j+offset));
    }

    Integer offset;
};

template<typename T, typename E1, typename E2>
struct CONVOLVE<T,E1,E2,VALID>: CONVOLVE_BASE<CONVOLVE<T,E1,E2,VALID>,T,E1,E2> 
{
    typedef CONVOLVE TSelf;
    typedef CONVOLVE_BASE<CONVOLVE<T,E1,E2,VALID>,T,E1,E2>  TBase;
    CONVOLVE(const E1 & _expr1, const E2 &_expr2): TBase(_expr1,_expr2)
    {
        offset = this->expr1.ncols + this->expr2.size()-1;;
        this->ncols = std::abs(this->expr1.ncols-this->expr2.size())+1;
        offset = (offset - this->ncols)/2;
    }
    const T operator [](Integer idx) const 
    {
        Integer i = idx % this->nrows;
        Integer j = idx / this->nrows;
        return TBase::operator[](i+this->nrows*(j+offset));
    }

    Integer offset;
};

//**** MOP definition
#define DEF_MOP(NAME, OPFUNC)\
    template<typename T, typename E1, typename E2>\
    struct NAME: MBinOp<T,E1,E2, OPFUNC<T> > \
    { NAME(const E1 & _expr1, const E2 &_expr2): MBinOp<T,E1,E2, OPFUNC<T> >(_expr1,_expr2, OPFUNC<T>()){} };

DEF_MOP(MADD, std::plus);
DEF_MOP(MSUB, std::minus);
DEF_MOP(MMULT, std::multiplies);
DEF_MOP(MDIV, std::divides);
DEF_MOP(MMOD, std::modulus);

//**** Mask expressions
template<typename D, typename T, typename E>
struct VIEWBase: RefCountCRTP<D>, MPointerQuery<D,T>
{
    VIEWBase(const E &_expr): expr(_expr), ptr_expr(LVALUE_PTR(E,_expr)), nrows(_expr.nrows), ncols(_expr.ncols), cZERO() {}
    Integer size(void) const {return nrows*this->ncols;}
   
    typedef Const_Iterator<T,D> const_iterator;
    const_iterator begin(void) const { return const_iterator(static_cast<const D &>(*this), 0);}
    const_iterator end(void) const { return const_iterator(static_cast<const D &>(*this), size());}

    typedef Iterator<T,D> iterator;
    iterator begin(void) { return iterator(*CAST_THIS(D), 0);}
    iterator end(void) { return iterator(*CAST_THIS(D), size());}

    const T operator() (Integer ridx, Integer cidx) const { return static_cast<const D &>(*this)[nrows*cidx+ridx];}

	E  expr;
    E * ptr_expr;
    Integer nrows;
    Integer ncols;
    T cZERO;
    T ZERO;
};

template <typename T, typename E>
struct DIAGVIEW: VIEWBase<DIAGVIEW<T,E>,T,E>
{
    DIAGVIEW(const E &_expr, Integer _offset=0): VIEWBase<DIAGVIEW<T,E>,T,E>(_expr), offset(_offset) {}

    const T operator [](Integer idx) const 
    { 
        return ((idx-offset)%(this->nrows+1)==0) ? this->expr[idx]: this->cZERO;
    }

    T & operator [](Integer idx) 
    { 
        return ((idx-offset)%(this->nrows+1)==0) ? this->expr[idx]: (this->ZERO=this->cZERO);
    }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=row+offset; stop=start+1;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=col-offset; stop=start+1;}

    Integer offset;
};

template <typename T, typename E>
struct UTVIEW: VIEWBase<UTVIEW<T,E>,T,E>
{
    UTVIEW(const E &_expr, Integer _offset = 0): VIEWBase<UTVIEW<T,E>,T,E>(_expr), offset(_offset) {}

    const T operator [](Integer idx) const 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        return (rr > cc-offset) ? this->cZERO : this->expr[idx];
    }

    T & operator [](Integer idx) 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        return (rr > cc-offset) ? (this->ZERO=this->cZERO) : this->expr[idx];
    }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=row+offset; stop=this->nrows;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=col-offset+1;}

    Integer offset;
};


template <typename T, typename E>
struct LTVIEW: VIEWBase<LTVIEW<T,E>,T,E>
{
    LTVIEW(const E &_expr, Integer _offset = 0): VIEWBase<LTVIEW<T,E>,T,E>(_expr), offset(_offset) {}

    const T operator [](Integer idx) const 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        return (rr < cc-offset) ? this->cZERO : this->expr[idx];
    }

    T & operator [](Integer idx) 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        return (rr < cc-offset) ? (this->ZERO=this->cZERO) : this->expr[idx];
    }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=row+offset+1;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=col-offset; stop=this->nrows;}

    Integer offset;
};

template<typename D, typename T, typename E1, typename E2>
struct STACKBase: MBinOpBase<D, T, E1, E2>
{
    typedef STACKBase TSelf;
    typedef MBinOpBase<D, T, E1, E2> TBase;

    STACKBase(const E1 & _expr1, const E2 &_expr2): TBase(_expr1,_expr2) {} 

    EXPOSE_CONST_ITER(TBase);

    typedef Iterator<T,D> iterator;
    iterator begin(void) const { return iterator(static_cast<D &>(*this), 0);}
    iterator end(void) const { return iterator(static_cast<D &>(*this), this->size());}

    bool Qdo_copy(void * ptr) const { return this->ref_count_node(ptr) > 0; }
};

template<typename T, typename E1, typename E2>
struct VSTACK: STACKBase<VSTACK<T,E1,E2>,T,E1,E2>
{
    typedef STACKBase<VSTACK<T,E1,E2>,T,E1,E2> TBase;

    VSTACK(const Matrix<T,E1>& _expr1, const Matrix<T,E2>& _expr2): TBase(_expr1, _expr2) 
    {
        assert(this->expr1.ncols == this->expr2.ncols);

        this->nrows = this->expr1.nrows + this->expr2.nrows;
        this->ncols = this->expr1.ncols;
    }

    const T operator [](Integer idx) const 
    { 
        Integer ridx = idx % this->nrows;
        Integer cidx = idx / this->nrows;
        if(ridx < this->expr1.nrows) 
            return this->expr1(ridx,cidx); 
        else 
            return this->expr2(ridx-this->expr1.nrows,cidx);
    }

    T & operator [](Integer idx)
    {
        Integer ridx = idx % this->nrows;
        Integer cidx = idx / this->nrows;
        if(ridx < this->expr1.nrows) 
            return this->expr1(ridx,cidx); 
        else 
            return this->expr2(ridx-this->expr1.nrows,cidx);
    }
};

template<typename T, typename E1, typename E2>
struct HSTACK: STACKBase<HSTACK<T,E1,E2>,T,E1,E2>
{
    typedef STACKBase<HSTACK<T,E1,E2>,T,E1,E2> TBase;

    HSTACK(const Matrix<T,E1>& _expr1, const Matrix<T,E2>& _expr2): TBase(_expr1, _expr2) 
    {
        assert(this->expr1.nrows == this->expr2.nrows);

        this->ncols = this->expr1.ncols + this->expr2.ncols;
        this->nrows = this->expr1.nrows;
    }

    const T operator [](Integer idx) const 
    { 
        Integer ridx = idx % this->nrows;
        Integer cidx = idx / this->nrows;
        if(cidx < this->expr1.ncols) 
            return this->expr1(ridx,cidx); 
        else 
            return this->expr2(ridx, cidx-this->expr1.ncols);
    }

    T & operator [](Integer idx)
    {
        Integer ridx = idx % this->nrows;
        Integer cidx = idx / this->nrows;
        if(cidx < this->expr1.ncols) 
            return this->expr1(ridx,cidx); 
        else 
            return this->expr2(ridx, cidx-this->expr1.ncols);
    }
};

//------------- Special Treatment for Mask
#define DEF_MOP_MASK(RMSK, MSK1, NAME, OP, MSK2) \
    template<typename T, typename E1, typename E2>\
    inline Matrix<T, RMSK<T,NAME<T,MSK1<T,E1>,MSK2<T,E2> > > > OP (\
            const Matrix<T, MSK1<T,E1> > & expr1, const Matrix<T, MSK2<T,E2> > & expr2) \
    { \
        return Matrix<T, RMSK<T,NAME<T,MSK1<T,E1>,MSK2<T,E2> > > > (\
            RMSK<T,NAME<T,MSK1<T,E1>,MSK2<T,E2> > >(\
                NAME<T,MSK1<T,E1>,MSK2<T,E2> >(expr1.expr, expr2.expr)));\
    }

//------------ General Dot Product
//         Dot product with array
template<typename T, typename E1, typename E2>
inline const Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > > dot (const Matrix<T,E1> & expr1, const Matrix<T,E2> & expr2) 
{ 
    return Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > >(DOT<T,Matrix<T,E1>,Matrix<T,E2> >(expr1, expr2));
}
template<typename T, typename E1>
inline const Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,_SArray<T> > > > dot (const Matrix<T,E1> & expr1, const T & expr2) 
{ 
    return Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,_SArray<T> > > >(
                DOT<T,Matrix<T,E1>,Matrix<T,_SArray<T> > >(expr1, Matrix<T,_SArray<T> >(expr2)));
}
template<typename T, typename E2>
inline const Matrix<T, DOT<T,Matrix<T,_SArray<T> >,Matrix<T,E2> > > dot (const T & expr1, const Matrix<T,E2> & expr2) 
{ 
    return Matrix<T, DOT<T,Matrix<T,_SArray<T> >,Matrix<T,E2> > >(
                DOT<T,Matrix<T,_SArray<T> >,Matrix<T,E2> >(Matrix<T,_SArray<T> >(expr1), expr2));
}
template<typename T, typename E1, typename E2>
inline const Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > > dot(const Array<T,E1> & expr1, const Matrix<T,E2> & expr2)
{
    return Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > >(
                DOT<T,Matrix<T,E1>,Matrix<T,E2> >(
                        Matrix<T,E1>(1, expr1.size(), expr1.expr), expr2));
}

template<typename T, typename E1, typename E2>
inline const Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > > dot(const Matrix<T,E1> & expr1, const Array<T,E2> & expr2)
{
    return Matrix<T, DOT<T,Matrix<T,E1>,Matrix<T,E2> > >(
                DOT<T,Matrix<T,E1>,Matrix<T,E2> >(
                        expr1, Matrix<T,E2>(expr2.size(), 1, expr2.expr)));
}

template<typename T, typename E1, typename E2>
inline const T dot(const Array<T,E1> & expr1, const Array<T,E2> & expr2)
{
    return dot(expr1.asRowMat(), expr2.asColMat())[0];
}
//
////*****       Return_mask, Mask1,  operator,  Mask2
#define DEF_MOP_RULE(NAME, OP) \
    DEF_MOP_MASK(DIAGVIEW, DIAGVIEW, NAME, OP, DIAGVIEW);\
    DEF_MOP_MASK(UTVIEW, DIAGVIEW, NAME, OP, UTVIEW);\
    DEF_MOP_MASK(LTVIEW, DIAGVIEW, NAME, OP, LTVIEW);\
    DEF_MOP_MASK(UTVIEW, UTVIEW, NAME, OP, DIAGVIEW);\
    DEF_MOP_MASK(UTVIEW, UTVIEW, NAME, OP, UTVIEW);\
    DEF_MOP_MASK(LTVIEW, LTVIEW, NAME, OP, DIAGVIEW);\
    DEF_MOP_MASK(LTVIEW, LTVIEW, NAME, OP, LTVIEW);\
    
DEF_MOP_RULE(DOT, dot);
DEF_MOP_RULE(MADD, operator +);
DEF_MOP_RULE(MSUB, operator -);
DEF_MOP_RULE(MMULT, operator *);
DEF_MOP_RULE(MDIV, operator /);
DEF_MOP_RULE(MMOD, operator %);

//************* Matrix Transpose
template<typename T, typename E>
struct MTRANS: RefCountCRTP<MTRANS<T,E> >, MPointerQuery<MTRANS<T,E>, T>
{
    typedef PointerQuery<MTRANS<T,E>, T> TPtrQuery;

	MTRANS(Matrix<T,E> & _expr): 
        expr(_expr), ptr_expr(LVALUEM_PTR(T,E,_expr)), nrows(_expr.ncols), ncols(_expr.nrows) {}
    
    T & operator [](Integer idx) { return expr[(idx/nrows)+(idx%nrows)*ncols];}
    const T  operator [](Integer idx) const { return expr[(idx/nrows)+(idx%nrows)*ncols];}
    Integer size(void) const {return nrows*ncols;}

    typedef Const_Iterator<T,MTRANS> const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0); }
    const_iterator end(void) const { return const_iterator(*this, nrows*ncols); }

    typedef Iterator<T,MTRANS> iterator;
    iterator begin(void) { return iterator(*this, 0); }
    iterator end(void) { return iterator(*this, nrows*ncols); }

    void nzCols(Integer row, Integer & start, Integer & stop) const { expr.nzRows(row, start, stop);}
    void nzRows(Integer col, Integer & start, Integer & stop) const { expr.nzCols(col, start, stop);}

    // redefine Qdo_copy
    bool Qdo_copy(void * ptr) const { return this->get()==ptr || expr.Qdo_copy(ptr);}
    // redifine resize
    shared_array<T> & resize(Integer _nrows, Integer _ncols)
    {
        nrows=_nrows;
        ncols=_ncols;
        return TPtrQuery::resize(_ncols, _nrows);
    }

    Matrix<T,E> expr;
    Matrix<T,E> * ptr_expr;
    Integer nrows;
    Integer ncols;
};


//************ Matrix reshape
template<typename T, typename E>
struct MRESHAPE: RefCountCRTP<MRESHAPE<T,E> >, PointerQuery<MRESHAPE<T,E>, T>
{
    MRESHAPE(Matrix<T,E> & _expr, Integer _nrows, Integer _ncols): 
        expr(_expr), ptr_expr(LVALUEM_PTR(T,E,_expr)), nrows(_nrows), ncols(_ncols) {}
    
    T & operator [](Integer idx) { return expr[(idx/nrows)*ncols+(idx%nrows)];}
    const T  operator [](Integer idx) const { return expr[(idx/nrows)*ncols+(idx%nrows)];}
    Integer size(void) const {return nrows*ncols;}

    typedef Const_Iterator<T,MRESHAPE> const_iterator;
    const_iterator begin(void) const { return const_iterator(*this, 0); }
    const_iterator end(void) const { return const_iterator(*this, nrows*ncols); }

    typedef Iterator<T,MRESHAPE> iterator;
    iterator begin(void) { return iterator(*this, 0); }
    iterator end(void) { return iterator(*this, nrows*ncols); }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=nrows;}

    Matrix<T,E> expr;
    Matrix<T,E> * ptr_expr;
    Integer nrows;
    Integer ncols;
};

//************ Triangular Matrix
template<typename D, typename T>
struct TRIMAT: RefCountCRTP<TRIMAT<D,T> >, PointerQuery<TRIMAT<D,T>, T>
{
    TRIMAT(Integer _rows, Integer _cols, const T & _val=T()): 
        dim(std::min(_rows,_cols)), nrows(_rows), ncols(_cols), _len(dim*(dim+1)/2), 
        expr(new T[_len], _len), cZERO()
    {
        std::fill(expr.get(), expr.get()+_len, _val);
    }
    
    TRIMAT(Integer _rows, Integer _cols, T (*rndFunc)(void) ): 
        dim(std::min(_rows,_cols)), nrows(_rows), ncols(_cols), _len(dim*(dim+1)/2), 
        expr(new T[_len], _len,rndFunc), cZERO() {}

    Integer size(void) const {return nrows*ncols;}

    typedef T value_type;
	typedef Iterator<T, D> iterator;
    typedef Const_Iterator<T, D> const_iterator;

    iterator begin(void) { return iterator(static_cast<D &>(*this), 0);}
    iterator end(void) { return iterator(static_cast<D &>(*this), nrows*ncols);}

    const_iterator begin(void) const { return const_iterator(static_cast<const D &>(*this), 0);}
    const_iterator end(void) const { return const_iterator(static_cast<const D &>(*this), nrows*ncols);}

    //redefin resize
    shared_array<T> & resize(Integer _nrows, Integer _ncols)
    {
        nrows=_nrows;
        ncols=_ncols;
        dim=std::min(nrows,ncols);
        _len=dim*(dim+1)/2;
        return this->expr.resize(_len);
    }

    Integer dim;
    Integer nrows;
    Integer ncols;
    Integer _len; // length of physical memory
    T ZERO;
    T cZERO;
    _Array<T> expr;

};

//************ Symmetric Matrix
template<typename T>
struct SYMMAT: TRIMAT<SYMMAT<T>,T>
{
    SYMMAT(Integer _dim, const T & _val=T()): TRIMAT<SYMMAT<T>,T>(_dim, _dim, _val) {}
    SYMMAT(Integer _dim,  T (*rndFunc)(void) ): TRIMAT<SYMMAT<T>,T>(_dim, _dim, rndFunc) {}
    
    T &  operator [](Integer idx)
    {   // upper triangular representation, column-major
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        if(rr > cc) std::swap(rr,cc); 
        return this->expr[rr+cc*(cc+1)/2];
    }

    const T  operator [](Integer idx) const 
    {   // upper triangular representation, column-major
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;
        if(rr > cc) std::swap(rr,cc); 
        return this->expr[rr+cc*(cc+1)/2];
    }

    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=this->ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=this->nrows;}
};

template<typename T>
struct SMatrix : Matrix<T, SYMMAT<T> >
{
    SMatrix(Integer _dim, const T & _val=T()): Matrix<T,SYMMAT<T> >(SYMMAT<T>(_dim, _val)) {}
    SMatrix(Integer _dim, T (*rndFunc)(void)): Matrix<T,SYMMAT<T> >(SYMMAT<T>(_dim, rndFunc)) {}

    SMatrix & operator = (const SMatrix & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=0; cc <= rr; ++cc)
                (*this)(rr,cc)=other(rr,cc);
        return *this;
    }

    template<typename E1>
    SMatrix & operator = (const Matrix<T,E1> & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=0; cc <= rr; ++cc)
                (*this)(rr,cc)=other(rr,cc);
        return *this;
    }

};

//************ Upper Triangular Matrix
template<typename T>
struct _UTRIMAT: TRIMAT<_UTRIMAT<T>,T>
{
    _UTRIMAT(Integer _rows, Integer _cols, const T & _val=T()): 
        TRIMAT<_UTRIMAT<T>,T>(_rows, _cols,  _val) {}
    _UTRIMAT(Integer _rows, Integer _cols,  T (*rndFunc)(void) ): 
        TRIMAT<_UTRIMAT<T>,T>(_rows, _cols, rndFunc) {}
       
    T & operator [](Integer idx)
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;

        return (rr > cc) ? (this->ZERO=this->cZERO) : this->expr[rr+cc*(cc+1)/2];
    }

    const T  operator [](Integer idx) const 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows;

        return (rr > cc) ? this->cZERO : this->expr[rr+cc*(cc+1)/2];
    }
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=row; stop=this->ncols;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=0; stop=col+1;}
};

template<typename T>
struct UTMatrix : Matrix<T, UTVIEW<T, _UTRIMAT<T> > >
{
    UTMatrix(Integer _rows, Integer _cols, const T & _val=T()): 
        Matrix<T, UTVIEW<T, _UTRIMAT<T> > >(
            UTVIEW<T, _UTRIMAT<T> >(
                _UTRIMAT<T>(_rows, _cols, _val))) {}

    UTMatrix(Integer _rows, Integer _cols, T (*rndFunc)(void)): 
        Matrix<T, UTVIEW<T, _UTRIMAT<T> > >(
            UTVIEW<T, _UTRIMAT<T> >(
                _UTRIMAT<T>(_rows, _cols, rndFunc))) {}
    
    UTMatrix & operator = (const UTMatrix & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=rr; cc < this->ncols; ++cc)
            {
                (*this)(rr,cc)=other(rr,cc);
            }
        return *this;
    }

    template<typename E1>
    UTMatrix & operator = (const Matrix<T,E1> & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=rr; cc < this->ncols; ++cc)
            {
                (*this)(rr,cc)=other(rr,cc);
            }
        return *this;
    }
};


//************ Lower Triangular Matrix
template<typename T>
struct _LTRIMAT: TRIMAT<_LTRIMAT<T>,T>
{
    _LTRIMAT(Integer _rows, Integer _cols, const T & _val=T()): TRIMAT<_LTRIMAT<T>,T>(_rows, _cols, _val) {}
    _LTRIMAT(Integer _rows, Integer _cols,  T (*rndFunc)(void) ): TRIMAT<_LTRIMAT<T>,T>(_rows, _cols, rndFunc) {}
       
    T & operator [](Integer idx)
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows; 
        return (rr < cc) ? (this->ZERO=this->cZERO): this->expr[cc+rr*(rr+1)/2];
    }

    const T  operator [](Integer idx) const 
    { 
        Integer rr=idx%this->nrows; 
        Integer cc=idx/this->nrows; 
        return (rr < cc) ? this->cZERO: this->expr[cc+rr*(rr+1)/2];
    }
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=0; stop=row+1;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=col; stop=this->nrows;}
};

template<typename T>
struct LTMatrix : Matrix<T, LTVIEW<T,_LTRIMAT<T> > >
{
    LTMatrix(Integer _rows, Integer _cols, const T & _val=T()): 
        Matrix<T, LTVIEW<T,_LTRIMAT<T> > >(
            LTVIEW<T,_LTRIMAT<T> >(
                _LTRIMAT<T>(_rows, _cols, _val))) {}
    LTMatrix(Integer _rows, Integer _cols, T (*rndFunc)(void)): 
        Matrix<T, LTVIEW<T,_LTRIMAT<T> > >(
            LTVIEW<T,_LTRIMAT<T> >(
                _LTRIMAT<T>(_rows, _cols, rndFunc))) {}
    
    LTMatrix & operator = (const LTMatrix & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=0; cc <= rr; ++cc)
            {
                (*this)(rr,cc)=other(rr,cc);
            }
        return *this;
    }

    template<typename E1>
    LTMatrix & operator = (const Matrix<T,E1> & other)
    {
        for(Integer rr=0; rr < this->nrows; ++rr)
            for(Integer cc=0; cc <= rr; ++cc)
            {
                (*this)(rr,cc)=other(rr,cc);
            }
        return *this;
    }
};

//************ Diagonal Matrix
template<typename T>
struct _DIAGMAT: RefCountCRTP<_DIAGMAT<T> >, PointerQuery<_DIAGMAT<T>, T>
{
    _DIAGMAT(Integer _rows, Integer _cols, const T & _val=T()): nrows(_rows), ncols(_cols), _len(std::min(_rows,_cols)), 
        cZERO(T(0)), expr(new T[std::min(_rows,_cols)], std::min(_rows,_cols)) 
    {
        std::fill(expr.get(), expr.get()+_len, _val);
    }
     
    _DIAGMAT(Integer _rows, Integer _cols,  T (*rndFunc)(void) ):   nrows(_rows), ncols(_cols), _len(std::min(_rows,_cols)), 
        cZERO(T(0)), expr(new T[std::min(_rows,_cols)], std::min(_rows,_cols), rndFunc)   {}

    template<typename E>
    _DIAGMAT(const Array<T,E> & _expr): nrows(_expr.size()), ncols(_expr.size()), 
            _len(_expr.size()), cZERO(T(0)), expr(new T[_expr.size()], _expr.size()) 
    {
        expr = _expr;
    }

    template<typename E>
    _DIAGMAT & operator = (const Array<T,E> & _expr) { expr = _expr; return *this;}

    T & operator [](Integer idx) 
    {   
        return idx % (nrows+1) == 0 ? expr[idx/(nrows+1)]: (ZERO=cZERO);
    }

    const T operator [](Integer idx) const
    { 
        return idx % (nrows+1) == 0 ? expr[idx/(nrows+1)]: cZERO;
    }

    Integer size(void) const {return nrows*ncols;}


    typedef T value_type;
	typedef Iterator<T, _DIAGMAT<T> > iterator;
    typedef Const_Iterator<T, _DIAGMAT<T> > const_iterator;

    iterator begin(void) { return iterator(*this, 0);}
    iterator end(void) { return iterator(*this, nrows*ncols);}

    const_iterator begin(void) const { return const_iterator(*this, 0);}
    const_iterator end(void) const { return const_iterator(*this, nrows*ncols);}
    
    void nzCols(Integer row, Integer & start, Integer & stop) const { start=row; stop=start+1;}
    void nzRows(Integer col, Integer & start, Integer & stop) const { start=col; stop=start+1;}

    //redefine resize
    shared_array<T> & resize(Integer _nrows, Integer _ncols)
    {
        nrows=_nrows;
        ncols=_ncols;
        return expr.resize(std::min(_nrows,_ncols));
    }
    Integer _len;
    Integer nrows;
    Integer ncols;
    T ZERO;
    const T cZERO;
    _Array<T> expr;
};

template<typename T>
struct DMatrix : Matrix<T, DIAGVIEW<T, _DIAGMAT<T> > >
{
    DMatrix(Integer _rows, Integer _cols, const T & _val=T()): 
        Matrix<T, DIAGVIEW<T, _DIAGMAT<T> > >(
            DIAGVIEW<T,_DIAGMAT<T> >(
                _DIAGMAT<T>(_rows, _cols, _val))) {}
    
    DMatrix(Integer _rows, Integer _cols,  T (*rndFunc)(void) ): 
        Matrix<T, DIAGVIEW<T, _DIAGMAT<T> > >(
            DIAGVIEW<T,_DIAGMAT<T> > (
                _DIAGMAT<T>(_rows, _cols, rndFunc))){}

    template<typename E>
    explicit DMatrix(const Array<T,E> & _expr): 
        Matrix<T, DIAGVIEW<T, _DIAGMAT<T> > >(
                DIAGVIEW<T,_DIAGMAT<T> >(
                    _DIAGMAT<T>(_expr)))
    {
        this->expr.expr = _expr;
    }

    template <typename S, typename E>
    DMatrix & operator = (const Matrix<S,E> & mat)
    {
        for(Integer ii=0; ii < this->nrows; ++ii)
            (*this)(ii,ii) = mat(ii,ii);
        return *this;
    }

};

//**** Utility functions

double randU(void); 
double randN(void);

template<>
struct random<UNIFORM>
{
    double operator() (void) const { return randU();}
};

template<>
struct random<NORMAL>
{
    double operator() (void) const { return randN();}
};

//********************************************
//***  sum & cumsum
//********************************************
#define CUM_NAME(NAME)  cum ## NAME

#define DEF_SUMPROD(NAME)\
    template<typename T,typename E>\
    inline typename sum_prod<Array<T,E> >::type NAME(const Array<T,E>& arr)\
    {\
        return sum_prod<Array<T,E> >::NAME(arr);\
    }\
    \
    template<typename T,typename E>\
    inline typename sum_prod<Array<T,E> >::cum_type CUM_NAME(NAME)(const Array<T,E>& arr)\
    {\
        return sum_prod<Array<T,E> >::CUM_NAME(NAME)(arr);\
    }\
    \
    template<typename T,typename E>\
    inline typename sum_prod<Matrix<T,E> >::type NAME(const Matrix<T,E>& mat, Integer axis)\
    {\
        return sum_prod<Matrix<T,E> >::NAME(mat, axis);\
    }\
    template<typename T,typename E>\
    inline typename sum_prod<Matrix<T,E> >::cum_type CUM_NAME(NAME)(const Matrix<T,E>& mat, Integer axis)\
    {\
        return sum_prod<Matrix<T,E> >::CUM_NAME(NAME)(mat, axis);\
    }

DEF_SUMPROD(sum)
DEF_SUMPROD(prod)

template<typename T, typename E>
struct sum_prod<Array<T,E> >
{
    typedef T type;
    typedef Array<T> cum_type;

    #define DEF_SUMPROD_ARRAY(NAME, INIT, OP)\
    static T NAME(const Array<T,E> & arr)\
    {\
        T ss= INIT;\
        for(typename Array<T,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
        {\
            ss OP (*it);\
        }\
        return ss;\
    }\
    static Array<T> CUM_NAME(NAME)(const Array<T,E> & arr)\
    {\
        Array<T> res(arr.size(), true);\
        res.unnamed = true; \
        typename Array<T>::iterator jt=res.begin();\
        \
        T ss = INIT;\
        for(typename Array<T,E>::const_iterator it=arr.begin(); it != arr.end(); ++it, ++jt)\
        {\
            ss OP (*it);\
            *jt = ss;\
        }\
        return res;\
    }

    DEF_SUMPROD_ARRAY(sum,0,+=)
    DEF_SUMPROD_ARRAY(prod,1,*=)
};
template<typename E>
struct sum_prod<Array<bool,E> >
{
    typedef Integer type;
    typedef Array<Integer> cum_type;

    #define DEF_SUMPROD_BOOL_ARRAY(NAME, INIT, OP)\
    static Integer NAME(const Array<bool,E> & arr)\
    {\
        Integer ss = INIT;\
        for(typename Array<bool,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
        {\
            ss OP (*it);\
        }\
        return ss;\
    }\
    static Array<Integer> CUM_NAME(NAME)(const Array<bool,E> & arr)\
    {\
        Array<Integer> res(arr.size(),true);\
        res.unnamed = true; \
        typename Array<Integer>::iterator jt=res.begin();\
        \
        Integer ss = INIT;\
        for(typename Array<bool,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
        {\
            ss OP (*it);\
            *jt = ss;\
        }\
        return res;\
    }

    DEF_SUMPROD_BOOL_ARRAY(sum,0,+=)
    DEF_SUMPROD_BOOL_ARRAY(prod,1,*=)
};
template<typename T, typename E>
struct sum_prod<Matrix<T,E> >
{
    typedef Array<T> type;
    typedef Matrix<T> cum_type;

    #define DEF_SUMPROD_MATRIX(NAME, INIT, OP)\
    static Array<T> NAME(const Matrix<T,E> & mat, Integer axis)\
    {\
        Array<T> res(axis==0?mat.nrows:mat.ncols);\
        res.unnamed = true; \
        typename Array<T>::iterator jt=res.begin();\
        \
        if(axis==0)\
        {\
            for(Integer ii=0; ii < mat.ncols; ++ii)\
            {\
                res[ii]=_numcpp::NAME(mat.column(ii));\
            }\
        }\
        else\
        {\
            for(Integer ii=0; ii < mat.nrows; ++ii)\
            {\
                res[ii]=_numcpp::NAME(mat.row(ii));\
            }\
        }\
        return res;\
    }\
    static Matrix<T> CUM_NAME(NAME)(const Matrix<T,E> & mat, Integer axis)\
    {\
        Matrix<T> res(mat.nrows, mat.ncols,true);\
        res.unnamed = true; \
        if(axis==0)\
        {\
            for(Integer ii=0; ii < mat.ncols; ++ii)\
            {\
                res.column(ii)=_numcpp::CUM_NAME(NAME)(mat.column(ii));\
            }\
        }\
        else\
        {\
            for(Integer ii=0; ii < mat.nrows; ++ii)\
            {\
                res.row(ii)=_numcpp::CUM_NAME(NAME)(mat.row(ii));\
            }\
        }\
        return res;\
    }

    DEF_SUMPROD_MATRIX(sum,0,+=)
    DEF_SUMPROD_MATRIX(prod,1,*=)
};
template<typename E>
struct sum_prod<Matrix<bool,E> >
{
    typedef Array<Integer> type;
    typedef Matrix<Integer> cum_type;

    #define DEF_SUMPROD_BOOL_MATRIX(NAME, INIT, OP)\
    static Array<Integer> NAME(const Matrix<bool,E> & mat, Integer axis)\
    {\
        Array<Integer> res(axis==0?mat.nrows:mat.ncols, true);\
        res.unnamed = true; \
        typename Array<bool>::iterator jt=res.begin();\
        \
        if(axis==0)\
        {\
            for(Integer ii=0; ii < mat.ncols; ++ii)\
            {\
                res[ii]=NAME(mat.column(ii));\
            }\
        }\
        else\
        {\
            for(Integer ii=0; ii < mat.nrows; ++ii)\
            {\
                res[ii]=NAME(mat.row(ii));\
            }\
        }\
        return res;\
    }\
    static Matrix<Integer> CUM_NAME(NAME)(const Matrix<bool,E> & mat, Integer axis)\
    {\
        Matrix<Integer> res(mat.nrows, mat.ncols,true);\
        res.unnamed = true; \
        if(axis==0)\
        {\
            for(Integer ii=0; ii < mat.ncols; ++ii)\
            {\
                res.column(ii)=CUM_NAME(NAME)(mat.column(ii));\
            }\
        }\
        else\
        {\
            for(Integer ii=0; ii < mat.nrows; ++ii)\
            {\
                res.row(ii)=CUM_NAME(NAME)(mat.row(ii));\
            }\
        }\
        return res;\
    };

    DEF_SUMPROD_BOOL_MATRIX(sum,0,+=)
    DEF_SUMPROD_BOOL_MATRIX(prod,1,*=)
};

//#define DEF_SUMPROD(NAME, INIT, OP)\
//        template<typename T, typename E>\
//        T NAME(const Array<T,E> & arr)\
//        {\
//            T ss= INIT;\
//            for(typename Array<T,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
//            {\
//                ss OP (*it);\
//            }\
//            return ss;\
//        }\
//        \
//        template<typename E>\
//        Integer NAME(const Array<bool,E> & arr)\
//        {\
//            Integer ss = INIT;\
//            for(typename Array<bool,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
//            {\
//                ss OP (*it);\
//            }\
//            return ss;\
//        }\
//        \
//        template<typename T, typename E>\
//        Array<T> NAME(const Matrix<T,E> & mat, Integer axis)\
//        {\
//            Array<T> res(axis==0?mat.nrows:mat.ncols);\
//            res.unnamed = true; \
//            typename Array<T>::iterator jt=res.begin();\
//            \
//            if(axis==0)\
//            {\
//                for(Integer ii=0; ii < mat.ncols; ++ii)\
//                {\
//                    res[ii]=NAME(mat.column(ii));\
//                }\
//            }\
//            else\
//            {\
//                for(Integer ii=0; ii < mat.nrows; ++ii)\
//                {\
//                    res[ii]=NAME(mat.row(ii));\
//                }\
//            }\
//            return res;\
//        }\
//        \
//        template<typename E>\
//        Array<bool> NAME(const Matrix<bool,E> & mat, Integer axis)\
//        {\
//            Array<bool> res(axis==0?mat.nrows:mat.ncols, true);\
//            res.unnamed = true; \
//            typename Array<bool>::iterator jt=res.begin();\
//            \
//            if(axis==0)\
//            {\
//                for(Integer ii=0; ii < mat.ncols; ++ii)\
//                {\
//                    res[ii]=NAME(mat.column(ii));\
//                }\
//            }\
//            else\
//            {\
//                for(Integer ii=0; ii < mat.nrows; ++ii)\
//                {\
//                    res[ii]=NAME(mat.row(ii));\
//                }\
//            }\
//            return res;\
//        }\
//        template<typename T, typename E>\
//        Array<T> CUM_NAME(NAME)(const Array<T,E> & arr)\
//        {\
//            Array<T> res(arr.size(), true);\
//            res.unnamed = true; \
//            typename Array<T>::iterator jt=res.begin();\
//            \
//            T ss = INIT;\
//            for(typename Array<T,E>::const_iterator it=arr.begin(); it != arr.end(); ++it, ++jt)\
//            {\
//                ss OP (*it);\
//                *jt = ss;\
//            }\
//            return res;\
//        }\
//        \
//        template<typename E>\
//        Array<Integer> CUM_NAME(NAME)(const Array<bool,E> & arr)\
//        {\
//            Array<Integer> res(arr.size(),true);\
//            res.unnamed = true; \
//            typename Array<Integer>::iterator jt=res.begin();\
//            \
//            Integer ss = INIT;\
//            for(typename Array<bool,E>::const_iterator it=arr.begin(); it != arr.end(); ++it)\
//            {\
//                ss OP (*it);\
//                *jt = ss;\
//            }\
//            return res;\
//        }\
//        template<typename T, typename E>\
//        Matrix<T> CUM_NAME(NAME)(const Matrix<T,E> & mat, Integer axis)\
//        {\
//            Matrix<T> res(mat.nrows, mat.ncols,true);\
//            res.unnamed = true; \
//            if(axis==0)\
//            {\
//                for(Integer ii=0; ii < mat.ncols; ++ii)\
//                {\
//                    res.column(ii)=CUM_NAME(NAME)(mat.column(ii));\
//                }\
//            }\
//            else\
//            {\
//                for(Integer ii=0; ii < mat.nrows; ++ii)\
//                {\
//                    res.row(ii)=CUM_NAME(NAME)(mat.row(ii));\
//                }\
//            }\
//            return res;\
//        }\
//        \
//        template<typename E>\
//        Matrix<Integer> CUM_NAME(NAME)(const Matrix<bool,E> & mat, Integer axis)\
//        {\
//            Matrix<Integer> res(mat.nrows, mat.ncols,true);\
//            res.unnamed = true; \
//            if(axis==0)\
//            {\
//                for(Integer ii=0; ii < mat.ncols; ++ii)\
//                {\
//                    res.column(ii)=CUM_NAME(NAME)(mat.column(ii));\
//                }\
//            }\
//            else\
//            {\
//                for(Integer ii=0; ii < mat.nrows; ++ii)\
//                {\
//                    res.row(ii)=CUM_NAME(NAME)(mat.row(ii));\
//                }\
//            }\
//            return res;\
//        };\
//
//
//DEF_SUMPROD(sum, 0, +=);
//DEF_SUMPROD(prod, 1, *=);
//
template<typename T, typename E>
inline T sumSq(const Array<T,E> & arr)
{
    return sum(arr*arr);
}


//********************************************
//***  norm
//********************************************

template<Integer ord, typename T, typename E>
inline T norm(const Array<T,E> & arr)
{
    return std::pow(sum(pow(abs(arr), T(ord))), T(1.0/ord));
}

template<typename T, typename E>
inline T norm1(const Array<T,E> & arr)
{
    return sum(abs(arr));
};

template<typename T, typename E>
inline T norm2(const Array<T,E> & arr)
{
    return std::sqrt(sumSq(arr));
};


//********************************************
//***  range
//********************************************

inline Range range(Integer start, Integer stop, Integer step=1)
{
    return Range(_Range(start,stop,step));
}

inline Range range(Integer stop)
{
    return Range(_Range(0,stop,1));
}

inline Range R(Integer start, Integer stop, Integer step=1)
{
    return Range(_Range(start,stop,step));
}

inline Range R(Integer start=0)
{
    return Range(_Range(start, _N, 1));
}


//********************************************
//***  linspace
//********************************************

template<typename T>
inline Array<T, _Linspace<T> > linspace(const T & start, const T & stop, Integer nsteps)
{
    return Array<T,_Linspace<T> >(_Linspace<T>(start,stop, nsteps));
}

template<typename T, typename E1>
inline DMatrix<T> diag(const Array<T,E1> & vec)
{
    return DMatrix<T>(vec);
}

template<typename T, typename E1>
inline Array<T, Slice<T,E1> > diag(const Matrix<T,E1> & mat, Integer offset = 0)
{
    return Array<T, Slice<T,E1> > ( 
                Slice<T,E1>( mat.expr, 
                            (offset < 0 ? -offset : offset*mat.nrows), 
                            (offset < 0 ? -offset : offset*mat.nrows)
                            + (mat.nrows+1)*std::min(
                                mat.nrows+std::min(offset,0), 
                                mat.ncols-std::max(offset,0) 
                                ),
                            mat.nrows+1 )  );
}

template<typename T, typename E>
Array<Integer> pivot2index(const Array<T,E> & pivot)
{
    Array<Integer> res(pivot.size());
    res = R(0, pivot.size());
    Integer idx=0;
    for(typename Array<T,E>::const_iterator it=pivot.begin(); it != pivot.end(); ++it, ++idx)
        if(idx != *it) 
        {
           std::swap(res[idx], res[(Integer)(*it)]);
        }
    return res;
}

template<typename E1>
Array<Integer> mask2index(const Array<bool,E1> & mask)
{
    Array<Integer> res(sum(mask));
    res.unnamed = true;
    Integer idx=0, ii=0;
    for(typename Array<bool,E1>::const_iterator it=mask.begin(); it != mask.end(); ++it, ++idx)
        if(*it) res[ii++] = idx;
    return res;
}

#define DEF_LIKE(NAME) NAME ## _like
#define DEF_ZEROS_ONES(NAME, IVAL)\
        template<typename T>\
        inline Array<T> NAME(Integer _len)\
        {\
            return Array<T>(_len, T(IVAL));\
        }\
        \
        template<typename T, typename E1>\
        inline Array<T> DEF_LIKE(NAME)(const Array<T,E1> & arr)\
        {\
            return Array<T>(arr.size(), T(IVAL));\
        }\
        \
        template<typename T>\
        inline Matrix<T> NAME(Integer _rows, Integer _cols)\
        {\
            return Matrix<T>(_rows, _cols, T(IVAL));\
        }\
        \
        template<typename T, typename E1>\
        inline Matrix<T> DEF_LIKE(NAME)(const Matrix<T,E1> & arr)\
        {\
            return Matrix<T>(arr.nrows, arr.ncols, T(IVAL));\
        }
DEF_ZEROS_ONES(zeros, 0);
DEF_ZEROS_ONES(ones,  1);

#define EXTEND_TO_MATRIX(T1, NAME) \
        template<typename T, typename E1>\
        Array<T1> NAME(const Matrix<T,E1> & mat, Integer axis)\
        {\
            Integer L = (axis==0 ? mat.ncols: mat.nrows);\
            Array<T1> res(L);\
            res.unnamed = true; \
            if(axis==0){\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.column(ii));\
            }\
            else\
            {\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.row(ii));\
            }\
            return res;\
        }

#define EXTEND_TO_MATRIX2(T1, NAME) \
        template<typename T, typename E1>\
        Matrix<T1> NAME(const Matrix<T,E1> & mat, Integer axis)\
        {\
            Integer L = (axis==0 ? mat.ncols: mat.nrows);\
            Matrix<T1> res(mat.nrows, mat.ncols);\
            res.unnamed = true; \
            if(axis==0){\
                for(Integer ii=0; ii < L; ++ii)\
                    res.column(ii) = NAME(mat.column(ii));\
            }\
            else\
            {\
                for(Integer ii=0; ii < L; ++ii)\
                    res.row(ii) = NAME(mat.row(ii));\
            }\
            return res;\
        }

#define EXTEND_TO_MATRIX_ARRAY(T1, NAME) \
        template<typename T, typename E1, typename E2>\
        Array<T1> NAME(const Matrix<T,E1> & mat, Integer axis, const Array<T,E2> & weight)\
        {\
            Integer L = (axis==0 ? mat.ncols: mat.nrows);\
            Array<T1> res(L);\
            res.unnamed = true; \
            if(axis==0){\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.column(ii), weight);\
            }\
            else\
            {\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.row(ii), weight);\
            }\
            return res;\
        }
#define EXTEND_TO_MATRIX_INTEGER(T1, NAME) \
        template<typename T, typename E1, typename E2>\
        Array<T1> NAME(const Matrix<T,E1> & mat, Integer axis, Integer bias)\
        {\
            Integer L = (axis==0 ? mat.ncols: mat.nrows);\
            Array<T1> res(L);\
            res.unnamed = true; \
            if(axis==0){\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.column(ii), bias);\
            }\
            else\
            {\
                for(Integer ii=0; ii < L; ++ii)\
                    res[ii] = NAME(mat.row(ii), bias);\
            }\
            return res;\
        }
template<typename T, typename E1>
bool all(const Array<T,E1> & arr)
{
    const T cZERO(0);

    for(typename Array<T,E1>::const_iterator it = arr.begin(); it != arr.end(); ++it)
        if(*it == cZERO) return false;
    return true;
}

template<typename T, typename E1>
bool any(const Array<T,E1> & arr)
{
    const T cZERO(0);

    for(typename Array<T,E1>::const_iterator it = arr.begin(); it != arr.end(); ++it)
        if(*it != cZERO) return true;
    return false;
}


template<typename T, typename E1>
T max(const Array<T,E1> & arr)
{
    assert(arr.size() > 0);
    typename Array<T,E1>::const_iterator it = arr.begin();
    T val = *it; ++it;
    for(; it != arr.end(); ++it)
        if(*it > val)
        {
            val = *it;
        }
    return val;
}

template<typename T, typename E1>
T min(const Array<T,E1> & arr)
{
    assert(arr.size() > 0);
    typename Array<T,E1>::const_iterator it = arr.begin();
    T val = *it; ++it;
    for(; it != arr.end(); ++it)
        if(*it < val)
        {
            val = *it;
        }
    return val;
}

template<typename T, typename E1>
Integer argmax(const Array<T,E1> & arr)
{
    assert(arr.size() > 0);
    typename Array<T,E1>::const_iterator it = arr.begin();
    Integer idx=0;
    T val = *it; ++it;
    for(Integer ii=1; it != arr.end(); ++it,++ii)
        if(*it > val)
        {
            val = *it;
            idx = ii;
        }
    return idx;
}

template<typename T, typename E1>
Integer argmin(const Array<T,E1> & arr)
{
    assert(arr.size() > 0);
    
    typename Array<T,E1>::const_iterator it = arr.begin();
    Integer idx=0;
    T val = *it; ++it;
    for(Integer ii=1; it != arr.end(); ++it,++ii)
        if(*it < val)
        {
            val = *it;
            idx = ii;
        }
    return idx;
}

EXTEND_TO_MATRIX(bool, all);
EXTEND_TO_MATRIX(bool, any);
EXTEND_TO_MATRIX(T, max);
EXTEND_TO_MATRIX(T, min);
EXTEND_TO_MATRIX(Integer, argmax);
EXTEND_TO_MATRIX(Integer, argmin);

//***************************************************
template<typename T1, typename E1, typename T2, typename E2>
inline Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> > zip(const Array<T1,E1> & arr1, const Array<T2,E2> & arr2)
{
    return Array<tuple<T1,T2>, ZIP<T1,E1,T2,E2> >(
                ZIP<T1,E1,T2,E2> (arr1, arr2) );
}

template<Integer, typename T1, typename T2> struct itemgetter;

template<typename T1, typename T2>
struct itemgetter<0,T1,T2>
{
    typedef T1 type;

    type & operator ()(tuple<T1,T2> & obj) const
    {
        return obj.get<0>();
    }

    const type & operator ()(const tuple<T1,T2> & obj) const
    {
        return obj.get<0>();
    }
};


template<typename T1, typename T2>
struct itemgetter<1,T1,T2>
{
    typedef T2 type;

    type & operator ()(tuple<T1,T2> & obj) const
    {
        return obj.get<1>();
    }

    const type & operator ()(const tuple<T1,T2> & obj) const
    {
        return obj.get<1>();
    }
};

template<typename T>
struct Identity
{
    typedef T type; 

    T & operator()(T & obj) const { return obj;}
    const T & operator()(const T & obj) const { return obj;}
};

template<typename K>
struct KeyComp
{
    KeyComp(const K & _key, bool _reverse = false): key(_key), reverse(_reverse) {}
    template<typename T>
    bool operator()(const T & left, const T & right) { return (key(left) < key(right)) ^ reverse;}
private:
    K key;
    bool reverse;
};


template<typename T, typename E> 
Array<T> sorted(const Array<T,E> & arr, bool reverse=false)
{
    Array<T> res(arr.size());
    res.unnamed = true;
    res = arr;
    std::sort(res.begin(), res.end(), KeyComp<Identity<T> >(Identity<T>(), reverse));
    return res;
}

template<typename T, typename E, typename K>
Array<T> sorted(const Array<T,E> & arr, const K & key, bool reverse=false)
{
    Array<T> res(arr.size());
    res.unnamed = true;
    res = arr;
    std::sort(res.begin(), res.end(), KeyComp<K>(key, reverse));
    return res;
}

template<typename T, typename E1>
Array<Integer> argsort(const Array<T,E1> & arr, bool reverse=false)
{
    Array<tuple<T, Integer> > tmp(arr.size());
    tmp = zip(arr, range(arr.size()));
    std::sort(tmp.begin(), tmp.end(), KeyComp<itemgetter<0,T,Integer> >(itemgetter<0,T,Integer>(), reverse));
    Array<Integer> res(arr.size());
    res.unnamed = true;
    std::transform(tmp.begin(), tmp.end(), res.begin(), itemgetter<1,T,Integer>());
    return res;
}

template<typename T, typename E1>
Matrix<Integer> argsort(const Matrix<T,E1> & mat, Integer axis, bool reverse=false)
{
    Integer L = (axis==0 ? mat.ncols: mat.nrows);
    Matrix<Integer> res(mat.nrows, mat.ncols);
    res.unnamed = true;
    if(axis==0){
        for(Integer ii=0; ii < L; ++ii)
            res.column(ii) = argsort(mat.column(ii), reverse);
    }
    else
    {
        for(Integer ii=0; ii < L; ++ii)
            res.row(ii) = argsort(mat.row(ii), reverse);
    }
    return res;
}


template<typename T, typename E>
Array<T, REPEAT<T,E> > repeat(const Array<T,E> & arr, Integer repeats)
{
    assert(repeats > 0);
    return Array<T, REPEAT<T,E> > ( REPEAT<T,E>(arr.expr, repeats) );
}

template<typename T, typename E>
Matrix<T, MREPEAT<T, Matrix<T,E> > > repeat(const Matrix<T,E> & mat, Integer repeats, Integer axis)
{
    assert(repeats > 0);
    return Matrix<T, MREPEAT<T, Matrix<T,E> > > (
                     MREPEAT<T, Matrix<T,E> > ( mat, repeats, axis));
    
}

//***************************************************

template<typename T, typename E1>
inline Matrix<T, UTVIEW<T,Matrix<T,E1> > > triu(const Matrix<T,E1> & mat, Integer offset=0)
{
    return Matrix<T, UTVIEW<T,Matrix<T,E1> > > ( UTVIEW<T,Matrix<T,E1> >(mat, offset) );
}

template<typename T, typename E1>
inline Matrix<T, LTVIEW<T,Matrix<T,E1> > > tril(const Matrix<T,E1> & mat, Integer offset=0)
{
    return Matrix<T, LTVIEW<T,Matrix<T,E1> > > ( LTVIEW<T,Matrix<T,E1> >(mat, offset) );
}

template<typename T>
inline DMatrix<T> eye(Integer _rows, Integer _cols, Integer _offset=0)
{
    return DMatrix<T>(_rows, _cols, 1);
}

template<typename T>
inline DMatrix<T> identity(Integer _dim)
{
    return DMatrix<T>(_dim, _dim, 1);
}

template<typename T, typename E>
inline Matrix<T,E> reshape(const Matrix<T,E> & mat, Integer _nrows, Integer _ncols)
{
    return Matrix<T,E>(_nrows, _ncols, mat.asarray());
}

//******** asarray
template<typename T, typename E1>
    inline Array<T,E1> & asarray(Array<T,E1> & obj) { return obj;}
template<typename T, typename E1>
    inline Array<T,E1> & asarray(Matrix<T,E1> & obj) { return *static_cast<Array<T,E1> *>(&obj);}
template<typename T>
    inline Array<T,_SArray<T> > asarray(T & val) { return Array<T,_SArray<T> >(val);}

template<typename T, typename E1>
    inline const Array<T,E1> & asarray(const Array<T,E1> & obj) { return obj;}
template<typename T, typename E1>
    inline const Array<T,E1> & asarray(const Matrix<T,E1> & obj) { return *static_cast<const Array<T,E1> *>(&obj);}
template<typename T>
    inline const Array<T,_SArray<T> > asarray(const T & val) { return Array<T,_SArray<T> >(val);}

//******** copy array/matrix
template<typename T, typename E1>
    inline Array<T> copy(const Array<T,E1>& obj) { Array<T> arr(obj); arr.dup(); return arr;}

template<typename T, typename E1>
    inline Matrix<T> copy(const Matrix<T,E1>& obj) { Matrix<T> mat(obj); mat.dup(); return mat;}

//******** compress matrix
template<Integer axis, typename T, typename E1, typename E2> struct CompressRet;
template<typename T, typename E1, typename E2> 
    struct CompressRet<0,T,E1,E2> { typedef Matrix<T, INDEX2<T, Matrix<T,E2>, MASK2ARRAY<E1>, _Range > > type; };
template<typename T, typename E1, typename E2> 
    struct CompressRet<1,T,E1,E2> { typedef Matrix<T, INDEX2<T, Matrix<T,E2>, _Range, MASK2ARRAY<E1> > > type; };

template<typename T, typename E1, typename E2>
    inline typename CompressRet<0,T,E1,E2>::type compress0 (
    const Array<bool,E1> & cond, Matrix<T,E2> & mat)
{
    return mat(cond,R(0));
}

template<typename T, typename E1, typename E2>
    inline typename CompressRet<1,T,E1,E2>::type compress1 (
    const Array<bool,E1> & cond, Matrix<T,E2> & mat)
{
    return mat(R(0), cond);
}

//******** nonzero
template<typename T, typename E>
Array<tuple<Integer,Integer>, ZIP<Integer,_Array<Integer>, Integer, _Array<Integer> > > 
    nonzero(const Matrix<T,E> & mat)
{
    //return mat.nonzero();
    Matrix<bool> mask(mat != T(0));
    Integer len = sum(mask);

    Array<Integer> arr1(len), arr2(len);

    Integer count=0, rr, cc;
    for(cc=0; cc < mask.ncols; ++cc)
    {
        for(rr=0; rr < mask.nrows; ++rr)
        {
            if(mask(rr,cc)){
                arr1[count]=rr;
                arr2[count]=cc;
                ++count;
            }
        }
    }

    return zip(arr1,arr2);
}

//******** peak-to-peak
template<typename T, typename E1>
inline T ptp(const Array<T,E1> & arr)
{
    return max(arr)-min(arr);
}

template<typename T, typename E1>
inline Array<T> ptp(const Matrix<T,E1> & mat, Integer axis)
{
    return max(mat, axis)-min(mat, axis);
}

//******** put
template<typename T, typename IDX, typename VAL>
void put(const T & obj, const IDX & index, const VAL & values)
{
    assert(index.size() == values.size());
    obj[index] = values;
}

//******** mean/std

template<typename T, typename E>
inline double mean(const Array<T,E> & arr)
{
    return (double)(sum(arr)*1.0/arr.size());
}
EXTEND_TO_MATRIX(double, mean);

template<typename T, typename E, typename E1>
inline double average(const Array<T,E> & arr, const Array<T,E1> & weight)
{
    assert(weight.size()==0 || arr.size() == weight.size());
    return (double)(sum(arr*weight)*1.0/sum(weight));
}
EXTEND_TO_MATRIX_ARRAY(double, average);

template<typename T, typename E>
inline double var(const Array<T,E> & arr, Integer ddof=0)
{
    return (sum(arr*arr) - arr.size()*std::pow((double)sum(arr)*1.0/arr.size(), 2.0))/(arr.size()-ddof);
}
EXTEND_TO_MATRIX_INTEGER(double, var);

template<typename T, typename E>
inline double std_(const Array<T,E> & arr, Integer ddof=0)
{
    return std::sqrt(var(arr,ddof));
}
EXTEND_TO_MATRIX_INTEGER(double, std_);

template<typename T, typename E1, typename E2>
inline double cov(const Array<T,E1> & arr1, const Array<T,E2> & arr2, Integer ddof=0)
{
    assert(arr1.size() == arr2.size());
    double mm1, mm2;
    Integer N = arr1.size()-ddof;
    mm1=mean(arr1); mm2=mean(arr2);
    return sum(arr1*arr2)/N-(arr1.size())*1./N*mm1*mm2;
}

template<typename T, typename E>
Matrix<T> cov(const Matrix<T,E> & mat, Integer bias=0, bool corrcoeff=false)
{
    Matrix<T> res(mat.nrows, mat.nrows);
    res.unnamed = true;
    Integer ddof = 1-bias;
    for(Integer ii=0; ii < mat.nrows; ++ii)
    {
        for(Integer jj=ii; jj < mat.nrows; ++jj)
        {
            res(ii,jj)=cov(mat.row(ii), mat.row(jj), ddof);
            res(jj,ii)=res(ii,jj);
        }
    }
    if(corrcoeff)
    {
        for(Integer ii=0; ii < mat.nrows; ++ii)
        {
            for(Integer jj=ii+1; jj < mat.nrows; ++jj)
            {
                res(ii,jj) /= std::sqrt(res(ii,ii))*std::sqrt(res(jj,jj));
                res(jj,ii) = res(ii,jj);
            }
        }
        for(Integer ii=0; ii < mat.nrows; ++ii)
            res(ii,ii) = T(1);
    }
    return res;
}

template<typename T, typename E>
Matrix<T> corrcoef(const Matrix<T,E> & mat, Integer bias=0)
{
    return cov(mat, bias, true);
}

//******* diff *****
template<typename T, typename E>
Array<T> diff(const Array<T,E> & arr)
{
    return Array<T>(arr[R(1)]-arr[R(0,-1)], unnamed_lvalue());
}

template<typename T, typename E>
Array<T> ratio(const Array<T,E> & arr)
{
    return Array<T>(arr[R(1)]/arr[R(0,-1)], unnamed_lvalue());
}


#define DEF_DIFF_RATIO(NAME)\
    template<typename T, typename E>\
    Matrix<T> NAME(const Matrix<T,E> & mat, Integer axis)\
    {\
        Matrix<T> res(mat.nrows+(axis==0 ? -1: 0), mat.ncols+(axis==1 ? -1: 0));\
        res.unnamed = true; \
        if(axis==0)\
        {\
            for(Integer jj=0; jj < mat.ncols; ++jj)\
            {\
                res.column(jj) = NAME(mat.column(jj));\
            }\
        }\
        else\
        {\
            for(Integer ii=0; ii < mat.nrows; ++ii)\
            {\
                res.row(ii) = NAME(mat.row(ii));\
            }\
        }\
        return res;\
    }

DEF_DIFF_RATIO(diff);
DEF_DIFF_RATIO(ratio);

//******* histogram *****
template<typename T, typename E1, typename E2>
Array<double> histogram(const Array<T,E1> & arr, const Array<T,E2> & bins, bool density=false)
{
    Array<double> res(bins.size()-1);
    res.unnamed = true;

    for(Integer ii=0; ii < arr.size(); ++ii)
    {
        Integer offset;

        typename Array<T,E2>::const_iterator it=std::upper_bound(bins.begin(), bins.end(), arr[ii]);
        offset = it - bins.begin() - 1;
        if(offset >= res.size() && arr[ii] == bins[offset]) --offset;
        if(offset >= 0 && offset < res.size()) ++res[offset];
    }
    if(density)  
    {
        double ss = sum(res);
        res /= (bins[R(1)]-bins[R(0,-1)])*ss;
    }

    return res;
}

template<typename T, typename E1>
Array<double> histogram(const Array<T,E1> & arr, Integer bins, bool density=false)
{
    return histogram(arr, linspace(min(arr), max(arr), bins+1), density);
}

//******* convolution ******
template<ENUM_CONV CT, typename T, typename E1, typename E2>
inline Matrix<T, CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT> > convolve(const Matrix<T,E1> & arr1, const Matrix<T,E2> & arr2)
{
    return Matrix<T, CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT> >(
            CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT>(arr1, arr2));
}

template<ENUM_CONV CT, typename T, typename E1, typename E2>
inline Matrix<T, CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT> > convolve(const Matrix<T,E1> & arr1, const Array<T,E2> & arr2)
{
    return convolve<CT>(arr1, arr2.asColMat());
}

template<ENUM_CONV CT, typename T, typename E1, typename E2>
inline Array<T, CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT> > convolve(const Array<T,E1> & arr1, const Array<T,E2> & arr2)
{
    return Array<T, CONVOLVE<T,Matrix<T,E1>,Matrix<T,E2>,CT> >(
            convolve<CT>(arr1.asRowMat(), arr2.asColMat()));
}

//******* matrix vstack ******
template<typename T, typename E1, typename E2>
inline Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > vstack(const Matrix<T,E1> & mat1, const Matrix<T,E2> & mat2)
{
    return Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     VSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1, mat2));
}

template<typename T, typename E1, typename E2>
inline Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > vstack(const Matrix<T,E1> & mat1, const Array<T,E2> & mat2)
{
    return Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     VSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1, mat2.asRowMat()));
}

template<typename T, typename E1, typename E2>
inline Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > vstack(const Array<T,E1> & mat1, const Matrix<T,E2> & mat2)
{
    return Matrix<T, VSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     VSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1.asRowMat(), mat2));
}

//******* matrix hstack ******
template<typename T, typename E1, typename E2>
inline Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > hstack(const Matrix<T,E1> & mat1, const Matrix<T,E2> & mat2)
{
    return Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     HSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1, mat2));
}

template<typename T, typename E1, typename E2>
inline Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > hstack(const Matrix<T,E1> & mat1, const Array<T,E2> & mat2)
{
    return Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     HSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1, mat2.asRowMat()));
}

template<typename T, typename E1, typename E2>
inline Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > > hstack(const Array<T,E1> & mat1, const Matrix<T,E2> & mat2)
{
    return Matrix<T, HSTACK<T,Matrix<T,E1>,Matrix<T,E2> > >(
                     HSTACK<T,Matrix<T,E1>,Matrix<T,E2> >(mat1.asRowMat(), mat2));
}


//******* matrix inverse ******
template<typename T, typename E>
Matrix<T> inv(const Matrix<T,E> & mat)
{
    assert(mat.nrows == mat.ncols);
    Matrix<T> res(mat.nrows, mat.ncols);
    res.unnamed = true;
    long rcode = inv_(mat, res);
    return res;
}

//******* complex related *******

template<typename T>
inline std::ostream & SetWComplex(std::ostream & os)
{
    std::ios::fmtflags flags = os.flags();
    os << std::setw(18);
    os.flags(flags);
    return os;
}

template<>
inline std::ostream & SetWComplex<Integer>(std::ostream & os)
{
    return os;
}

template<typename T>
std::ostream & operator << (std::ostream & os, const std::complex<T> & cmplx)
{
    std::stringstream sstr(std::stringstream::out);

    if(cmplx.real())
    {
        sstr << cmplx.real() << std::showpos;
    }
    if(cmplx.imag()){
        if(cmplx.real()) sstr << std::left << std::setw(0);
        sstr << cmplx.imag() << "j";
    }
    SetWComplex<T>(os) << sstr.str();
    return os;
}

