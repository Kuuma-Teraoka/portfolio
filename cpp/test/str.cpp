
#include "str.hpp"
#include "slice.hpp"

#include <iostream>
#include <stdio.h>

int main()
{ 
    // コピー初期化。どちらも同じコピー初期化のコンストラクタが呼ばれる。
    str a("a") ;
    str b = "b" ;

    // str &operator=(const char *)
    std::cout << (b = "a") << "\n" ;
    // str &operator=(const str &)
    std::cout << (b = a) << "\n" ;

    // friend str operator+(const str &, const char *)
    // friend str operator+(const char *, const str &)
    std::cout << "\x1b[31mred" + str(" a ") + "\x1b[0mnone" << "\n" ; 

    // friend str operator*(const str &, const int) 
    std::cout << str("━") * 30 << "\n" ; 

    // endswith()
    std::cout << "str(\"tmp.mat\").endswith(\".mat\"): " << str("tmp.mat").endswith(".mat") << "\n" ;
    std::cout << "str(\"tmp.mat\").endswith(\".fmat\"): " << str("tmp.mat").endswith(".fmat") << "\n" ;

    // operator==
    std::cout << (str("abc") == "abc") << "\n" ;

    // operator[](slice)
    std::cout << str("abcde")[slice(2,4)] << "\n" ;

    // find()
    std::cout << str("abcdefg").find("cdea") << "\n" ;

    // split()
    for(str i : str("abc:sd:sg:yw").split(","))
        std::cout << i << ", " ;
    std::cout << "\n" ;

    // startwith()
    std::cout << str("abcde").startwith("a") << "\n" ;

    // str(void *, int)
    std::cout << str(str("fghijklm").strings, str("fghijklm").len) << "\n" ;
 
}

/*
　operator+でchar*を返すとfreeしないと解放されないので、strを返すようにした。
str("1234") + str("abc")
　で、char*のときは、
str::str(): 'abc' (0x55b9a9174eb0)
str::str(): '1234' (0x55b9a91752e0)
1234abc
str::~str(): '1234' (0x55b9a91752e0)
str::~str(): 'abc' (0x55b9a9174eb0)
　で、strにすると、
str::str(): 'abc' (0x55681664ceb0)
str::str(): '1234' (0x55681664d2e0)
str::str(): '1234abc' (0x55681664d320)
'1234abc'
str::~str(): '1234abc' (0x55681664d320)
str::~str(): '1234' (0x55681664d2e0)
str::~str(): 'abc' (0x55681664ceb0)
　なので、ちゃんと解放できている。
*/






