
/*
　print_binで大きいファイルの先頭だけみるため作ったもの。
cat a.txt | head_bin 10 | ...
　とすると、先頭10バイトだけ標準出力する。つまり、標準入力の一部を切り出して標準出力するコード
*/

// stdin
#include <stdio.h>

// str_to_i()
#include <bin.h>

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        unsigned char data ;
        unsigned int i ;

        for(i = 0 ; i < str_to_i(argv[1]) ; i++)
        {
            if((data = fgetc(stdin)) == EOF)
                break ;
            fputc(data, stdout) ;
        }
    }

    return 0 ;
}