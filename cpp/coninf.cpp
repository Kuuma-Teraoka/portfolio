
const char *help =
    "\n"
    "Usage: coninf confusion_matrix.fmat infer_links.fmat\n"
    "\n"
    "confusion_matrix.fmatをinfer_links.fmatに変換するプログラム。\n" ;

#include "infer_links.hpp"

    
int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << help ;
        return 1 ;
    }

    infer_links::confusion_matrix(argv[1]).to_inferred().write(argv[2]) ;

    return 0 ;
}


