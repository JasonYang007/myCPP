#include "numcpp.h"

namespace numcpp
{

double randU(void) { return rand()*1.0/RAND_MAX;}

static boost::math::normal normDist;
#define rndEps 1.0E-10
double randN(void) { return boost::math::quantile(normDist, (1.0-2*rndEps)*randU()+rndEps); }

LRValue unnamed_lvalue(void) { return LRValue(true,true); }


} /* end of namespace */

