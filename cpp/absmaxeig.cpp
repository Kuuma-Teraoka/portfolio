
#include "echo_state_network.hpp"

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout <<
            "\n"
            "Usage: absmaxeig model.esn\n"
            "\n"
            "main(): argc != 2\n" ;
        return 1 ;
    }

    std::cout << echo_state_network(argv[1]).w_x.to_matrix().max_abs_eig() << "\n" ;

    return 0 ;
}

