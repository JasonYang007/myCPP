#ifndef __MATRIX_LAPACK_H_
#define __MATRIX_LAPACK_H_

#include "numcpp.h"
#include "Tuples.h"

namespace lapack 
{
    extern "C" {
        #include <f2c.h>
        #ifndef __GNUC__
            #include <clapack.h>
            typedef integer Int;
        #else
            typedef int Int;
            Int dpotrf_ (char *, Int *, double *, Int *, Int *);
            Int dsyev_(char *, char *, Int *, double *, Int *, double *, double *, Int *, Int *);
            Int dgesvd_(char *jobu, char *jobvt, Int *m, Int *n, double *A, Int *lda, double *S, double *U, 
                        Int *ldu, double *Vt, Int *ldvt, double *wkOpt, Int *lwork, Int *rcode);
            Int dgeqrf_(Int *m, Int *n, double *A, Int *lda, double *tau, double *wkOpt, Int *lwork, Int *rcode);
            Int dorgqr_(Int *m, Int *n, Int *K, double *A, Int *lda, double *tau, double *work, Int *lwork, Int *rcode);
            Int dgetrf_(Int *m, Int *n, double *A, Int *lda, Int *ipiv, Int *rcode);
            Int dgelss_(Int *m, Int *n, Int * nrhs, double *A, Int *lda, double *B, Int *ldb, double *S, 
                        double *rcond, Int *rank, double *wkOpt, Int *lwork, Int *rcode);
            Int dgetri_(Int *n, double *A, Int *lda, Int *ipiv, double *wkOpt, Int *lwork, Int *rcode);
        #endif
    }
}

