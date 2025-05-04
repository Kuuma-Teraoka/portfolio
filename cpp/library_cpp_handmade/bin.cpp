

#include <iostream> // std::malloc()
#include "bin.hpp"




void *bin::copy(void *input, unsigned int size)
{
    void *output = (void *)std::malloc(size) ;
    for(unsigned int i = 0 ; i < size ; i++)
        ((unsigned char *)output)[i] = ((unsigned char *)input)[i] ;
    return output ;
}

 /*
　bin_strでもいいのだが、printするのは決まっているのに、strで返すと、
char *str = bin_str(input, size) ; 
printf("%s\n", str) ;
free(str) ;
　と3行必要になる。
str_print(input, size) ;
　だけのほうが使い勝手がいいので、printにする。まあ、strを作って、printをstrで作るのがいいだろうが、それはあとでやろう。
*/
void bin::print(const void *input, const unsigned int size)
{
    printf("Bi | 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10\n") ;
    printf("----------------------------------------------------") ;
    
    for(unsigned int i = 0 ; i < size ; i++)
    {
        if(i % 16 == 0)
            printf("\n%X%X |", (i / 16) / 16, (i / 16) % 16) ;
        printf(" %X%X", ((unsigned char *)input)[i] / 16, ((unsigned char *)input)[i] % 16) ;
    }
    printf("\n") ;
}




