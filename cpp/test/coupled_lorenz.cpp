
#include "coupled_lorenz.hpp"
#include "network.hpp"
#include "file.hpp"
#include "str.hpp"
#include "infer_links.hpp"

int main()
{
    matrix ts = coupled_lorenz::time_series
    (
        network::random(1, 0),
        60000,
        file(file::environ("RINDFILE_PATH") + str("/1.bin")),
        0.06,
        0.3,
        0.001
    ) ;
    ts.write("ts.fmat") ; 
    infer_links::delayed_embedding(ts(0, {}), 3, 100).write("delay.fmat") ;
}

