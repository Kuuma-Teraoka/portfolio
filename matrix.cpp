
// randgen::rouletteと、matrixで共依存。

#include "matrix.hpp"
#include "randgen.hpp"


// 乱数で全て埋められた行列を生成するクラス関数。コンストラクタにしないのはidentityと同じ。
matrix matrix::random(int rows, int cols, const file &rand_file, double start, double end)
{
    matrix result(rows, cols) ;
    for(int i = 0 ; i < result.shape[0] * result.shape[1] ; i++)  
        result.data[i] = randgen::fcustom(rand_file, start, end) ;
    return result ;
}

// 非ゼロ要素を乱数に書き換える関数。そもそもcsrは非ゼロ要素以外は変えられないので。
matrix::csr &matrix::csr::randomize_nonzero(const file &rand_file, double start, double end)
{
    for(int i = 0 ; i < len ; i++)
        data[i] = randgen::fcustom(rand_file, start, end) ;
    return (*this) ;
}

