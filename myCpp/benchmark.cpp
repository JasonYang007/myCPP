#include <iostream>
#include <fstream>
#include <ctime>
#include <complex>
#include <map>
#include <cassert>
#include <string>
#include <vector>
#include <list>
#include <omp.h>

#include "numcpp.h"
#include "StopWatch.h"
#include "class_traits.h"
#include "lambda.h"
#include "Tuples.h"

#define Million 1000000

extern bool debug;


void benchmark_array(void)
{
    StopWatch swatch;

    if(!debug)
    {
        #define LEN  1000*Million

        using namespace numcpp;

        std::cout << "\nBenchmarking started: Array" << std::endl;
        std::cout << "Allocating memories of size " << LEN/Million << "M..." << std::endl;

        Array<double> x(LEN, 3.14); 
        Array<double> y(LEN, 1.0);
        Array<double> z(LEN);
        Array<double, _SArray<double> > c(1.23);

        std::cout << "Done!" << std::endl;
        std::cout.flush();

        //std::cout << "c=" << c << std::endl;
        //std::cout << "x=" << x << std::endl;
        //std::cout << "y=" << y << std::endl;

        {
            std::cout << "\nRunning C-code..."; std::cout.flush();


            double *zptr, cc, *xptr, *yptr;
            zptr=z.get(); cc=c[0]; xptr=x.get(); yptr=y.get();

            swatch.start();

            for(unsigned ii=0; ii < LEN; ++ii)
            {
//                z[ii]=c[ii]*x[ii]+x[ii]/y[ii];
                zptr[ii]=cc*xptr[ii]+xptr[ii]/yptr[ii];

            }
            
            swatch.stop();

            std::cout << "Done! ("<< swatch.str() << ")" << std::endl;
        }


        {
            std::cout << "\nRunning Expression template..."; std::cout.flush();

            swatch.start();

            z = c*x + x/y;
            
            swatch.stop(); 

            std::cout << "Done! ("<< swatch.str() << ")" << std::endl;
        }

        {
            std::cout << "\nRunning traditional C++ code..."; std::cout.flush();

            swatch.start();

            {
                Array<double> tmp(LEN);        
                for(unsigned ii=0; ii < LEN; ++ii)
                {
                    tmp[ii]=c[ii]*x[ii];

                }

                Array<double> tmp2(LEN);        
                for(unsigned ii=0; ii < LEN; ++ii)
                {
                    tmp2[ii]=x[ii]/y[ii];

                }

                Array<double> tmp3(LEN);        
                for(unsigned ii=0; ii < LEN; ++ii)
                {
                    tmp3[ii]=tmp[ii]+tmp2[ii];

                }

                for(unsigned ii=0; ii < LEN; ++ii)
                {
                    z[ii] = tmp3[ii];
                }
            }
            
            swatch.stop();

            std::cout << "Done! ("<< swatch.str() << ")" << std::endl;
        }
    }
}

