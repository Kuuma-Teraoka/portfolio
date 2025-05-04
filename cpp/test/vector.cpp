
#include <vector>
#include <iostream>

int main()
{
    std::vector<int> vec(3) ;
    vec[0] = 3 ;
    for(int i : vec)
        std::cout << i << ", " ;
    std::cout << "\n" ;

    std::vector<int> vec2 = {0,3,6,1} ;
    std::cout << *std::max_element(vec2.begin(), vec2.end()) << "\n" ;

    std::vector<double> vec3 = {0.0, 1.0, 1.5, -3.4} ;
    std::cout << *std::max_element(vec3.begin(), vec3.end()) << "\n" ;


    std::vector<int> vec4 = {1, 3, 4, 7} ;
    std::cout << *(vec4.end() - 1) << "\n" ;

    // size()
    for(int i = 0 ; i < 10 ; i++)
    {
        vec4.push_back(4) ;
        std::cout << vec4.size() << ", " ;
    }
    std::cout << "\n" ;
    

    return 0 ;
}
