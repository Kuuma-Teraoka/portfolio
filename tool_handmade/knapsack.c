

/*
　学食マネーを使いきりたいが、1080を、いくつかあるメニューの金額の定数倍に一致するか調べるのは整数計画法とかなんとかっていう最適化問題を解かなければならないらしい。
　オペレーションズリサーチ、計算論あたりのlectureをすべて自分でノートにまとめてtext化したい。オペレーションズリサーチのは最小にするもので、かつ小数が許されてたように思う。

　とりあえず、このコードは、
1080
44
66
77
...
495
　を標準入力で受け取って、組を求めるコードにする。1行目がtargetで、2行目以降がそれぞれの整数倍する値。
*/

// printf()
#include <stdio.h>

// malloc()
#include <stdlib.h>

// str_to_ui()
#include <bin.h>

void solba(unsigned int, unsigned int *, unsigned int, unsigned int*, unsigned int ***) ;
void free_answer(unsigned int *, unsigned int ***) ;
void merge_answer(unsigned int, unsigned int *, unsigned int ***, unsigned int *, unsigned int ***) ;
void remove_0A(char *) ;

int main()
{

    char *std_line ;

    std_line = (char *)malloc(sizeof(char) * 128) ;

    if(fgets(std_line, 128, stdin) == NULL)
    {
        perror("no input") ;
        free(std_line) ;
        return 1 ;
    }

    remove_0A(std_line) ;
    unsigned target = str_to_ui(std_line) ;
    unsigned *array, *temp ;
    array = (unsigned *)malloc(sizeof(unsigned)) ;
    unsigned len = 0 ;

    while(fgets(std_line, 128, stdin) != NULL)
    {

        len++ ;
        temp = (unsigned *)realloc(array, sizeof(unsigned) * len) ;
        if (temp == NULL)
        {
            free(array) ;
            perror("Failed to reallocate memory") ;
            return 1 ;
        }
        array = temp ;

        remove_0A(std_line) ;
        array[len - 1] = str_to_ui(std_line) ;

    }

    free(std_line) ;



    unsigned int ansc = 0 ;
    unsigned int **ansv = NULL ;
    solba(target, array, len, &ansc, &ansv) ;


    if(ansv == NULL)
    {
        perror("no answer") ;
        free(array) ;
        return 1 ;
    }

    else
    {
        unsigned int i, j ;
        for(i = 0 ; i < ansc ; i++)
        {
            printf("%u =", target) ;
            for(j = 0 ; j < len ; j++)
                printf(" +%u*%u", array[j], ansv[i][j]) ;
            printf("\n") ;
        }
        
        free(array) ;
        free_answer(&ansc, &ansv) ;

        return 0 ;
    }
}

/*
　targetに43を与えられて、ポインタと長さで{1, 3, 40}を受け取ったら、1*0+3*1+40*1=43という解を見つけて、{0, 1, 1}を返す関数。返す配列の長さはlenと一致するので、返すのはポインタだけでいい。もし長さも返す場合、構造体を返す形にしてポインタと長さという複数の値を返す必要がでてくるはず。いや、引数にポインタを与えて書き換えるんだった。

　target=1080で、590, 500, 400, ...と続いていたら、
590が0, 1個の2パターンのそれぞれについて、
500が0, 1, 2個の3パターンのそれぞれについて、
400が0, 1, 2個の3パターンのそれぞれについて、
...
と続けて、2*3*3*...パターンを計算して、その中に一つでも1080に一致するものがあれば返す。

　これを再起を使って実現していて、再起を崩してみれば上の全パターン計算の挙動だが、再起を崩さずみると、590が0個のときtarget=1080で500, 400, ...の問題が解ければ解けて、590が1個のときtarget=490で500, 400, ...の問題が解ければ590を1個足せば1080に一致するので解けて、、、という理解になる。

　すべての解を返すようにする。

***ansv
　がややこしいので整理する。
56 00 00 00 3A 00 00 00
　というunsigned int型のデータが格納されたメモリの先頭が01 00番地だったとして、
01 00 02 00 03 00
　というように、番地を格納されたメモリを用意する。そしてその先頭の01 00が01 80番地だったとする。返すのは01 80と、それが指すメモリの容量である3という数字である。つまり、unsigend intとunsigned int **を返す。ただ、返すのが1変数ではないので、引数にポインタを与えて書き換えてもらうようにしているので、それぞれのポインタである、unsigned int *とunsigned int ***が引数にある。

　また、unsigned int型の配列の長さはstringとはちがい必要であるが、lenと一致しているので返していない。

　ansvは確保せずポインタ変数だけ渡せばいいが、anscはunsigned int a ;で、&aをわたす。

https://www.msi.co.jp/solution/nuopt/docs/examples/html/02-05-00.html
　より、ナップサック問題の一種なのかもしれない。バックに1000gはいるなか、それぞれのアイテムに価値とgが決まっていて、同じものをたくさんいれていいという条件のもと、価値を最大化する問題。これの価値とgを同じに設定すると、gをぴったり1000gにする問題になり、その解をすべて求めているのが今回。
*/
void solba(unsigned int target, unsigned int *array, unsigned int len, unsigned int *ansc, unsigned int ***ansv)
{
    if(len == 0)
    {
        *ansc = 0 ;
        *ansv = NULL ;
    }

    else if(len == 1)
    {
        if(target % *array == 0)
        {
            *ansc = 1 ;
            // *ansc==1なので* *anscは必要ないが一応かいた。
            *ansv = (unsigned int **)malloc(sizeof(unsigned *) * *ansc) ;
            // len==1なので* lenは必要ないが一応かいた。
            (*ansv)[0] = (unsigned *)malloc(sizeof(char) * len) ;
            (*ansv)[0][0] = target / *array ;
        }
        else
        {
            *ansc = 0 ;
            *ansv = NULL ;
        }
    }

    else
    {
        unsigned int i, j ;
        unsigned int *temp ;

        unsigned int current_c ;
        unsigned int **current_v ;

        *ansc = 0 ;
        *ansv = NULL ;

        // target=10, array={1, 3}なら、3を0, 1, 2, 3個の4つの場合が考えられる。つまり、10/3=3なので、i = 0 ; i <= 3 ;。reallocは末尾に追加されるので、末尾を消したものをとく。
        for(i = 0 ; i <= target / array[len - 1] ; i++)
        {
            // ansc, ansvは空なので受け皿に使っていい。
            solba(target - array[len - 1] * i, array, len - 1, &current_c, &current_v) ;
            if(current_v != NULL)
            {
                for(j = 0 ; j < current_c ; j++)
                {
                    temp = (unsigned *)realloc(current_v[j], sizeof(unsigned) * len) ;
                    if(temp == NULL)
                    {
                        free_answer(&current_c, &current_v) ;
                        perror("Failed to reallocate memory") ;
                        exit(1) ;
                    }
                    current_v[j] = temp ;
                    current_v[j][len - 1] = i ;
                }
                merge_answer(len, ansc, ansv, &current_c, &current_v) ;
                free_answer(&current_c, &current_v) ;
            }
        }
    }
}