void benchmark_matrix(void)
{
    StopWatch swatch;

    if(true)
    {
        #define KILO 1000
        #define MLEN 500
        #define PDIM 3

        {
            using namespace numcpp;

            //std::cout << "c=" << c << std::endl;
            //std::cout << "x=" << x << std::endl;

            std::cout << "\nBenchmarking started: Matrix" << std::endl;
            std::cout << "Allocating memories of double[" << MLEN*MLEN*1.0/Million << "M]...";

            Matrix<double> x(MLEN,MLEN, numcpp::random<NORMAL>()); 
            Matrix<double> y(MLEN,MLEN, numcpp::random<NORMAL>());
            Matrix<double> z1(MLEN,MLEN);
            Matrix<double> z2(MLEN,MLEN);
            Matrix<double, _SArray<double> > c(1.23);

            std::cout << "Done!" << std::endl;

            //std::cout << "y=" << y << std::endl;
            double mt_etime;
            double st_etime;

            std::cout << "\n*** Num of procs: " << omp_get_num_procs() << std::endl;

            {
                using_mt mt(0);

                std::cout << "\nRunning Expression template(multi-threaded: " << mt.get_num_threads() << ")..."; std::cout.flush();

                swatch.start();

                z1 = c*tanh(x) + dot(x,dot(x,y));
                
                swatch.stop(); 
                mt_etime = swatch.elapsed();

                std::cout << "Done! ("<< mt_etime << "sec)" << std::endl;
                std::cout << "Result: \n" << z1(R(0,PDIM),R(0,PDIM)) << std::endl;
            }

            {
                std::cout << "\nRunning Expression template(single threaded)..."; std::cout.flush();

                swatch.start();

                z2 = c*tanh(x) + dot(x,dot(x,y));
                
                swatch.stop(); 
                st_etime = swatch.elapsed();

                std::cout << "Done! ("<< st_etime << "sec)" << std::endl;
                std::cout << "Result: \n" << z2(R(0,PDIM),R(0,PDIM)) << std::endl;
            }
            std::cout << "\nFrobenius norm: " << norm<3>(z2-z1) << std::endl << std::endl;
            std::cout << ">>> mt speed vs. st: " << st_etime/mt_etime << std::endl;
        }

    }
//        {
//            std::cout << "\nRunning C-code..."; std::cout.flush();
//            Matrix<double> & z = z2;
//
//
//            double *zptr, cc, *xptr, *yptr;
//            zptr=z2.get(); cc=c[0]; xptr=x.get(); yptr=y.get();
//
//            swatch.start();
//
////            for(unsigned ii=0; ii < MLEN; ++ii)
////            for(unsigned jj=0; jj < MLEN; ++jj)
//            for(unsigned kk=0; kk < MLEN*MLEN; ++kk)
//                {
//                    #define IDX ii+jj*MLEN
////                    unsigned kk = IDX;
//                    zptr[kk]=cc*std::tanh(xptr[kk]);
//                    double dd=0.0;
//                    for(unsigned
//
//                }
//            
//            swatch.stop();
//
//            std::cout << "Done! ("<< swatch.str() << ")" << std::endl;
//            std::cout << "Result: \n" << z2(R(0,PDIM),R(0,PDIM)) << std::endl;
//            std::cout << "Frob Diff: " << norm<3>(z2-z1) << std::endl;
//        }
//
//
//        if(false){
//            std::cout << "\nRunning traditional C++ code..."; std::cout.flush();
//
//            swatch.start();
//
//            {
//                Matrix<double> tmp(MLEN,MLEN);        
//                for(unsigned ii=0; ii < MLEN; ++ii)
//                for(unsigned jj=0; jj < MLEN; ++jj)
//                {
//                    tmp[IDX]=c[IDX]*x[IDX];
//
//                }
//
//                Matrix<double> tmp2(MLEN,MLEN);        
//                for(unsigned ii=0; ii < MLEN; ++ii)
//                for(unsigned jj=0; jj < MLEN; ++jj)
//                {
//                    tmp2[IDX]=x[IDX]/y[IDX];
//
//                }
//
//                Matrix<double> tmp3(MLEN,MLEN);        
//                for(unsigned ii=0; ii < MLEN; ++ii)
//                for(unsigned jj=0; jj < MLEN; ++jj)
//                {
//                    tmp3[IDX]=tmp[IDX]+tmp2[IDX];
//
//                }
//
//                for(unsigned ii=0; ii < MLEN; ++ii)
//                for(unsigned jj=0; jj < MLEN; ++jj)
//                {
//                    z3[IDX] = tmp3[IDX];
//                }
//            }
//            
//            swatch.stop();
//
//            std::cout << "Done! ("<< swatch.str() << ")" << std::endl;
//            std::cout << "Result: \n" << z3(R(0,PDIM),R(0,PDIM)) << std::endl;
//        }
//    }
}

