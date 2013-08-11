
#ifndef __NUMCPP_H__
#define __NUMCPP_H__

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4996 4180)
#endif

#include <ostream>
#include <sstream>
#include <time.h>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iterator>
#include <climits>
#include <boost/math/distributions/normal.hpp>
#define _USE_MATH_DEFINES
#include <cmath>
#include <omp.h>

#include "Import_type_traits.h"
#include "Tuples.h"
#include "Shared_Array.h"
using memory::shared_ptr;
using memory::shared_array;
#include "Thread.h"

#ifdef __GNUC__
    //#include <tr1/memory>
    //using std::tr1::shared_ptr;
#elif defined _MSC_VER

    //#include <memory>
    //using std::shared_ptr;

    #undef copysign
    #define copysign _copysign
#endif


//********************** Expression Template **********************

namespace numcpp
{

struct using_mt
{
    using_mt(unsigned n): curr_num_threads(n), old_num_threads(omp_get_num_threads())
    {
        if(curr_num_threads == 0)  curr_num_threads = std::max(1, (int)(omp_get_num_procs()/2));

        omp_set_num_threads(curr_num_threads);
    }

    ~using_mt()
    {
        omp_set_num_threads(old_num_threads);
    }

    unsigned get_num_threads(void) { return curr_num_threads;}

private:
    unsigned curr_num_threads;
    unsigned old_num_threads;
};

namespace _numcpp=numcpp;

#include "numcpp_gen.h"

template<typename ASS_FUNC>
struct Array_Assign
{
    Array_Assign(void): func(ASS_FUNC()) {}
    
    template<typename T, typename E, typename S, typename E1>
    void realloc(Array<T,E> & arr, Array<S,E1> const & other)
    {
        T * ptr = arr.get_sptr().get();
        shared_array<T> new_ptr=arr.resize(other.size());
        arr.reset(ptr, new_ptr); 
    }

    template<typename T, typename E>
    bool move(Array<T,E> & arr, Array<T,_Array<T> >  const & other)
    {
        T * ptr = arr.get_sptr().get();
        const shared_array<T> new_ptr=other.expr.get_sptr();
        arr.reset(ptr, new_ptr);
        arr.setsize(other.size());
        return true;
    }
    
    template<typename T, typename E, typename S, typename E1>
    bool move(Array<T,E> & arr, Array<S,E1>  const & other)
    {
        return false;
    }

    template<typename T, typename E, typename S, typename E1>
    Array<T,E> & operator ()(Array<T,E> & arr, Array<S,E1>  const & other)
    {
        if(arr.lvalue && other.lvalue && other.unnamed && other.size() > 0 && func.IsAssign())
        {
            if(move(arr,other)) return arr;
        }
        if(arr.size() != other.size() && other.size() > 0)
        {
            realloc(arr,other);
        }
        bool doCopy = other.Qdo_copy(arr.get_sptr().get());
        if(doCopy)
        {
            Array<T> tmp(arr.size());

            do_assign(ASSIGN<T>(), tmp, other);
            check_dup(arr, other); 

            arr = tmp;
        }
        else
        {
            check_dup(arr, other); 
            do_assign(func, arr, other);
        }
		return arr;  
    }

    template<typename F, typename T, typename E, typename S, typename E1>
    static void do_assign(F func, Array<T,E>& arr, const Array<S,E1>& other)
    {
        Integer len = arr.expr.end()-arr.expr.begin();

        #pragma omp parallel for 
        for(Integer ii=0; ii < len; ++ii)
        {
            func(arr[ii],other[ii]);
        }
    }

    ASS_FUNC func;
};

} /* End of name space */



#endif /* __NUMCPP_H__ */
