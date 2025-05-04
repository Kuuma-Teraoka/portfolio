
#include "matrix.hpp"
#include "network.hpp"
#include "coupled_lorenz.hpp"

int main()
{
    // mul_blas()
    double A[9] = {6.0, 2.0, 1.0,
               9.0, 3.0, 1.0,
               7.0, 4.0, 1.325233} ;
    double B[9] = {2.0, 7.0, 3.0,
               1.0, 8.0, 2.0,
               8.0, 5.0, 1.0} ;
    matrix a(A, 3, 3), b(B, 3, 3) ;
    std::cout << "a:\n" << a << "b:\n" << b ;
    std::cout << "a*b:\n" << a * b ;
    std::cout << "a.T * b\n" << a.T() * b ;
    std::cout << "a * b.T\n" << a * b.T() ;
    std::cout << "a(slice(0,1), slice()) * b:\n" << a(slice(0,2), slice()) * b ; 

    /*
    　inv()。
    import numpy as np
    import ndarray
    a = ndarray.read("a.cmat")
    np.linalg.inv(a)
    　でpythonと比較。一致したのであっているはず。
    */
    //a.write("a.cmat") ;
    std::cout << "a.inv():\n" << a.inv() ;

    // matrix::identity()
    std::cout << "identity():\n" << matrix::identity(3) * 0.0001 ;
    std::cout << "a - bi:\n" << a + matrix::identity(3) * 0.0001 ;

    
    // slice(1,-1,3)
    matrix akl = network::random(10, 30) ;
    matrix original_ts = coupled_lorenz::time_series(akl, 10) ;
    matrix tmp = original_ts(slice(0,-1,3), slice()) ; // move
    matrix ts = tmp ; // copy
    std::cout << "original_ts:" << original_ts.strides[0] << original_ts.strides[1] << "\n" << original_ts ;
    std::cout << "slice_ts:" << ts.strides[0] << ts.strides[1] << "\n" << ts ;






}

