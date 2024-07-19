
/*

　カレンダー生成ツール。月日曜日の3つを入力すると、そこから何日か分のカレンダーを次のように標準出力する。4つ目にその日数だろう。指定がない場合、今月と来月まででいいかも。つまり、
calender 6 6 THU
　で
 Jun.
MON TUE WED THU FRI SAT SUN
10  11  12  13  14  15  16
17  18  19  20  21  22  23
24  25  26  27  28  29  30

 Jul.
MON TUE WED THU FRI SAT SUN
 1   2   3   4   5   6   7
 8   9  10  11  12  13  14
　のように標準出力するプログラム。

　うるう年。4年に一度2月29日がくる。これに関しては、うるうを考えないことにする。もし、必要になったらそのとき実装すればいい。

　けいせんまわり。treeのけいせんと、小説のけいせん。
https://note.com/koge0koge/n/n38f2a993c893
　が参考になるかも。てかけいせんつかわなかった。

　このプログラムには、それぞれの月が何日であるかのデータが必要で、
1 31
2 28
3 31
4 30
5 31
6 30
　という形式になるが、各行2つのintなので、とりあえずは、int[12]に二つ目の値をいれることで実装しようと思う。
*/

// NULL
#include <stdio.h>
#include <bin.h>

int main(int argc, char **argv)
{
    // counter
    unsigned int i ;

    // option。dowはday of the weekの略で曜日の意。
    unsigned int m, d ;
    char *dow ;

    // これしないとオプション入れ忘れたときに10000とか入ってたら10000で生成しちゃう。n=0でなにも出力しない、k<2でなにも出力しない。p=0でつなぎかえない。
    m = 0 ;
    d = 0 ;
    dow = NULL ;

    // 表示する日数。とりあえず50に
    unsigned int n = 50 ;
    unsigned int 

    // 適当な文字列をオプションに入れても、いくつかのうち定義されたものだけ取り出して読み込むので関係ない。++iのところはi+1でもいいのだが、-n 23の23が引っかかることはないので飛ばしているだけ。++iで+1してからその値を代入している。
    for(i = 0 ; i < argc ; i++)
    {
        else if(str_cmp(argv[i], "--help", length_string(argv[i]) + 1))
        {
            printf("use like \"calendar 6 23 tue\"\n") ;
            return 0 ;
        }
    }

    if(argc == 4)
    {
        m = str_to_ui(argv[1]) ;
        d = str_to_ui(argv[2]) ;
        dow = argv[3] ;


    } 

    

    return 0 ;
    
}