#ifndef __GENERIC_FUNC_H__
#define __GENERIC_FUNC_H__

#include <vector>
#include <algorithm>

namespace numcpp
{
    enum ENUM_PWFUNC   { CONST, PWCONST, PWLINEAR };
    enum ENUM_PWPOLICY { TERM_CONST, TERM_LINEAR, CONT_LEFT, CONT_RIGHT};

    template<ENUM_PWFUNC> struct default_policy;
    template<> struct default_policy<CONST>    { static const ENUM_PWPOLICY POLICY = TERM_CONST; };
    template<> struct default_policy<PWCONST>  { static const ENUM_PWPOLICY POLICY = CONT_LEFT;  };
    template<> struct default_policy<PWLINEAR> { static const ENUM_PWPOLICY POLICY = TERM_CONST; };
    template<ENUM_PWPOLICY PLEFT, ENUM_PWPOLICY PRIGHT = PLEFT> struct PWPolicy {};

    template<ENUM_PWFUNC F, ENUM_PWPOLICY PLEFT=default_policy<F>::POLICY, ENUM_PWPOLICY PRIGHT=PLEFT,
             typename R=double, typename T=R> struct PWFunc;

    //*** flat constant
    template<ENUM_PWPOLICY P, typename R, typename T> 
    struct PWFunc<CONST, P, P, R, T>
    {
        PWFunc(const R & _y): y(_y) {}

        R operator () (const T & x) { return y;}

        private:
            R y;
    };

    //*** Piecewise constant
    template<ENUM_PWPOLICY P, typename R, typename T> 
    struct PWFunc<PWCONST, P, P, R, T>
    {
        typedef typename std::iterator_traits<typename std::vector<T>::const_iterator>::difference_type distance_type;

        template<typename Tx, typename Ty >
        PWFunc(const Tx & x, const Ty & y, const T & _epsilon=T()): epsilon(_epsilon)
        {
            xi.resize(x.size());
            yi.resize(y.size());
            std::copy(x.begin(), x.end(), xi.begin());
            std::copy(y.begin(), y.end(), yi.begin());
        }

        R operator () (const T & x)
        {
            return call(x, PWPolicy<P,P>());
        }
        
    private:

        R call(T x, const PWPolicy<CONT_LEFT,CONT_LEFT>)
        {
            x += epsilon;
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx==xi.begin()) return yi[0];
            if(itx==xi.end())   return yi[yi.size()-1];
            --itx;
            
            distance_type step = std::distance(xi.begin(),itx);
            typename std::vector<R>::const_iterator ity = yi.begin();
            std::advance(ity, step);

            if(*itx == x)
                return *ity;
            else
                return *++ity;
        }

        R call(T x, const PWPolicy<CONT_RIGHT,CONT_RIGHT>)
        {
            x -= epsilon;
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx==xi.begin()) return yi[0];
            if(itx==xi.end())   return yi[yi.size()-1];
            --itx;
            
            distance_type step = std::distance(xi.begin(),itx);
            typename std::vector<R>::const_iterator ity = yi.begin();
            std::advance(ity, step);

            return *ity;
        }

        T epsilon;
        std::vector<T> xi;
        std::vector<R> yi;
    };

    //*** Piecewise linear
    template<ENUM_PWPOLICY PLEFT, ENUM_PWPOLICY PRIGHT, typename R, typename T> 
    struct PWFunc<PWLINEAR, PLEFT, PRIGHT, R, T>
    {
        typedef typename std::iterator_traits<typename std::vector<T>::const_iterator>::difference_type distance_type;

        template<typename Tx, typename Ty>
        PWFunc(const Tx & x, const Ty & y)
        {
            xi.resize(x.size());
            std::copy(x.begin(), x.end(), xi.begin());

            yi.resize(y.size());
            std::copy(y.begin(), y.end(), yi.begin());
        }

        R operator ()(const T & x)
        {
            return call(x, PWPolicy<PLEFT,PRIGHT>());   
        }

    private:
        R call(const T & x, const PWPolicy<TERM_CONST,TERM_CONST>)
        {
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx==xi.begin()) return yi[0];
            if(itx==xi.end()) return yi[yi.size()-1];

            --itx;
            return _call(x, itx);
        }

        R call(const T & x, const PWPolicy<TERM_LINEAR,TERM_CONST>)
        {
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx==xi.end()) return yi[yi.size()-1];
            if(itx != xi.begin()) --itx;

            return _call(x,itx);
        }

        R call(const T & x, const PWPolicy<TERM_CONST,TERM_LINEAR>)
        {
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx == xi.begin()) return yi[0];
            if(itx == xi.end()) --itx;
            if(itx != xi.begin()) --itx;

            return _call(x, itx);
        }

        R call(const T & x, const PWPolicy<TERM_LINEAR,TERM_LINEAR>)
        {
            typename std::vector<T>::iterator itx = std::upper_bound(xi.begin(), xi.end(), x);
            if(itx == xi.end()) --itx;
            if(itx != xi.begin()) --itx;

            return _call(x,itx);
        }

        R _call(const T &x, typename std::vector<T>::iterator & itx)
        {
            distance_type step = std::distance(xi.begin(),itx);
            
            typename std::vector<R>::const_iterator ity = yi.begin();
            std::advance(ity, step);

            T xl, xr, Dx;
            xl=*itx;
            xr=*(++itx);
            Dx=xr-xl;
            
            R yl, yr;
            yl=*ity;
            yr=*(++ity);

            return yl*(xr-x)/Dx+yr*(x-xl)/Dx;   
        }

        std::vector<T> xi;
        std::vector<R> yi;

    };




} /* end of namespace numcpp */

#endif /* __GENERIC_FUNC_H__ */

