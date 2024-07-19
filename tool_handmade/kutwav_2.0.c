
// FILE
#include <stdio.h>
// malloc()
#include <stdlib.h>
// myrealpath()
#include <file.h>
// str_cmp()
#include <bin.h>

#include <wav.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        perror("argc != 2\nPlease input wav file name such as \"$ kutwav some.wav\"") ;
        return 1 ;
    }

    FILE *input_file ;
    char *input_path = myrealpath(argv[1]) ;
    if((input_file = fopen(input_path, "r")) == NULL)
    {
        perror("fopen returned NULL. file name is different, or file don't exist in current directory") ;
        return 1 ;
    }
    free(input_path) ;

    WAV *input_data ;
    if((input_data = read_wav(input_file)) == NULL)
    {
        fclose(input_file) ;
        return 1 ;
    }
    fclose(input_file) ;



    free_wav(input_data) ;

    return 0 ;
}


