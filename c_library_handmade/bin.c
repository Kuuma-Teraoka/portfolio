#include <stdio.h>
#include <stdlib.h>

#include <bin.h>

// 61 62 63 0A 00と続いたら4を返す関数。fgetsだと0Aが含まれるので文字数+1になることに注意。
unsigned int length_string(char array[])
{
    unsigned int i ;

    i = 0 ;
    while(array[i] != '\0')
    {
        if(i > 2147483647)
        {
            perror("no NULL for 2147483647") ;
            return 2147483647 ;
        }
        i++ ;
    }

    return i ;
}

/*
string count 2
　を
styring
count
2
　と20||0A||00区切りで分割する関数。

argc:argument count(引数の個数)
argv:argument vector(引数の配列)
　から
strc:string count(文字列の個数)
strv:string vector(文字列の配列)
　という命名にしている。

***strv
　がややこしいので整理する。
31 32 33 00 
　というchar型のデータが格納されたメモリの先頭が01 00番地だったとして、
01 00 02 00 03 00
　というように、番地を格納されたメモリを用意する。そしてその先頭の01 00が01 80番地だったとする。返すのは01 80と、それが指すメモリの容量である3という数字である。つまり、unsigend intとchar **を返す。ただ、返すのが1変数ではないので、引数にポインタを与えて書き換えてもらうようにしているので、それぞれのポインタである、unsigned int *とchar ***が引数にある。
*/
void str_split(unsigned int numInput, char *input, unsigned int *strc, char ***strv)
{
    unsigned int currentInput, currentInput2, currentOutput, i ;


    for(*strc = currentInput = 0 ; ;)
    {

        while(input[currentInput] == ' ' || input[currentInput] == '\n' || input[currentInput] == '\0')
        {
            currentInput++ ;
            if(currentInput >= numInput) break ;
            // 20 00<-currentInput 0A 31 33 36 20 0A 31 33 | 57 AF 0A 00
        }
        if(currentInput >= numInput) break ;

        // 20 00 0A 31<-currentInput 33 36 20 0A 31 33 | 57 AF 0A 00
        (*strc)++ ;

        while(!(input[currentInput] == ' ' || input[currentInput] == '\n' || input[currentInput] == '\0'))
        {
            currentInput++ ;
            if(currentInput >= numInput) break ;
            // 20 00 0A 31 33<-currentInput 36 20 0A 31 33 | 57 AF 0A 00
        }
        if(currentInput >= numInput) break ;

        // 20 00 0A 31 33 36 20<-currentInput 0A 31 33 | 57 AF 0A 00
    }

    *strv = (char **)malloc(sizeof(char *) * (*strc)) ;

    for(currentInput2 = currentOutput = 0 ; currentOutput < *strc ; currentOutput++)
    {
        /*
        20 00 0A 31 33 36 20 0A 31 33 20 0A | 00
        　という場合でも、*strc=2なので、31 33 36 00, 31 33 00の二つを読み取ったらすでにループを抜けているはず。
        20 00 0A 31 33 36 20 0A 31 33 20 0A<-currentInput2 | 00
        20 00 0A 31 33 36 20 0A 31 33 20 0A | 00<-currentInput2
        　とnumInputを超えることはない。
        */
        while(input[currentInput2] == ' ' || input[currentInput2] == '\n' || input[currentInput2] == '\0') currentInput2++ ;

        currentInput = currentInput2 ;

        // 20 00 0A 31<-currentInput,currentInput2 33 36 20 0A 31 33 | 57 AF 0A 00

        /*
        20 00 0A 31 33 36 20 0A 31 33<-currentInput2 | 57 AF 0A 00
        20 00 0A 31 33 36 20 0A 31 33 | 57<-currentInput2 AF 0A 00
        　というようにnumInputを超えて進んでしまうのでちゃんとifでbreakする。なお、この下にあるのは文字列を入れる処理なので、二重ループまでbreakしなくていい。このエラーでyahooアルゴリズム問題の1時間半すべて使った。
        */
        while(!(input[currentInput2] == ' ' || input[currentInput2] == '\n' || input[currentInput2] == '\0'))
        {
            currentInput2++ ;
            if(currentInput2 >= numInput) break ;
        }
            

        // 20 00 0A 31<-currentInput 33 36 20<-currentInput2 0A 31 33 | 57 AF 0A 00

        // NULLのため+1
        (*strv)[currentOutput] = (char *)malloc(sizeof(char) * (currentInput2 - currentInput + 1)) ;

        for(i = 0 ; i < currentInput2 - currentInput ; i++) (*strv)[currentOutput][i] = input[currentInput + i] ;
        (*strv)[currentOutput][i] = '\0' ;

        currentInput = currentInput2 ;

        // 20 00 0A 31 33 36 20<-currentInput,currentInput2 0A 31 33 | 57 AF 0A 00
    }
}


// 31 32 00 から、12を返す関数。検知機能はなし。31 32 0A 00なら0A-30=-26=-38(2)で、120+(-38)=82になる。(実験済み)
unsigned int str_to_ui(char *a)
{
    unsigned int i, sum ;

    for(sum = i = 0 ; i < length_string(a) ; i++) sum = sum * 10 + (a[i] - '0') ;

    return sum ;
}

// 2D(-) 31 32 00から、-12を返す関数。1バイト目が-かどうかで全体に-をかけるだけで、str_to_uiと同じ。つまり、2D 31 32 0A 00なら-82になる。（実験済み）
int str_to_i(char *a)
{
    int sum, sign ;
    unsigned int i ;

    sign = 1 ;
    sum = 0 ;
    i = 0 ;
    if(a[i] == '-')
    {
        sign = -1 ;
        i++ ;
    }
    for(; i < length_string(a) ; i++) sum = sum * 10 + (a[i] - '0') ;

    return (sign * sum) ;
}

