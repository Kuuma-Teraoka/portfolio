
// getcwd()
#include <unistd.h>
// malloc()
#include <stdlib.h>


#include <file.h>
// str_cmp
#include <bin.h>



/*
　相対パスを引数として受け取って、絶対パスを返す関数。ヌルコードでデータ長は調べられるので、char *, intの二つを返す必要はない。
　ながいカレントディレクトリのパスで、
/mnt/d/data/stady/academicAbility/university/lecture_to_study/past/GESc
　くらいで、この文字数が71+NULLなので、1024もあれば十分だと思う。

　新しい文字列をmallocで確 保して返す。mergeを使っている場合、merge関数でmallocを使っているためそのまま返せばいい。
*/
char *myrealpath(char *relative_path)
{
    char cwd_path[1024] ;
    
    if(getcwd(cwd_path, 1024) == NULL)
        return NULL ;

    if(relative_path[0] == '/')
    {
        // 絶対パスだったとき。
        char *target ;
        unsigned int i ;
        target = (char *)malloc(sizeof(char) * (length_string(relative_path) + 1)) ;
        for(i = 0 ; i < length_string(relative_path) + 1 ; i++)
            target[i] = relative_path[i] ;
        return target ;
    }
    else if(str_cmp(relative_path, "./", 2))
    {
        // ./a.txt
        return str_merge(cwd_path, relative_path + 1) ;
    }
    else if(str_cmp(relative_path, "../", 3))
    {
        // ../a.txt
    }
    else if(str_cmp(relative_path, "~/", 2))
    {
        // ~/a.txt
        
    }
    else
    {
        // a.txt
        char *tmp, *tmp2 ;
        tmp = str_merge(cwd_path, "/") ;
        tmp2 = str_merge(tmp, relative_path) ;
        free(tmp) ;
        return tmp2 ;
    }
}
