#include "mymath.h"


unsigned int stare(unsigned int a)
{
    if(a == 0 || a == 1) return 1 ;
    else return (a * stare(a - 1)) ;
}

// (2,4)のような値を受け取ると、2, 1, 0, FFFFFFFFが乗算されて、0になり破綻するので注意。あと、なぜか(2, 4)を受け取った時にsegmentation faultになるときとならないときがあり、原因は不明。
unsigned int combination(unsigned int n, unsigned int r)
{
    unsigned int i, sum ;

    sum = 1 ;
    for(i = 0 ; i < r ; i++)
    {
        sum *= n ;
        n-- ;
    }

    return (sum / stare(r)) ;
}

/*
// 再起利用。
unsigned int combination(unsigned int n, unsigned int r)
{
    if(n == r) return 1 ;
    else if(n == 1) return n ;
    else return (combination(n - 1, r - 1) + combination(n - 1, r)) ;
}
*/



double myexp(double x) {
    double result = 1.0; // 初期値は1
    double term = 1.0;
    int i;
    
    // テイラー展開を使用した指数関数の近似計算
    for (i = 1; i <= 10; ++i) {
        term *= x / i;
        result += term;
    }
    
    return result;
}

double mylog(double x) {
    double result = 0.0;
    double term = (x - 1) / (x + 1);
    double term_squared = term * term;
    double numerator = term;
    int i;
    
    // テイラー展開を使用した自然対数関数の近似計算
    for (i = 1; i <= 10; ++i) {
        result += numerator / (2 * i - 1);
        numerator *= term_squared;
    }
    
    return 2 * result;
}

// 二つの値のうち小さいほうを返す関数。
unsigned int min(unsigned int a, unsigned int b)
{
    if(a > b)
        return b ;
    else
        return a ;
}
