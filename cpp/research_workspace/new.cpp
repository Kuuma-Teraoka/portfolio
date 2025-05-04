

#include <unistd.h> // getpid()
#include <vector>
#include <set>

#include "str.hpp"
#include "file.hpp"
#include "matrix.hpp"
#include "network.hpp"
#include "coupled_lorenz.hpp"
#include "echo_state_network.hpp"
#include "notify.hpp"

void simulation(int r) ;

const str rand_dir = file::environ("RINDFILE_PATH") ;

int node = 12 ;
double h_value = 0.06 ;
double c = 0.3 ;
double tau = 0.02 ;
int dump = 20000 ;
int learn = 60000 ;
int pred = 10000 ;
double beta = 0.0001 ;



void simulation(int r)
{
    matrix truth_links(12, 12) ;
    truth_links({0,6}, {0,6}) = network::barabasi_albert(6, 6, 6, rand_dir + "/" + str(r) + ".bin").to_matrix(rand_dir + "/" + str(r) + ".bin") ;
    truth_links({6,12}, {6,12}) = network::barabasi_albert(6, 6, 6, rand_dir + "/" + str(r) + ".bin").to_matrix(rand_dir + "/" + str(r) + ".bin") ;

    matrix ts = coupled_lorenz::time_series
    (
        truth_links,
        dump+learn+pred,
        rand_dir + "/" + str(r) + ".bin",
        h_value,
        c,
        tau
    ) ;
    ts = ts({}, {dump, -1}).copy() ; 

    echo_state_network model
    ({
        .Nu = 6 * 3,
        .range_w_in = 0.1,
        .Nx = 5000,
        .neuron_links = network::random
            (5000, 5000 * 3, rand_dir + "/" + str(r) + ".bin"),
        .spectral_radius = 0.9,
        .rand_file = rand_dir + "/" + str(r) + ".bin"
    }) ;
    model.train(ts({0, 18}, {0, learn}), ts({0, 18}, {1, learn + 1}), beta) ;
    std::cout << model.t_valid(ts({0, 18}, {learn + 1, learn + pred}), 2) << "\n" ; 
    model.train(ts({9, 27}, {0, learn}), ts({9, 27}, {1, learn + 1}), beta) ;
    std::cout << model.t_valid(ts({9, 27}, {learn + 1, learn + pred}), 2) << "\n" ;
}





int main()
{
    try
    { 
        for(int r = 0 ; r < 100 ; ++r)
        {
            simulation(r) ; 
            notify::send(str(getpid()) + " completed rand: " + str(r)) ;
        }
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << "\n" ;
        notify::send(e.what()) ;
        return 1 ;
    }
    return 0 ;
}










