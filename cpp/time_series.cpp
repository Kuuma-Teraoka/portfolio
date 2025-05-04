#include "gnuplot-iostream.h"
#include <vector>
#include <utility> // std::pair
#include <string>  // std::string

#include "matrix.hpp"
#include "str.hpp"





int main(int argc, char **argv)
{    
    Gnuplot gp;

    str output_file ;
    if(argc == 3)
        output_file = argv[2] ;
    else
        output_file = "gnuplot-iostream_no_output_name.pdf" ;

    matrix mat(argv[1]) ;
    mat = mat({0,1000}).copy() ;

    gp << 
        "set terminal pdf enhanced\n"
        "set output '" <<output_file<< "'\n"
        "set tics font 'Times New Roman, 18'\n"
        "set key font 'Times New Roman, 14'\n"
        "set xlabel 't' font 'Times New Roman, 22'\n"
        "set ylabel 'Output' font 'Times New Roman, 22'\n" 
        //"set xrange [0:1000]\n"
        //"set yrange [-50:50]\n"
        "plot '-' using 0:1 with linespoints pt 7 ps "<<(double)100/mat.shape[1]<<" lt 1 lw "<<(double)1000/mat.shape[1]<<" lc rgb '#000000' notitle\n" ;
 
    gp.send1d(mat.to_vector(1)) ; 

    std::cout << "generated: " << output_file << "\n" ;
    return 0;
}
