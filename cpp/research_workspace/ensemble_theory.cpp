
// math.comb(100, 51)で98913082887808032681188722800なので、combはcppのintでは求められない。pythonの上限なしintを実装しない限り。
#include <iostream>

#include "calc.h"


int main()
{
    int N = 100 ;
    double q = 0.3 ;
    double sum = 0 ; 
    std::cout << calc_comb(100, 51) ;
    return 0 ;
    for(int i = (int)((double)N / 2) + 1 ; i <= N ; i++)
        sum += calc_comb(N, i) *  calc_expint(q, i) * calc_expint(1 - q, N - i) ;
    std::cout << sum ;
}

