#include <iostream>
#include <vector>
#include <omp.h>

#define CHUNKSIZE 100

//#define DO_PARALLEL

void test_array(int nn, float *a, float *b, float *c, float & dummy)
{
	for(int j=0; j < nn; ++j)
		c[j]=j*a[j]+b[j]+a[j]*b[j];
	dummy=c[0];
}

void test_omp(unsigned N)
{
int i, chunk;
float *a, *b, *c;

a = new float[N];
b = new float[N];
c = new float[N];

for(i=0; i < N; i++)
	a[i]=b[i]=i*1.0;

//*******************************
std::cout << "\nOpenMP enabled! (N=" << N << ")" <<std::endl;
double stime, mt_elapsed, st_elapsed;
float dummy;
std::vector<float> vec(N);

stime=omp_get_wtime();

#pragma omp parallel for
for(int ii=0; ii < N; ++ii)
{	
	test_array(N,a,b,c, dummy);
	
	std::vector<float>::iterator it = vec.begin()+ii;	
	*it = a[it-vec.begin()];
}

std::cout << "elapsed time(sec): " << (mt_elapsed=omp_get_wtime()-stime) << std::endl;

//*******************************
std::cout << "\nSingle thread (N=" << N << ")" <<std::endl;

stime=omp_get_wtime();

for(i=0; i < N; ++i)
{	
	test_array(N,a,b,c, dummy);
}
std::cout << "elapsed time(sec): " << (st_elapsed=omp_get_wtime()-stime) << std::endl;

std::cout << "\n>>> Improvement: " << st_elapsed/mt_elapsed << std::endl;

std::cout << "\n" << std::endl;

delete[] a;
delete[] b;
delete[] c;
}
