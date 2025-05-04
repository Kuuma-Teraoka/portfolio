
#include "infer_links.hpp"
#include "matrix.hpp"
#include "err.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        std::cout <<
            "\n"
            "Usage: fp_fpr confusion_matrix.fmat\n"
            "\n"
            "argc < 2\n" ;
        return 1 ;
    }

    if(argc == 2)
        std::cout << infer_links::fp_fpr(matrix(argv[1])) << "\n" ;
    else
    {
        matrix lst(argc - 1, 1) ;
        for(int i = 0 ; i < lst.shape[0] ; i++)
            lst(i, 0) = infer_links::fp_fpr(matrix(argv[i + 1])) ;
        std::cout << lst.mean() << "\n" ;
    }
    return 0 ;
}

