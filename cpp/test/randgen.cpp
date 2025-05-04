
#include "randgen.hpp"
#include "matrix.hpp"

int main()
{
    // static f01()
    for(int i = 0 ; i < 20 ; i++) 
        std::cout << randgen::f01() << "\n" ;
    std::cout << randgen::used << "\n" ;

    randgen a ;

    // float64()
    for(int i = 0 ; i < 20 ; i++) 
        std::cout << a.float64() << "\n" ;
    std::cout << randgen::used << "\n" ;

    // uint64()
    std::cout << a.uint64() << "\n" ;

    // roulette
    matrix prob(3, 1) ;
    prob(0,0) = 0.7 ; prob(1,0) = 0.2 ; prob(2,0) = 1 ;
    for(int i = 0 ; i < 100 ; i++) 
        std::cout << a.roulette(prob) << "\n" ;
}

