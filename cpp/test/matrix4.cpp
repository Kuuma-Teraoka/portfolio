
#include "matrix.hpp"
#include "network.hpp"
#include "coupled_lorenz.hpp"

int main()
{
    // copy()。x(299).copy()で、copy()の(*this)が右辺値参照になるのか、左辺値参照になるのか。matrix(matrix &&)とmatrix(const matrix &)でstd::coutをオンにしてから動かすこと。copyの右辺値参照クオリファイアである&&をつけるつけないで変わる。
    double A[9] = {6.0, 2.0, 1.0,
               9.0, 3.0, 1.0,
               7.0, 4.0, 1.325233} ;
    double B[9] = {2.0, 7.0, 3.0,
               1.0, 8.0, 2.0,
               8.0, 5.0, 1.0} ;
    matrix a(A, 3, 3), b(B, 3, 3) ;

    std::cout << (void *)a.data << "\n" ;
    matrix c = a(2).copy() ;
    std::cout << (void *)c.data << "\n" ;

    std::cout << a(4, 4) ;

}

