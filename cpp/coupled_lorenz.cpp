
#include "coupled_lorenz.hpp"
#include "network.hpp"
#include "err.h"

/*
　pythonのcoupled_lorenzが遅すぎるので、pythonから呼び出せるように実行可能ファイルに。9000とか、100はオプションで変えられるようにしたいが、あとでやる。
*/

int main(int argc, char **argv)
{
    if(argc < 2)
        err_raise( 
            "\n"
            "Usage: coupled_lorenz.cpp ~/randfile/1.bin\n"
            "\n"
            "argc < 2\n"
        ) ;

    matrix coupling_links = network::random(20, 100, argv[1]) ;
    matrix ts = coupled_lorenz::time_series(coupling_links, 90000, argv[1]) ;
    coupling_links.write("coupling_links.cmat") ;
    ts.write("ts.cmat") ;
    return 0 ;
}

