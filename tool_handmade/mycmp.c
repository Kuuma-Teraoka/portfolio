/*
　cmp自作。
cmp a.txt b.txt
　で比較するのは変わらないが、
cmp a.txt b.txt -n 3
　とすると、それぞれ、
31 31
32 33
34 34
20 00
00 20
33 33
34 35
　というバイナリだったときに、3つまで違うところを
1:32 33
3:20 00
4:00 20
5~:skip
　と標準出力する。また、完全バイナリ一致の際は、
compelely equal binary
　と出力する。
*/

#include <stdio.h>
#include <stdlib.h>

#include <file.h>

char *myrealpath(char *) ;

int main(int argc, char *argv[])
{
    myrealpath(NULL) ;

}
