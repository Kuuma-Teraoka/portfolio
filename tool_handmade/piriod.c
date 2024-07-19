
/*
sedでよくね？
*/
// FILE
#include <stdio.h>
// malloc()
#include <stdlib.h>

// myrealpath()
#include <file.h>

#include <bin.h>

int main(int argc, char *argv)
{
    if(argc == 0)
    {
        char *std_line ;

        std_line = (char *)malloc(sizeof(char) * 1024 * 1024) ;

        while(fgets(std_line, 1024 * 1024, stdin) != NULL)
        {
            if(ends_with_0A(std_line))
            {
                perror("Error: The line could not be fully read because it does not end with a newline character (0A) and the buffer size is insufficient.") ;
                return 1 ;
            }


        }

    }

    else if(argc == 1)
    {
        FILE *input_file ;
        char *input_path = myrealpath(argv[1]) ;
        if((input_file = fopen(input_path, "r")) == NULL)
        {
            printf("fopen returned NULL. file name is different, or file don't exist in current directory\n") ;
            return 1 ;
        }
    }

    
}

// 31 32 0A 00なら0, 31 32 00なら1を返す関数。fgetsでバッファが足りず一行すべてを読み込めなかったとき。3byte文字がセパレートされてしまうかもしれないので。
int ends_with_0A(char *string)
{
    if(string[length_string(string) - 1] == '\n')
        return 0 ;
    else
        return 1 ;
}