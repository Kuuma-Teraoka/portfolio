
#include "matrix.hpp"

int main()
{
    // 列優先確認。matrix(double *, int, int)。
    double A[9] = {6.0, 2.0, 1.0,
               9.0, 3.0, 1.0,
               7.0, 4.0, 1.0} ;
    matrix a(A, 3, 3) ;
    std::cout << "matrix a(A, 3, 3):\n" << a ;

    // write(), matrix(str)
    a.write("tmp.cmat") ;
    // cmatで行優先に変換して書き出して、cmatで行優先を列優先に変換して読み込んでいるので正常。一方、fmatに名前を変換すると、行優先のものをそのまま列優先として読み込むので転置された状態になる。
    std::cout << "matrix(\"tmp.cmat\"):\n" << matrix("tmp.cmat") ; 
    //std::cout << "matrix(\"tmp.fmat\"):\n" << matrix("tmp.fmat") ; 
    
    // matrix::random
    std::cout << "matrix::random(3, 3):\n" << matrix::random(3, 3) ;

    // matrix::csr(), matrix::csr.to_matrix()
    std::cout << "matrix::csr(a).to_matrix():\n" << matrix::csr(a).to_matrix() ;

    // matrix::csr(file), csr.write
    matrix::csr(a).write("a.csr") ;
    std::cout << "matrix::csr(a).to_matrix():\n" << matrix::csr("a.csr").to_matrix() ;
    

}

