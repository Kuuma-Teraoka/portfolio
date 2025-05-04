
#include "file.hpp"
#include "progress.hpp"

int buffer_size = 1000000 ;

int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << "\nUsage pv in/input.mp4 dist/output.mp4\n\n" ;
        return 1 ;
    }

    file input(argv[1]), output(argv[2], "x") ;
    long long int file_size = input.size() ;
    progress bar(file_size, "", 0) ;
    unsigned char buffer[buffer_size] ; 
 
    for(int i = 1 ; i <= file_size / buffer_size ; i++)
    {
        input.read(buffer, buffer_size) ;
        output.write(buffer, buffer_size) ;
        bar.update((long long int)i * buffer_size - 1) ; 
    }
    for(int i = 1 ; i <= file_size % buffer_size ; i++)
    {
        input.read(buffer, 1) ;
        output.write(buffer, 1) ;
        bar.update((file_size / buffer_size) * buffer_size + i - 1) ;  
    }

    return 0 ;
}





