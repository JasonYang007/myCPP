#ifndef __THREAD_H__
#define __THREAD_H__

#include "class_traits.h"

extern "C" 
{
#ifdef __GNUC__
    #include <pthread.h>
#endif
}

namespace thread
{

struct callable
{
    virtual void operator()() =0;
};

template<typename F>
struct Callable: callable
{
    Callable(F _f): f(_f) {}
    void operator()() { f(); }

    F f;
};


} /* end of namespace */

#endif /* __THREAD_H__ */