void test_numcpp(void)
{
    {
        using namespace numcpp;
        Integer nrows = 2;
        Integer ncols = 3;

        Integer dim = nrows*ncols;

        Array<double> arr(dim,numcpp::random<NORMAL>());
     
        std::cout << "arr:\n" << arr << std::endl;

        //std::cout << "reshape(arr,nrows,ncols):\n" << reshape(arr,nrows,ncols) << std::endl;
        
        Range rng(range(0,10));
        
        WrapRange<0,Range> wr(rng);
        Integer ii = wr(5,6);

        WrapRange<0,int> wr2(ii);
        ii = wr2();

        typedef tuples::tuple<double,double,int,double>::TBase Ttpl;

        ii = ClassCount<Ttpl, double>::count;
        int ii0 = ClassCount<Ttpl, double,0>::index;
        int ii1 = ClassCount<Ttpl, double,1>::index;
        int ii2 = ClassCount<Ttpl, double,2>::index;
        int ii3 = ClassCount<Ttpl, double,3>::index;

        ii0 = ClassIndex<Ttpl, double, 0>::index;
        ii1 = ClassIndex<Ttpl, double, 1>::index;

        typedef ndarray<2,double> Tndarray;
        typedef NDINDEX<2,double, Tndarray, Range, Integer> TNDINDEX;

        ndarray<2,double> aa(reshape(arr,nrows,ncols));
        double dd = aa(0,1);

        TNDINDEX ndidx(aa, range(0,2), 0);
        dd = ndidx(1);

        ndarray<1,double,TNDINDEX> nda(ndidx.create_ndarray());

        dd =nda(1);


        ndarray<1,double,TNDINDEX> nda2(make_ndarray(aa,range(0,2), 0));
        dd = nda2(1);

        //std::cout << "reshape(arr,nrows,ncols)(0, R(0)): \n" << make_ndindex(reshape(arr,nrows,ncols), 0, R(0))(0) << std::endl;
        
        return;
    }


    StopWatch swatch;
    

    {
        using namespace numcpp;
        Integer dim = 400;
        
        std::cout << "\nBenchmarking Matrix(" << dim << "x" << dim << ")" << std::endl;
        std::cout << std::endl << std::string(30, '*') << std::endl;
        std::cout << "z = c*(x+y) + dot(x,dot(x,y))" << std::endl;
        std::cout << std::endl << std::string(30, '*') << std::endl;

        std::cout << "Allocating memories of double[" << dim*dim*1.0/Million << "M]...";

        Matrix<double> x(dim,dim, numcpp::random<NORMAL>());
        Matrix<double> y(dim,dim, numcpp::random<NORMAL>());
        Matrix<double> z1(dim,dim);
        Matrix<double> z2(dim,dim);
        double c(1.23);

        std::cout << "Done!" << std::endl;

        double mt_etime;
        double st_etime;

        std::cout << "\n*** Num of procs: " << omp_get_num_procs() << std::endl;

        {
            // multithreaded calculation using default number of threads
            using_mt mt(0); 

            std::cout << "\nRunning multi-threaded matrix calc (" 
                      << mt.get_num_threads() << " threads)..."; std::cout.flush();

            swatch.start();

            z1 = c*(x+y) + dot(x,dot(x,y));
                
            swatch.stop(); 
            mt_etime = swatch.elapsed();

            std::cout << "Done! ("<< mt_etime << "sec)" << std::endl;
            std::cout << "Result: z(R(0,3),R(0,3))= \n" << z1(R(0,PDIM),R(0,PDIM)) << std::endl;
        }

        {
            std::cout << "\nRunning single-threaded matrix calc ..."; std::cout.flush();

            swatch.start();

            z2 = c*(x+y) + dot(x,dot(x,y));
                
            swatch.stop(); 
            st_etime = swatch.elapsed();

            std::cout << "Done! ("<< st_etime << "sec)" << std::endl;
            std::cout << "Result: z(R(0,3),R(0,3))= \n" << z2(R(0,PDIM),R(0,PDIM)) << std::endl;
        }
        // Calculate Frobenius norm to verify the equal results
        std::cout << "\nFrobenius norm: norm<2>(z2-z1) = " << norm<2>(z2-z1) << std::endl << std::endl;

        // Speed improvement of multi-threading vs. single threading
        std::cout << ">>> multi-threading performance vs. single threading: " << st_etime/mt_etime << std::endl;
    }
}