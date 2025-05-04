
#include "network.hpp"
#include "file.hpp"
#include "str.hpp"

const str rand_dir = file::environ("RINDFILE_PATH") ;

int main()
{
    std::cout << network::watts_strogatz(20, str("/dev/random"), 5) ;
    std::cout << network::barabasi_albert(6, 8, 20, rand_dir + "/" + str(1) + ".bin").to_matrix(rand_dir + "/" + str(1) + ".bin").count(1) ;
    std::cout << network::barabasi_albert(6, 8, 20, rand_dir + "/" + str(1) + ".bin").to_matrix(rand_dir + "/" + str(1) + ".bin") ;
    
}

