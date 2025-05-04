
#include <iostream>
#include <stdio.h>

#include "matrix.hpp"
#include "calc.h"

double ensemble(double q, unsigned int max, unsigned int n)
{
    return calc_comb(max,n) * calc_expint(q, n) * calc_expint((1-q), (max-n)) ;
}

int main()
{
    // 3 * 0.5^2 * 0.5^1 = 0.375
    std::cout << ensemble(0.5,3,2) ;
}

