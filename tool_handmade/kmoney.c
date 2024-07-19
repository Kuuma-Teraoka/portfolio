#include <stdio.h>
#include <unistd.h>
#define PATH_SIZE 128
#define STRINGS_SIZE 128

int combert(char *, int) ;
int kexp(int, int) ;
void sumStrings(char *, char *, char *, int) ;
int nullNum(char *, int) ;
int commaNum(char *, int) ;
int find(char *, char *) ;

int main(int argc, char *argv[])
{
    FILE *csv ;

    char oneLIne[STRINGS_SIZE] ;
    int sumAll, sum ;

    if(argc != 4)
    {
        printf("argc != 3. please input 3 word such as \"$ kmoney a.csv suica transpotation\".\n\n") ;
        return 0 ;
    }


    char cwd[PATH_SIZE] ;
    getcwd(cwd, PATH_SIZE) ;
    char csvPath[nullNum(cwd, 0) + nullNum(argv[1], 0) + 2] ;
    sumStrings(cwd, argv[1], csvPath, 0) ;



    if((csv = fopen(csvPath, "r")) == 0)
    {
        printf("fopen returned NULL. file name is different, or file don't exist\n\n") ;
        return 0 ;
    }

    sumAll = 0 ;

    // 支払い総額,利用日,..の行をスキップ。
    fgets(oneLIne, STRINGS_SIZE, csv) ;

    // 1234,のとき、commaNumは5ではなく、4であることに注意。
    while(fgets(oneLIne, STRINGS_SIZE, csv) != NULL) if(find(oneLIne, "合計金額")) sumAll += combert(oneLIne, commaNum(oneLIne, 1)) ;

    fclose(csv) ;





    csv = fopen(csvPath, "r") ;


    sum = 0 ;

    // percentは未実装
    // 支払い総額,利用日,..の行をスキップ。
    // fgets(oneLIne, STRINGS_SIZE, csv) ;
    // printf("支払総額,percent,利用店名・商品名,利用日\n") ;

    while(fgets(oneLIne, STRINGS_SIZE, csv) != NULL)
    {
        if(find(oneLIne, argv[2]) && find(oneLIne, "合計金額")) printf("%s", oneLIne) ;
        // fgetsはgetsとは違い\nを含むので、,1234\n\0となっていることに注意。順番変えたので関係なくなったが、一応残しておく。
        // else sum += combert(&oneLIne[commaNum(oneLIne, 2)+1], nullNum(oneLIne, 0) - commaNum(oneLIne, 2) - 2) ;
        else if(find(oneLIne, "合計金額")) sum += combert(oneLIne, commaNum(oneLIne, 1)) ;
    }

    printf("%d,%s,sum\n", sum, argv[3]) ;
    printf("合計金額 : %d\n", sumAll) ;





    fclose(csv) ;

    return 0 ;
}



// charポインタと整数値を引数に、そのポインタを含めた整数値の長さの文字列を数字に変換し返す関数
int combert(char strings[], int num)
{
    register int i, j ;
    int sum ;

    sum = 0 ;
    // 5401であれば、5*10^3,4*10^2,0*10^1,1*10^0、num=4に注意して、strings[0]*10^3の3はnum-1-i。0x31は1。
    for(i = 0 ; i < num ; i++) sum += (strings[i]-0x30) * kexp(10, num-1-i) ;

    return sum ;
}

// 10と4を受け取ったら、10^4を返す関数
int kexp(int a, int b)
{
    register int i ;
    int sum ;

    sum = 1 ;
    for(i = 0 ; i < b ; i++) sum *= a ;

    return sum ;
}


void sumStrings(char *cwdPath, char *relativePath, char *sum, int mode)
{
    register int i ;
    // 毎回nullNumを呼び出すとそのたびに処理が行われるので、cpuよりメモリを使おう。
    int longCwdPath = nullNum(cwdPath, 0) ;
    int longRelativePath = nullNum(relativePath, 0) ;


    if(mode == 1)
    {
        for(i = 0 ; i < longCwdPath ; i++) sum[i] = cwdPath[i] ;
        for(i = 0 ; i < longRelativePath ; i++) sum[longCwdPath+i] = relativePath[i] ; 
        sum[longCwdPath+longRelativePath] = '\0' ;
    }
    else if(relativePath[0] == '.' && relativePath[1] == '.' && relativePath[2] == '/') printf("sorry\n") ;
    // 絶対パスに対応。
    else if(relativePath[0] == '/')
    {
        for(i = 0 ; i < longRelativePath + 1 ; i++) sum[i] = relativePath[i] ;
    }
    else if(relativePath[0] == '.' && relativePath[1] == '/')
    {    
        // cwdPathいれる
        for(i = 0 ; i < longCwdPath ; i++) sum[i] = cwdPath[i] ;
        // .入れず、/から入れる。ちゃんとヌルコードまで入れる。sum[longCwd + 1 - 1]から。
        for(i = 1 ; i < longRelativePath + 1 ; i++) sum[longCwdPath + i - 1] = relativePath[i] ;
    }
    else
    {
        // cwdPathいれる
        for(i = 0 ; i < longCwdPath ; i++) sum[i] = cwdPath[i] ;
        // /いれる
        sum[longCwdPath] = '/' ;
        // relativePath入れる、ちゃんとヌルコードまで入れる。
        for(i = 0 ; i < longRelativePath +1 ; i++) sum[longCwdPath + 1 + i] = relativePath[i] ;
    }
}

// 1なら内容を表示、ついでに末尾のヌルコードとその後のメモリ2byte分も表示。[5]がnullなら5を返す、0-4で文字数も5。
int nullNum(char *strings, int mode)
{
    register int i ;
    for(i = 0 ; strings[i] != 0 ; i++) if(mode) printf("0x%X:%c\n", strings[i], strings[i]) ;
    if(mode)
    {
        printf("0x%X:%c\n", strings[i], strings[i]) ;
        printf("0x%X:%c\n", strings[i+1], strings[i+1]) ;
        printf("0x%X:%c\n", strings[i+2], strings[i+2]) ;
    }
    return i ;
}

// 文字列と整数を引数に、文字列内の整数番目のコンマの位置を返す関数。abcd,ac\0であれば、4を返す。
int commaNum(char *strings, int num)
{
    register int i, j ;
    
    // i=-1,i++,ifcommaj,ifij,i++,ifcommaj,ifij,...,i++,ifcommaj=num,out
    i = -1 ;
    j = 0 ;
    do
    {
        i++ ;
        if(strings[i] == ',') j++ ;
    } while (j != num) ;
    
    return i ;
}

// 文字列と、argv[2], youtubeという文字列が与えられたら、後者が存在すれば0、しなければ1を返す関数。
// lineが\0だけでも、つまり文字数0でもちゃんと1を返す。
int find(char *line, char *key)
{
    int longLine = nullNum(line, 0) ;
    int longKey = nullNum(key, 0) ;
    register int i, j ;

    // null含めずlineが10、keyが4であれば、0-3から6-9まで。
    for(i = 0 ; i <= longLine-longKey ; i++)
    {
        for(j = 0 ; j < longKey ; j++)
        {
            if(line[i+j] != key[j]) break ; // ここでflagを0にするとかせず、アルゴリズムでなんとかした。kutwavにも使えるかも。
        }
        if(j == longKey) return 0 ;
    }
    return 1 ;
}
