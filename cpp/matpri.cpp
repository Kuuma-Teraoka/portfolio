
const char *help =
    "\n"
    "Usage: matpri [OPTIONS] MATRIX_FILE\n"
    "\n"
    "matpri a.mat\n"
    "　で行と列をそのまま' '区切りで標準出力。\n"
    "matpri -t a.mat\n"
    "　で転地して出力。\n"
    "matpri -t -s : 3:4 a.mat\n"
    "　でa[:, 3:4]を転地して出力。a.T[:, 3:4]ではなく、a[:, 3:4].Tであることに注意。\n"
    "matpri -o fkl.mat -s ::3 1::3 stcd.mat\n"
    "　でstcd[::3,1::3]をdat形式ではなくmat形式でfkl.matに出力。バイナリを標準出力するとバグるのでバイナリモードを追加するのではなく、出力先を決めるとバイナリで出力するようにした。\n"
    "matpri -d a.mat\n"
    "　で\n"
    "2 3\n"
    "9 1\n"
    "　というような行列から\n"
    "0 0 2\n"
    "0 1 3\n"
    "1 0 9\n"
    "1 1 1\n"
    "　というテキストを標準出力をする。\n"
    "\n"
    "Error: main(): argc - i != 1\n" ;

#include "matrix.hpp"
#include "slice.hpp"
#include "file.hpp"
#include "bin.hpp"

int main(int argc, char **argv)
{
    int t = false ;
    int d = false ;
    int h = false ;
    int b = false ;
    double *c = NULL ;
    slice row_slice, col_slice ;
    str output_file_name = "" ;

    int i ;
    for(i = 1 ; i < argc ; i++)
    {
        if(argv[i] == str("-t"))
            t = true ;
        else if(argv[i] == str("-d"))
            d = true ;
        else if(argv[i] == str("-h"))
            h = true ;
        else if(argv[i] == str("-b"))
            b = true ;
        else if(argv[i] == str("-c") && i + 1 < argc)
        {
            c = bin::malloc<double>(1) ;
            *c = str(argv[++i]).float64() ;
        }
        else if(argv[i] == str("-s") && i + 2 < argc)
        {
            row_slice = slice(argv[++i]) ;
            col_slice = slice(argv[++i]) ;
        }
        else if(argv[i] == str("-o") && i + 1 < argc)
            output_file_name = argv[++i] ;
        else
            break ;
    }

    if(argc - i != 1)
    {
        std::cout << help ;
        return 1 ;
    }



    matrix mat(argv[argc - 1]) ;

    if(c != NULL)
    {
        std::cout << mat.count(*c) << "\n" ;
        free(c) ;
    }
    else if(h)
        std::cout << mat.shape[0] << "," << mat.shape[1] << "\n" ;
    else if(b)
    {
        const file input_file = argv[argc - 1] ;
        double buffer ; int shape[2] ;
        input_file.read(shape, sizeof(int) * 2) ;
        std::cout << str(&(shape[0]), sizeof(int)) << ": " << mat.shape[0] << "\n" ;
        std::cout << str(&(shape[1]), sizeof(int)) << ": " << mat.shape[1] << "\n" ; 
        for(int i = 0 ; i < shape[0] * shape[1] ; i++)
        {
            input_file.read(&buffer, sizeof(double)) ;
            std::cout << str(&buffer, sizeof(double)) << ": " << buffer << "\n" ;
        }
    } 
    else
    {        
        mat = mat(row_slice, col_slice).copy() ;
        if(t)
            mat = mat.T().copy() ;

        if(d)
            for(int i = 0 ; i < mat.shape[0] ; i++)
                for(int j = 0 ; j < mat.shape[1] ; j++)
                    std::cout << i << " " << j << " " << mat(i, j) << "\n" ;
        else if(output_file_name != "")
            mat.write(output_file_name) ;
        else
            std::cout << mat ; 
    }
   
    return 0 ;
}

