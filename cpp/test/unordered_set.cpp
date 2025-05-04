

#include <iostream> // std::cout
#include <unordered_set> // std::unordered_set
#include <vector> 

int main()
{
    std::vector<std::unordered_set<int>> network ;

    network.push_back({5, 2}) ;
    network.push_back({2, 4}) ;
    network.push_back({5, 2}) ;

    for(auto &i : network)
        for(auto &j : i)
            std::cout << j << ", " ;

    //if(network.find({2, 5}) != network.end())
    std::cout << "found !\n" ;

    return 0 ;
}

