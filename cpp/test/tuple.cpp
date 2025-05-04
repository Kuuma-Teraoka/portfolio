
#include <tuple>
#include <iostream> // std::cout

int main()
{
    std::tuple<int, int> a(3,4) ;
    std::get<0>(a) = 1 ;
    std::cout << std::get<0>(a) << "\n" ;

    std::pair<int, int> b(3,4) ;
    b.first = 1 ;
    std::cout << b.first << ", " << b.second << "\n" ;
}

