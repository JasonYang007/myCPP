#include "MatrixLAPACK.h"

namespace numcpp {


// *************************************
// ** Eigen Values/Vectors
// *************************************

Int lapack_eigen_h(char & jobz, char & uplo, Int & n, double *AF, Int & lda, double *w)
{
    double wkOpt;
    Int rcode = 0;

    // Query and allocate the optimal workspace
    Int  lwork = -1;
    lapack::dsyev_(&jobz, &uplo, &n, AF, &lda, w, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    double * work = new double[lwork*sizeof(double)];
    shared_array<double> shptr(work); // automatic release of memory once done

    // Solve the eigenvalue probelm
    lapack::dsyev_(&jobz, &uplo, &n, AF, &lda, w, work, &lwork, &rcode);
    assert(rcode==0);

    return rcode;
}


// *************************************
// **  SVD decomposition
// *************************************
Int lapack_svd(char & jobu, char & jobvt, Int & m, Int & n, double *A, Int &lda, double *S, double *U, 
                Int &ldu, double *Vt, Int &ldvt)
{
    Int rcode = 0;

     // Query and allocate the optimal workspace
    Int  lwork = -1;
    double wkOpt;

    lapack::dgesvd_(&jobu, &jobvt, &m, &n, A, &lda, S, U, &ldu, Vt, &ldvt, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    double * work = new double[lwork*sizeof(double)];
    shared_array<double> shptr(work); // automatic release of memory once done

    // Solve the eigenvalue problem
    lapack::dgesvd_(&jobu, &jobvt, &m, &n, A, &lda, S, U, &ldu, Vt, &ldvt, work, &lwork, &rcode);

    return rcode;
}


// *************************************
// **  QR decomposition
// *************************************
Int lapack_qr(Int & m, Int & n, double *A, Int &lda, double *R)
{
    Int rcode = 0;
    Int  K=std::min(m, n);

     // Query and allocate the optimal workspace
    Int  lwork = -1;
    double wkOpt;

    Array<double> _tau(K);
    double * tau = &_tau[0];

    lapack::dgeqrf_(&m, &n, A, &lda, tau, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    double * work = new double[lwork*sizeof(double)];
    shared_array<double> shptr(work); // automatic release of memory once done

    // Solve the first step of QR decomposition: 
    //      Calculate reflectors as intermediate Q and R
    lapack::dgeqrf_(&m, &n, A, &lda, tau, work, &lwork, &rcode);
    // Copy Upper Triangular region to R
    Integer idx=0;
    for(Integer cc=0; cc < n; ++cc)
        for(Integer rr=0; rr <= cc; ++rr)
            R[idx++] = A[rr + m*cc];

    // Query and allocate the optimal workspace
    lwork = -1;
    lapack::dorgqr_(&m,&n,&K,A,&lda, tau, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    work = new double[lwork*sizeof(double)];
    shptr.reset(work); // automatic release of memory once done

    // Solve the second step of QR decomposition: 
    //      Calculate Q from previous result
    lapack::dorgqr_(&m, &n, &K, A, &lda, tau, work, &lwork, &rcode);

    return rcode;
}


// *************************************
// **  LU decomposition
// *************************************
Int lapack_lu(Int & m, Int & n, double *A, Int &lda, Int *ipiv)
{
    Int rcode = 0;

    lapack::dgetrf_(&m, &n, A, &lda, ipiv, &rcode);

    return rcode;
}


// *************************************
// **  Least Square
// *************************************
Int lapack_lstsq(Int & m, Int & n, Int & nrhs, 
                  double *A, Int &lda, double *B, Int &ldb, 
                  double *S, double * rcond, Int & rank, double * rss)
{
    Int rcode = 0;
    Int  K=std::min(m, n);

     // Query and allocate the optimal workspace
    Int  lwork = -1;
    double wkOpt;

    lapack::dgelss_(&m, &n, & nrhs, A, &lda, B, &ldb, S, rcond, &rank, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    double * work = new double[lwork*sizeof(double)];
    shared_array<double> shptr(work); // automatic release of memory once done

    // Solve the solution
    lapack::dgelss_(&m, &n, & nrhs, A, &lda, B, &ldb, S, rcond, &rank, work, &lwork, &rcode);

    for(Integer cc=0; cc < nrhs; ++cc)
    {   double res=0.0;
        for(Integer rr=n; rr < m; ++rr)
        {
            double val = B[rr+m*cc];
            res += val*val;
        }
        rss[cc] = res;
    }

    return rcode;
}


// *************************************
// **  inverse matrix
// *************************************
Int lapack_inv(Int & n, double *A, Int &lda, Int *ipiv)
{
    Int rcode = 0;

     // Query and allocate the optimal workspace
    Int  lwork = -1;
    double wkOpt;

    lapack::dgetri_(&n, A, &lda, ipiv, &wkOpt, &lwork, &rcode);
    assert(rcode==0);

    // Prepare the required memory
    lwork = (Int)wkOpt;
    double * work = new double[lwork*sizeof(double)];
    shared_array<double> shptr(work); // automatic release of memory once done

    // Solve the solution
    lapack::dgetri_(&n, A, &lda, ipiv, work, &lwork, &rcode);

    return rcode;
}


} /* namespace numcpp */
