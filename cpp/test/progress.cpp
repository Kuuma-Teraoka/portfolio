
#include "progress.hpp"


int main()
{
    progress bar(9999) ;
    std::cout << bar.max << " " << bar.step << "\n" ;
    for(int j = 0 ; j < 9999 ; j++)
    {
        for(int i=0 ; i<99999 ; i++)
            int a = 1 ;
        bar.update(j) ;
    }
}

