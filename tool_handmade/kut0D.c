#include <stdio.h>
#include <unistd.h>
#define PATH_SIZE 128

void sumStrings(char *, char *, char *, int) ;
int nullNum(char *, int) ;

int main(int argc, char *argv[])
{
    FILE *from ;
    FILE *to ;

    int c ;


    printf("\n") ;

    if(argc != 2)
    {
        printf("argc != 2. please input 2 word such as \"$ kut0D a.txt\".\n\n") ;
        return 0 ;
    }


    char cwd[PATH_SIZE] ;
    getcwd(cwd, PATH_SIZE) ;
    char fromPath[nullNum(cwd, 0) + nullNum(argv[1], 0) + 2] ;
    sumStrings(cwd, argv[1], fromPath, 0) ;

    char relativeTo[] = "modified.txt" ;
    char toPath[nullNum(cwd, 0) + nullNum(relativeTo, 0) + 2] ;
    sumStrings(cwd, relativeTo, toPath, 0) ;

    if((from = fopen(fromPath, "r")) == 0)
    {
        printf("fopen returned NULL. file name is different, or file don't exist\n\n") ;
        return 0 ;
    }
    to = fopen(toPath, "w") ;

    while((c = getc(from)) != EOF) if(c != 0x0D) putc(c, to) ;

    fclose(from) ;
    fclose(to) ;

    printf("complete deleting 0D\n\n") ;

    return 0 ;
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
    register int i = 0 ;
    for(i = 0 ; strings[i] != 0 ; i++) if(mode) printf("%d:%c\n", strings[i], strings[i]) ;
    if(mode)
    {
        printf("%d:%c\n", strings[i], strings[i]) ;
        printf("%d:%c\n", strings[i+1], strings[i+1]) ;
        printf("%d:%c\n", strings[i+2], strings[i+2]) ;
    }
    return i ;
}