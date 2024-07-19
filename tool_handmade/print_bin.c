
/*
　print_binは
print_bin -i a.txt
　でa.txtの中身を標準出力。デフォで128バイトまで。
print_bin -i a.txt -f
　で全てのバイトを出力
print_bin -i a.txt -n 20
　で20バイト分。
print_bin a.txt
　なら文字列として、61 '.' ...と標準出力。
print_bin
　なら標準入力をそのまま標準出力。つまりパイプ。

　print_bin関数が本質で、このプログラムはただコマンドライン引数に対してprint_binに与える引数を調整しているだけ。

　とりあえずいまは、引数がひとつあるときとないときのみ対応していて、あれば文字列、なければパイプにしている。また128バイトまでという制限は作っていないので、動画ファイルなどを入れた場合フリーズするかもしれない。
　また、パイプのときのfgetsでprint_binを繰り返し呼び出すことによって、改行で毎回表を作り直してしまっている。

　fgetcを使っていたのだが、標準入力でファイルをパイプで与えてもFFが続いてしまって、EOFで終了してくれなかった。問題はEOFは-1であり、-1はunsigned charでFFになることにあった。00~FFはもちろん、-1も格納できる4byteのintを使わないといけないらしい。
　といっても、100byteの読み取りで、4byteのintを100個分mallocするのはメモリの無駄で、さらにprint_binに渡しても、00が出力されてしまう。なので、getcをintに渡して、intをunsigned char *bufferに与えている。
*/

#include <bin.h>
// stdin, NULL
#include <stdio.h>
// malloc
#include <stdlib.h>

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        print_bin((unsigned char *)argv[1], length_string(argv[1]) + 1) ;
    }

    else if(argc == 1)
    {
        unsigned int i, buffer_size = 1024 ;
        int to_buffer ;
        unsigned char *buffer = (unsigned char *)malloc(buffer_size) ;

        for(i = 0 ; i < buffer_size ; i++)
        {
            if((to_buffer = getc(stdin)) == EOF)
                break ;
            else
                buffer[i] = to_buffer ;
        }
        print_bin(buffer, i) ;

        free(buffer) ;
    }

    return 0 ;
}