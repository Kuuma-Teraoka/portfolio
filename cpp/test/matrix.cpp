
#include "file.hpp"
#include "matrix.hpp"
//#include "coupled_lorenz.hpp"
#include "calc.h"
#include "network.hpp"


int main()
{ 
    // matrix &randomize_nonzero(const file &, double, double)
    // ムーブ初期化。だがstd::moveなしだとコピー初期化が使われてしまう。うむむ。
    matrix mat = matrix(3,3,1.0).randomize_nonzero() ;

    // コーピー初期化。matrix a = mat ;でも同じ。
    matrix a(mat) ;
 
    // aを書き換えても、ちゃんとmatはそのまま
    a.fill(1) ;
    std::cout << "書き換えたa:\n" << a ;
    std::cout << "aのコピーもとであるmat:\n" << mat ;


    // mul()
    std::cout << "a * mat:\n" << a * mat ;
    // 2000*1000*1000で55s。(*this)をdataアクセスにしたら46s。otherもdataアクセスにしたら40s。2000*1000*1000のループから2000のループにprogress移したら7s。
    // 5000,2000,5000。other.shape[0]をshに保存したら04:09から03:46。
    // 5000,6000,5000だとSegmentation fault。なんで。
    // 5000,2000,5000で転置して列優先を作ったら1:43。ビューでも大丈夫なように新しくmatrixをメモリに確保している。5000,6000,5000でSegmantation faultにならなくなった。なんでだろう。
    // 5000,6000,5000で4の倍数前提でforでk, k+1, k+2, k+3を計算する、ブロック化とかなんとかという方法で1:17。すごい。10にしたら00:31。まじかよ。20で、00:15。ええ。100で00:05。
    // 5000,60000,5000で10倍かと思いきや00:30。6倍。これは実用的。numpyは00:13。まあまあ。
    matrix b = matrix(5000, 60000) * matrix(60000, 5000) ;
    
    std::cout << "mat.T():\n" << mat.T()  ;
    matrix temp = mat.T() ;
    std::cout << "temp:\n" << temp ;

    std::cout << "matrix(3,3,4.0).fill(calc_tanh):\n" << matrix(3,3,4.0).fill(calc_tanh) ;


    // csr
    matrix c(3, 3, 0.0) ;
    c(0, 2) = 3 ; c(1, 0) = 4 ; c(1, 1) = 1 ; c(2, 1) = 9 ; c(2, 2) = 7 ;
    std::cout << "c:\n" << c ;
    matrix::csr sparse = c ;
    std::cout << "sparse * c:\n" << sparse * c ;

    std::cout << "c.hadamard(c):\n" << c.hadamard(c) ;

    std::cout << "matrix::identity(3):\n" << matrix::identity(3) ; 

    std::cout << "c(slice(), 0).norm:" <<  c(slice(), 0).norm() << "\n" ;

    //matrix w_x = matrix(1000, 1000, 1.0).randomize_nonzero() ;//network::random(5000, 5000 * 3) ; 
    //std::cout << "c.eig(): " << w_x.eig() << "\n" ;

    return 0 ;
}

