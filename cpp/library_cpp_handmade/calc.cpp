

#include <stdexcept> // std::invalid_argument
#include <cmath> // std::tanh()

#include "calc.hpp"


// numを受け取って、1 * 2 * 3 * , ..., * num、つまりnumの階乗(factorial)を求める関数。
unsigned int calc::fact(unsigned int num)
{
    /*
    if(num == 0 || num == 1)
        return 1 ;
    else
        return (num * fact(num - 1)) ;
    */
    unsigned int sum = 1 ;
    for(int i = 0 ; i < num ; i++)
        sum *= i + 1 ;
    return sum ;
}

// nCrを求める関数。fact(50)とかだとunsigned intを超えて0になってしまうので、ここで計算する。
unsigned int calc::comb(unsigned int n, unsigned int r)
{
    if(n < r)
        throw std::invalid_argument("comb(): n < r") ;

    unsigned int sum = 1 ;
    for(unsigned int i = 0 ; i < r ; i++)
    {
        sum *= n ;
        n-- ;
        //sum /= i + 1 ; // ここに組み込んでいい？
    }
    // sum / fact(r)を計算
    for(int i = 0 ; i < r ; i++)
        sum /= i + 1 ;


    return sum ;
}

/*
// 再起利用。
unsigned int calc::combination(unsigned int n, unsigned int r)
{
    if(n == r) return 1 ;
    else if(n == 1) return n ;
    else return (combination(n - 1, r - 1) + combination(n - 1, r)) ;
}
*/



// aが与えられたら\sqrt{a}を返す関数。3が与えられたら、まず3*1の長方形を考える。3と1の平均2を一辺にして、3/2をもう一辺もつ長方形は面積は変わっていない。これを繰り返すと面積が同じの正方形に近づく。よって、辺が求めたい平方根になる。
double calc::sqrt(double a)
{
    double x = a ;
    double y = 1 ;
    while(x - y > 0.0000001)
    {
        x = (x + y) / 2 ;
        y = a / x ;
        //std::cout << x << "," << y << "\n" ;
    }
    return x ;
}


// 3と3が与えられたら、3*3で27を返す関数。
double calc::expint(double x, unsigned int y)
{
    double result = 1 ;
    for(unsigned int i = 0 ; i < y ; i++)
        result *= x ;
    return result ;
}

// chatGPTのやつだったと思う。e^xを返す関数。xがintならforで*eを繰り返せば求められるが、xはdoubleであるのが問題。
double calc::exp(double x)
{
    double result = 1.0; // 初期値は1
    double term = 1.0;
    
    // テイラー展開を使用した指数関数の近似計算
    for(unsigned int i = 1 ; i <= 10 ; ++i)
    {
        term *= x / i ;
        result += term ;
    }
    
    return result ;
}

double calc::log(double x)
{
    double result = 0.0;
    double term = (x - 1) / (x + 1);
    double term_squared = term * term;
    double numerator = term;
    
    // テイラー展開を使用した自然対数関数の近似計算
    for(unsigned int i = 1 ; i <= 10 ; ++i)
    {
        result += numerator / (2 * i - 1) ;
        numerator *= term_squared ;
    }
    
    return 2 * result ;
}



/*
　sinh(x)を返す関数。双曲線関数については数学のメモでまとめておきたい。関数自体は単純で、
sinh(x) = (e^x-e^{-x}) / 2
cosh(x) = (e^x+e^{-x}) / 2
tanh(x) = sinh(x) / cosh(x)
*/
double calc::sinh(double x)
    {return (exp(x) - exp(-x)) / 2 ;}

// cosh(x)を返す関数
double calc::cosh(double x)
    {return (exp(x) + exp(-x)) / 2 ;}

// cppでのリライトでリザバーが上手くできなくて、入力次元3*2、ニューロン数8、時系列長10でひとつずつ行列が同じ確認。w_xの結合の数が少なすぎて固有値によるスケーリングでinfになっていて、cppのblasの行列積による仕様か、x1の一部が-nan、なぜかnumpyは1.0になっていた。w_xの結合を24にしたらnanが出ることもなくなり、cppもnumpyもだいたい同じ値に。つまり思い通り動いている。つまり、誤差が原因。そしてこのtanhがgptが示したexp()を使っているのをみて、これかなと思ったらまさにそれだった。最高。
// tanh(x)を返す関数
//double tanh(double x)
  //  {return sinh(x) / cosh(x) ;}

/*
　tanh'(x)を返す関数。双曲線関数について
Using_machine_learning_to_assess_short_term_causal_dependence_and_infer_network_links.tex
　でまとめたので参照してほしい。

tanh'(x)=1-tanh^2(x)
*/
double calc::dtanh(double x)
{
    double tanh_x = std::tanh(x) ;
    return 1 - tanh_x * tanh_x ;
}



// 二つの値のうち小さいほうを返す関数。
int calc::min(int a, int b)
{
    if(a > b)
        return b ;
    else
        return a ;
}

int calc::max(int a, int b)
{
    if(a > b)
        return a ;
    else
        return b ;
}

double calc::abs(double a)
{
    if(a < 0)
        return -a ;
    else
        return a ;
}





