void free_answer(unsigned int *ansc, unsigned int ***ansv)
{
    unsigned int i ;
    for(i = 0 ; i < *ansc ; i++)
        free((*ansv)[i]) ;

    free((*ansv)) ;

    *ansc = 0 ;
    *ansv = NULL ;
}

/*
　ansにcurを追加する関数。curが存在するのは前提として、ansが存在しなければコピーし、すれば追加する。
　これを実行したあとにcurをfreeするので、ポインタをそのままコピーするのではなく、mallocで確保して中身をちゃんとコピーする。
　追加の場合はargvをcurc分reallocして、同じようにmallocで確保してコピーする。
　というか、必ずcurはansに組み込むのでmallocして、呼び出しもとでfreeするなら、ポインタをコピーするだけにしてもいい気がする。いや、番地を格納しているメモリは解放しなければならないので、ちゃんとメモリ確保コピーにする。
　lenもポインタにしてもいいのだが、固定なのでポインタにしなかった。anscはcurcを足した値に書き換えるし、ansvは空の場合mallocしてそのポインタに書き換えるので、ポインタにする。curc, curvは書き換えは行わず読み取りだけなのでポインタである必要はないが、ansc, ansvと同じ形のほうがわかりやすいためポインタにした。
*/
void merge_answer(unsigned int len, unsigned int *ansc, unsigned int ***ansv, unsigned int *curc, unsigned int ***curv)
{
    unsigned int i ;

    if(*ansv == NULL)
    {
        *ansc = *curc ;
        *ansv = (unsigned int **)malloc(sizeof(unsigned *) * (*ansc)) ;
        for(i = 0 ; i < *ansc ; i++)
        {
            (*ansv)[i] = (unsigned *)malloc(sizeof(unsigned) * len) ;
            ui_cpy(len, (*curv)[i], (*ansv)[i]) ;
        }
    }

    else
    {
        unsigned int **temp2 ;

        temp2 = (unsigned int **)realloc(*ansv, sizeof(unsigned *) * (*ansc + *curc)) ;
        if(temp2 == NULL)
        {
            // 解が減るだけなのでそのまま返してもいいが、一応exitでプログラム終了。
            perror("Failed to reallocate memory") ;
            exit(1) ;
        }
        *ansv = temp2 ;

        for(i = 0 ; i < *curc ; i++)
        {
            (*ansv)[i + *ansc] = (unsigned *)malloc(sizeof(unsigned) * len) ;
            ui_cpy(len, (*curv)[i], (*ansv)[i + *ansc]) ;
        }

        *ansc += *curc ;
    }
}

// fgetsの末尾の0Aを消す。
void remove_0A(char *string)
{
    if(string[length_string(string) - 1] == '\n')
        string[length_string(string) - 1] = '\0' ;
}