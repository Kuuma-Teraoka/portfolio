
#include "matrix.hpp"

int main()
{
    double A[9] =
    {
        6.0, 2.0, 1.0,
        9.0, 3.0, 1.0,
        7.0, 4.0, 7.0
    } ;
    matrix mat(A, 3, 3) ;
    for(matrix i : mat)
        i = matrix(3, 1, 0.0) ;
    std::cout << mat ;
}


