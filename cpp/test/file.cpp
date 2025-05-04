
#include <iostream>
#include <stdio.h>

#include "file.hpp"

int main()
{
    // environ()
    std::cout << file::environ("RINDFILE_PATH") << "\n" ;

    // write()
    //file fp("a.txt", "x") ; 
    //fp.write("abcd", 4) ;
    //std::cout << "generated a.txt\n" ;

    // makedir()
    file::makedir("sum") ;

    // seek(), tell(), size() writeと同じだとうまくいかない。file.hpp参照。
    file a("a.txt") ; 
    std::cout << a.size() << "\n" ;
}

/*
file fp("a.txt", "x") ;
fp.write("a", 1) ; 
　としたところ、
str::str(): 'r' (0x556cc423aeb0)
str::str(): 'x' (0x556cc423b2e0)
str::~str(): 'x' (0x556cc423b2e0)
str::~str(): 'r' (0x556cc423aeb0)
str::str(): 'x' (0x556cc423aeb0)
str::str(): 'x' (0x556cc423b2e0)
str::~str(): 'x' (0x556cc423b2e0)
str::~str(): 'x' (0x556cc423aeb0)
file::file(): 0x556cc423b300
str::str(): 'x' (0x556cc423aeb0)
str::str(): 'x' (0x556cc423b2e0)
str::~str(): 'x' (0x556cc423b2e0)
str::~str(): 'x' (0x556cc423aeb0)
file::~file(): 0x556cc423b300
　まず、str(mode) == str("r")でxとrが作られて解放。rが先なので右辺が先に確保されるのは意外。次に、str(mode) == str("x")でxとxが作られて解放。そして、fileが開かれる。writeでstr(mode) != str("x")でxとxが作られて解放。そしてfwriteして、ファイルを解放。ちゃんと思い通りに動いている。
*/

