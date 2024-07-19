#include <stdio.h>
#include <unistd.h>
#define PATH_SIZE 128
#define LINE_SIZE 256

int find(char *, char *) ;
int limitFgets(char *, int, FILE *) ;
void sumStrings(char *, char *, char *, int) ;
int nullNum(char *, int) ;

int main(int argc, char *argv[])
{
    FILE *html ;
    FILE *sh ;

    char oneLineHtml[LINE_SIZE] ;
    oneLineHtml[LINE_SIZE-1] = '\0' ; // ffgetsで、もしこの値が変わっていたら上限に達している。

    short level ;  // フォルダ始まりで+、終わりで-で、argv[2]を出たら終了。







    printf("\n") ;

    // argv[1]がないと、pathの設定でsegmentation faultになるため、先にこっち。
    if(argc != 3)
    {
        printf("argc != 3. please input 3 word such as \"$ bmdl a.html youtube\".\n\n") ;
        return 0 ;
    }

    char cwd[PATH_SIZE] ;
    getcwd(cwd, PATH_SIZE) ;
    // /0と/を追加するので+2
    char htmlPath[nullNum(cwd, 0) + nullNum(argv[1], 0)+2] ;
    sumStrings(cwd, argv[1], htmlPath, 0) ;

    char relativeSh[] = "bmdl.sh" ;
    char shPath[nullNum(cwd, 0) + nullNum(relativeSh, 0)+2] ;
    sumStrings(cwd, relativeSh, shPath, 0) ;

    // youtubeという文字列から、youtube</H3>をつくる。
    char skipKey[nullNum(argv[2], 0)+6] ;
    sumStrings(argv[2], "</H3>", skipKey, 1) ;


    if((html = fopen(htmlPath, "r")) == 0)
    {
        printf("fopen returned NULL. file name is different, or file don't exist\n\n") ;
        return 0 ;
    }
    sh = fopen(shPath, "w") ;





    oneLineHtml[0] = '0' ;  // 初期化。メモリの残骸にyoutube</H3>があるかもしれないから。

    // fget, if, fget, if, fget(youtube), out
    // 
    
    do
    {
        if(limitFgets(oneLineHtml, LINE_SIZE, html) != 1)
        {
            printf("not found this folder or one line above LINE limit\n\n") ;
            return 0 ;
        }
    }while(find(oneLineHtml, skipKey)) ;



    fputs(oneLineHtml, sh) ;  // <DT><H3 ADD_DATE="" LAST_MODIFIED="">youtube</H3>

    //  youtube</H3>のあとは確定で<DL><p>なので意味はないが一応。
    if(limitFgets(oneLineHtml, LINE_SIZE, html) != 1 && find(oneLineHtml, "<DL><p>"))
    {
        printf("line above Line limit, or not <DL><p> next to youtube</H3>") ;
        return 0 ;
    }
    fputs(oneLineHtml, sh) ;  // <DL><p>



    level = 1 ;
    // fgets, iflebel, fputs, if, ..., fgets(youtubeの</DL>), iflebel(lebel = 0), fputs, out
    while(level)
    {
        if(limitFgets(oneLineHtml, LINE_SIZE, html) != 1)
        {
            printf("file end before folder end. so bookmark file is broken. or one line above LINE limit\n\n") ;
            return 0 ;
        }
        if(find(oneLineHtml, "<DL><p>") == 0) level++ ;
        if(find(oneLineHtml, "</DL><p>") == 0) level-- ;
        fputs(oneLineHtml, sh) ;
    }

    fclose(html) ;
    fclose(sh) ;


    printf("generated bmdl.sh. thankyou\n\n") ;


    return 0 ;
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

// fgetsはchar *fgetsの形。ファイル終了でNULLを返すが、文字のポインタ=NULLとはいったい。
// 最後尾に0をいれていたはずが、文字が入ってしまった場合、プログラミングが機能しないため、終了。それをmainにしらせるため、その場合2を返すint型にする。
// fgetsの代わりにこれを使えば、このようなことはなくなる。
int limitFgets(char *str, int size, FILE *file_ptr)
{
    if(fgets(str,size, file_ptr) == NULL) return 0;
    else if(str[LINE_SIZE-1] != '\0') return 2;
    else return 1 ;
}



// sumに、cwdPath、/、relativePath、\0を入れる関数。
// mode追加。1のときはただつなげるだけ。0のときはpath作成。
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