// *************************************
// *** Cholesky
// *************************************
namespace numcpp
{

#undef max
#undef min
#undef abs

typedef lapack::Int Int;

template<typename E>
Int cholesky(const Matrix<double, E> & mat, LTMatrix<double> & res)
{
    assert(mat.nrows == mat.ncols);
    Matrix<double> _mat(mat.nrows,mat.ncols);

    _mat = mat;
    Int dim = _mat.nrows;
    /* call LAPACK to compute Cholesky */
    char uplo='L'; //Upper Triangular, 'L': Lower Triangular
    double *AF = & _mat.expr[0];
    Int  rcode = 0;

    lapack::dpotrf_ (&uplo, &dim, AF, &dim, &rcode);
    assert(rcode == 0);

    // std::cout << "LAPACK:\n" << _mat << std::endl;
    res = _mat;
    return rcode;
}

// *************************************
// ** Eigen Values/Vectors
// *************************************
Int lapack_eigen_h(char & jobz, char & uplo, Int & n, double *AF, Int & lda, double *w);

template<typename R1, typename R2>
Int eigh(SMatrix<double> & mat, Array<double,R1> & eigvals, Matrix<double,R2> & eigvec)
{
    assert(mat.nrows == mat.ncols && mat.nrows == eigvals.size());

    Matrix<double> _mat(mat.nrows,mat.ncols);
    _mat = mat;
    
    char jobz='V'; // 'N': eigenvalue only, 'V': eigenvalue and eigenvectors.
    char uplo='U'; // 'U': upper triangle is stored, 'L': lower triangle is stored.
    Int n = mat.nrows;
    Int lda = n;
    
    double * AF = & _mat.expr[0];
    double * w = eigvals.expr.get();
    Int  rcode = 0;

    rcode=lapack_eigen_h(jobz, uplo, n, AF, lda, w);
    assert(rcode==0);

    // Set up the return matrices
    eigvec = _mat;
    return rcode;
}


template<typename R1>
Int eigvalsh(SMatrix<double> & mat, Array<double,R1> & eigvals)
{
    assert(mat.nrows == mat.ncols && mat.nrows == eigvals.size());
    Matrix<double> _mat(mat.nrows,mat.ncols);
    _mat = mat;
    
    char jobz='N'; // 'N': eigenvalue only, 'V': eigenvalue and eigenvectors.
    char uplo='U'; // 'U': upper triangle is stored, 'V': lower triangle is stored.
    Int n = mat.nrows;
    Int lda = n;
    
    double * AF = & _mat.expr[0];
    double * w = eigvals.expr.get();
    Int  rcode = 0;

    rcode=lapack_eigen_h(jobz, uplo, n, AF, lda, w);
    assert(rcode==0);

    return rcode;
}

// *************************************
// **  SVD decomposition
// *************************************
Int lapack_svd(char & jobu, char & jobvt, Int & m, Int & n, double *A, Int &lda, double *S, double *U, 
                Int &ldu, double *Vt, Int &ldvt);

template<typename R1, typename R2, typename R3>
Int svd(const Matrix<double> & mat, Matrix<double,R1> & u, Array<double,R2> & s, Matrix<double,R3> & vt, 
            bool full_matrices=true)
    // mat = u * s * vt
{
    Matrix<double> _mat(mat.nrows, mat.ncols);
    _mat = mat;

    Integer K=std::min(mat.nrows, mat.ncols);
    if(full_matrices){
        assert( (mat.nrows == u.nrows && u.nrows==u.ncols) &&
                (K== s.size()) &&
                (mat.ncols == vt.nrows && vt.nrows==vt.ncols) );
    }
    else
    {   
        assert( (mat.nrows == u.nrows && u.ncols==K) &&
                (K== s.size()) &&
                (K == vt.nrows && vt.ncols==mat.ncols) );
    }

    char jobu=(full_matrices)?'A':'S';  //'A': full matrices of U, 'S': reduced size of U
    char jobvt=(full_matrices)?'A':'S'; //'A': full matrices of U, 'S': reduced size of U
    Int m = mat.nrows;
    Int n = mat.ncols;
    double * A = &_mat.expr[0];
    Int lda = m;
    double * S = &s.expr[0];
    double * U = &u.expr[0];
    Int ldu = m;
    double * Vt = &vt.expr[0];
    Int ldvt = (full_matrices)?n:K;
    Int rcode;

    rcode = lapack_svd(jobu, jobvt, m, n, A, lda, S, U, ldu, Vt, ldvt);
   
    return rcode;
}

template<typename R1>
Int svd(const Matrix<double> & mat, Array<double,R1> & s)
{
    Matrix<double> _mat(mat.nrows, mat.ncols);
    _mat = mat;

    Integer K=std::min(mat.nrows, mat.ncols);
    
    assert( K== s.size() );

    char jobu='N';  //'N': no columns of U computed
    char jobvt='N';  //'N': no columns of Vt computed
    Int m = mat.nrows;
    Int n = mat.ncols;
    double * A = &_mat.expr[0];
    Int lda = m;
    double * S = &s.expr[0];
    double * U = NULL;
    Int ldu = 1;
    double * Vt = NULL;
    Int ldvt = 1;
    Int rcode;

    rcode = lapack_svd(jobu, jobvt, m, n, A, lda, S, U, ldu, Vt, ldvt);
   
    return rcode;
}


// *************************************
// **  QR decomposition
// *************************************
Int lapack_qr(Int & m, Int & n, double *A, Int &lda, double *R);

template<typename R1, typename R2>
Int qr(const Matrix<double> & mat, Matrix<double,R1> & q, Matrix<double,R2> & r)
    // mat = q * r, where q: orthogonal and r: upper triangular
{
    Integer K=std::min(mat.nrows, mat.ncols);
    assert( (q.nrows == mat.nrows) && (q.ncols == K ) &&
            (r.nrows == K) && (r.ncols == mat.ncols) );

    Matrix<double> _mat(mat.nrows, mat.ncols);
    _mat = mat;


    Int m = mat.nrows;
    Int n = mat.ncols;
    double * A = &_mat.expr[0];
    Int lda = m;
    double * R = &r.expr[0];

    Int rcode;

    rcode = lapack_qr(m, n, A, lda, R); // A contains Q as output

    q = _mat;
   
    return rcode;
}


// *************************************
// **  LU decomposition
// *************************************
Int lapack_lu(Int & m, Int & n, double *A, Int &lda, Int *ipiv);

template<typename R1, typename R2, typename R3>
Int lu(const Matrix<double> & mat, Matrix<double,R1> & l, Matrix<double,R2> & u, Array<Integer,R3> & pivot)
    // mat = pivot * l * u
{
    Integer K=std::min(mat.nrows, mat.ncols);
    assert( (l.nrows == mat.nrows) && (u.ncols == mat.ncols ) && (pivot.size() == K) );

    Matrix<double> _mat(mat.nrows, mat.ncols);
    _mat = mat;

    Array<Int> _pivot(pivot.size());

    Int m = mat.nrows;
    Int n = mat.ncols;
    double * A = &_mat.expr[0];
    Int lda = m;
    Int * ipiv = &_pivot[0];

    Int rcode;

    rcode = lapack_lu(m, n, A, lda, ipiv); // A contains Q as output
    
    triu(u)=_mat;
    tril(l)=_mat;
    diag(l) = 1.0;
    pivot = _pivot;
    pivot -= 1;

    return rcode;
}

// *************************************
// **  Least Square
// *************************************
Int lapack_lstsq(Int & m, Int & n, Int & nrhs, 
                  double *A, Int &lda, double *B, Int &ldb, 
                  double *S, double * rcond, Int & rank, double * rss);

template<typename R1, typename R2, typename R3, typename R4>
Int lstsq(const Matrix<double> & mat, Matrix<double,R1> & x, Matrix<double,R2> & b, 
           Array<double, R3> & rss, Int & rank, Array<double,R4> & singular)
{
    Int m = mat.nrows;
    Int n = mat.ncols;
    Int nrhs = b.ncols;
    Int K = std::min(m, n);

    assert( (mat.ncols == x.nrows) && (x.ncols == b.ncols) && (mat.nrows == b.nrows) &&
            (rss.size() == b.ncols) && (singular.size() == K) );

    Matrix<double> _mat(mat.nrows, mat.ncols);
    _mat = mat;
    Matrix<double> _x(b.nrows, b.ncols);
    _x = b;

    double * A = &_mat.expr[0];
    Int lda = m;
    double * B = &_x.expr[0];
    Int ldb = m;
    double rcond = -1.0;
    Int rcode;
    double * RSS = &rss.expr[0];
    double * S = &singular.expr[0];

    rcode = lapack_lstsq(m, n, nrhs, A, lda, B, ldb, S, &rcond, rank, RSS);
    for(Integer cc=0; cc < nrhs; ++cc)
        for(Integer rr=0; rr < x.nrows; ++rr)
            x(rr,cc) = _x(rr,cc);

    return rcode;
}


template<typename R>
tuples::tuple<int,double> slogdet(const Matrix<double,R> & mat)
{
    Matrix<double> l(mat.nrows,mat.ncols);
    Matrix<double> u(mat.nrows,mat.ncols);
    Array<Integer> p(std::min(mat.nrows,mat.ncols));

    lu(mat, l, u, p);

    Array<double> d=log(abs(diag(u)));
    double res = sum(d);

    int sgn = (int)prod(sign(diag(u))), idx=0;
    for(typename Array<Integer>::iterator it = p.begin(); it != p.end(); ++it, ++idx)
        if(idx != *it) sgn *= -1;
    return tuples::tuple<int,double>(sgn,res);
}

template<typename R>
double det(const Matrix<double,R> & mat)
{
    tuples::tuple<int,double> res = slogdet(mat);
    return res.get<0>() * std::exp(res.get<1>());
}

// *************************************
// **  inverse matrix
// *************************************
Int lapack_inv(Int & n, double *A, Int &lda, Int *ipiv);

template<typename R1, typename R2>
Int inv_(const Matrix<double,R1> & mat, Matrix<double,R2> & res)
{
    assert( (mat.nrows == res.nrows) && (mat.ncols == res.ncols) && (mat.ncols==mat.nrows) );
    Matrix<double> _mat(copy(mat));
    Array<Int> _pivot(mat.nrows);

    Int m = mat.nrows;
    double * A = &_mat.expr[0];
    Int lda = m;
    Int * ipiv = &_pivot[0];

    Int rcode = lapack_lu(m, m, A, lda, ipiv);
    if(rcode) return rcode;

    rcode=lapack_inv(m, A, lda, ipiv);

    if(rcode==0)  res = _mat;

    return rcode;
}
// *************************************
// **  pseudo inverse matrix
// *************************************
template<typename R1, typename R2>
Int pinv(const Matrix<double,R1> & mat, Matrix<double,R2> & res)
{
    assert( (mat.nrows == res.ncols) && (mat.ncols == res.nrows) );
    Matrix<double> u(mat.nrows, mat.nrows);
    Matrix<double> vt(mat.ncols, mat.ncols);
    Array<double> s(std::min(mat.nrows, mat.ncols));

    Int rcode = svd(mat, u, s, vt, true);
    if(!rcode)
    {   
        Array<double> sinv(s.size());
        Array<double>::iterator jt = sinv.begin();
        for(typename Array<double>::iterator it=s.begin(); it != s.end(); ++it, ++jt)
        {
            if(*it) 
                *jt = 1.0/(*it);
            else
                *jt = 0.0;
        }
        res = dot(dot(vt.Tr()(R(),R(0,sinv.size())), diag(sinv)),u.Tr()(R(0,vt.nrows),R()));
    }
    return rcode;
}


} /* namespace numcpp */

#endif /* __MATRIX_LAPACK_H_ */

