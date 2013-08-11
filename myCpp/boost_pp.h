#ifndef __BOOST_PP_H__
#define __BOOST_PP_H__

#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition.hpp>

#define TAG_COMMA_class_Tn(Z,N,_)	,class T ## N
#define TAG_class_Tn(Z,N,_)			BOOST_PP_COMMA_IF(N) class T ## N
#define TAG_class_An(Z,N,_)			BOOST_PP_COMMA_IF(N) class A ## N
#define TAG_Tn(Z,N,_)				BOOST_PP_COMMA_IF(N) T ## N
#define TAG_Tn_ref(Z,N,_)			BOOST_PP_COMMA_IF(N) T ## N &
#define TAG_An(Z,N,_)				BOOST_PP_COMMA_IF(N) A ## N
#define TAG_an(Z,N,_)				BOOST_PP_COMMA_IF(N) a ## N
#define TAG_Tn_An(Z,N,_)			BOOST_PP_COMMA_IF(N) T ## N  a ## N
#define TAG_Tn_uAn(Z,N,_)			BOOST_PP_COMMA_IF(N) T ## N  _a ## N
#define TAG_Tn_ref_An(Z,N,_)        BOOST_PP_COMMA_IF(N) T ## N  & a ## N
#define TAG_const_Tn_ref_An(Z,N,_)   BOOST_PP_COMMA_IF(N) const T ## N  & a ## N
#define TAG_An_uAn(Z,N,_)           BOOST_PP_COMMA_IF(N) A ## N  _a ## N
#define TAG_An_xn(Z,N,_)            BOOST_PP_COMMA_IF(N) A ## N  x ## N

#define TAG_Tn_ref_Xn(Z,N,_)        BOOST_PP_COMMA_IF(N) T ## N  & x ## N
#define TAG_Xn(Z,N,_)				BOOST_PP_COMMA_IF(N) x ## N
#define TAG_Xn_An(Z,N,_)            BOOST_PP_COMMA_IF(N) x ## N(a ## N)
#define TAG_An_uAn_init(Z,N,_)      BOOST_PP_COMMA_IF(N) a ## N(_a ## N)
#define TAG_Tn_Xn(Z,N,_)            T ## N x ## N ;
#define TAG_An_an(Z,N,_)            A ## N a ## N ;

#define  MYPP_ENUM(N, UNIT_FUNC, _)  BOOST_PP_REPEAT(N,UNIT_FUNC,_)
#define  MYPP_ENUM0(N, UNIT_FUNC, _) BOOST_PP_REPEAT(BOOST_PP_INC(N),UNIT_FUNC,_)
#define MYPP_NL  /*
*/
#define NPARAMS      9

#define FUNC_TRAITS_ARG(z,N,_) \
    typedef typename func_traits<F>::BOOST_PP_CAT(arg,BOOST_PP_INC(N))_type BOOST_PP_CAT(arg,BOOST_PP_INC(N))_type;

#define TAG_ARGTYPE_Xn(z,N,_)\
    BOOST_PP_COMMA_IF(N) BOOST_PP_CAT(arg,BOOST_PP_INC(N))_type BOOST_PP_CAT(x,N)

#endif
