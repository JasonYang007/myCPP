#ifndef __BIG_NUM_H__
#define __BIG_NUM_H__

template<unsigned n>
struct int_traits
{
    enum {nbits=1+int_traits<(n >> 1)>::nbits, lmask=(1 << (nbits-1)), rmask=1-lmask};
};

template<>
struct int_traits<0>
{
    enum {nbits=0, lmask=0, rmask=0};
};

#endif
