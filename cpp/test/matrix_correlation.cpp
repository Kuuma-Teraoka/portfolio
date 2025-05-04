
#include "matrix.hpp"
#include "coupled_lorenz.hpp"
#include "network.hpp"

int main()
{
    matrix akl = network::random(1, 0) ;
    matrix ts = coupled_lorenz::time_series(akl, 60000) ;
    ts(0,slice()).autocorrelation().write("autocorrelation.fmat") ;
}