/*
https://teratail.com/questions/246705
　より、string.hのcharをintに変換するatoi関数は次のコードらしい。
int atoi(const char *s)
{
	int n=0, neg=0;
	while (isspace(*s)) s++;
	switch (*s) {
	case '-': neg=1;
	case '+': s++;
	}
	// Compute n as a negative number to avoid overflow on INT_MIN
	while (isdigit(*s))
		n = 10*n - (*s++ - '0');
	return neg ? n : -n;
}
　intのMINである-2147483648を変換するときにint型に一時的に2147483648が入り、上限を超えてMINになり、マイナスをかけて+2147483648で上限超えてMIN、あれ問題ないな。とりあえずこれは問題で、そのためatoiは累計計算を負数で行い最後に符号反転するというトリッキーな実装になっているとのこと。
*/

// 文字列を受け取って、NULLまでのバイナリをdoubleに入れる関数。jあたりがややこしく、0-1、つまりMAXで初期化しておいて、.が来たら0にする。MAXでなくなったら数字を読み込むたびに+1する。最後にj回10で割る。つまりjにカウンタとフラッグを兼任させている。.が来なければj=MAXのままで、その場合は10で割らない。jが使える範囲が[0, MAX]から[0, MAX-1]になるが問題ないだろう。
double str_to_d(char *s)
{
    int sign ;
    double sum ;
    unsigned int i, j ;

    sign = 1 ;
    sum = 0 ;
    i = 0 ;
    j = 0 - 1 ;
    if(s[i] == '-')
    {
        sign = -1 ;
        i++ ;
    }
    for( ; i < length_string(s) ; i++)
    {
        if(s[i] == '.') j = 0 ;
        else
        {
            sum = sum * 10 + (s[i] - '0') ;
            if(j != 0 - 1) j++ ;
        }
    }

    if(j != 0 - 1) for(i = 0 ; i < j ; i++) sum /= 10 ;

    return sign * sum ;
}

// 文字列が完全に一致しているか確認する関数。一致すれば1、しなければ0を返す。31 33 34 20, 31 33 34 35, 3が与えられれば1を返す。
int str_cmp(char *a, char *b, unsigned int size)
{
    unsigned int i ;
    for(i = 0 ; i < size ; i++) if(a[i] != b[i]) return 0 ;
    return 1 ;
}

// 文字列をコピーする関数。inputからnum_input分のバイナリをoutputにコピーし、末尾に00をつける。outputにnum_input + 1の容量がないとオーバーフローを起こすので注意。
void str_cpy(char *input, unsigned int num_input, char *output)
{
    unsigned int i ;

    for(i = 0 ; i < num_input ; i++)
        output[i] = input[i] ;
    
    output[num_input] = '\0' ;
}

/*
　二つのポインタを受け取って、srcからn[byte]をdestにコピーする関数。標準ライブラリにもあり、
void *memcpy(void *dest, const void *src, size_t n);
　である。また、javaにもあり、
public static void arraycopy(Object src, int srcPos, Object dest, int destPos, int length)
　である。src, destの順のほうがすきなので、この順にした。
*/
void memory_copy(void *src, void *dest, size_t n)
{
    unsigned int i ;

    for(i = 0 ; i < n ; i++)
        ((unsigned char *)dest)[i] = ((unsigned char *)src)[i] ;
}

// 31 33 00, 34 35 36 20 00を受け取ったら、length_stringをもとに2+4+1[bytes]を確保して、31 33 34 35 36 20 00を入れて返す関数。
char *str_merge(char *a, char *b)
{
    char *merged_strings ;
    unsigned int i, num_a, num_b ;

    num_a = length_string(a) ;
    num_b = length_string(b) ;
    merged_strings = (char *)malloc(sizeof(char) * (num_a + num_b + 1)) ;

    for(i = 0 ; i < num_a ; i++) merged_strings[i] = a[i] ;
    for(i = 0 ; i < num_b ; i++) merged_strings[num_a + i] = b[i] ;
    merged_strings[num_a + num_b] = '\0' ;

    return merged_strings ;
}

void print_bin(void *a, unsigned int b)
{
    unsigned int i ;

    printf("Bi | 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10\n") ;
    printf("----------------------------------------------------") ;
    for(i = 0 ; i < b ; i++)
    {
        if(i % 16 == 0) printf("\n%X%X |", (i / 16) / 16, (i / 16) % 16) ;
        printf(" %X%X", ((unsigned char *)a)[i] / 16, ((unsigned char *)a)[i] % 16) ;
    }
    printf("\n") ;
}

// ポインタと容量を受け取って、"23 3A 23 23"という文字列をmallocで確保したメモリに入れて返す関数。\0を末尾につけるので、mallocのサイズはちょうどbyte*3になる。
char *str_bin(void *point, unsigned int byte)
{

}

// unsigned int型配列の先頭のポインタと要素数を受け取って、すべて異なるなら0、同じものが含まれているなら1を返す関数。配列の要素が10なら10C2通りのすべてのパターンで同じではないかチェックして、同じならフラグを立てる。
int checkDifferArray(unsigned int *array, unsigned int num)
{
    unsigned int i, j, flag ;
    flag = 0 ;

    for(i = 1 ; i < num ; i++) for(j = 0 ; j < i ; j++) if(array[i] == array[j]) flag = 1 ;

    return flag ;
}

// unsigned int型配列をlenの文コピーするもの。中身だけコピーするので、mallocする必要がある。
void ui_cpy(unsigned int len, unsigned int *input, unsigned int *output)
{
    unsigned int i ;
    for(i = 0 ; i < len ; i++)
        output[i] = input[i] ;
}


