#include <vector>
#include <climits>

#include "BigNum.h"

typedef unsigned short Integer;
#define NBITS sizeof(Integer)
#define LMASK (1 << (NBITS-1))
#define RMASK (LMASK-1)

struct BigNum
{
    BigNum(long n)
    {
        
    }
    std::vector<Integer> vec;
};

