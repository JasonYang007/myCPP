#include "lambda.h"


namespace lambda
{

#define DEF_PLACE_HOLDER(z,n,_)  PlaceHolder<BOOST_PP_INC(n)> BOOST_PP_CAT(_,BOOST_PP_INC(n)); 

BOOST_PP_REPEAT(NPARAMS, DEF_PLACE_HOLDER, _)

}