
#ifndef _random_hpp
#define _random_hpp

/*
　乱数生成にはrandfileを読み込む方法とリアルタイムでプログラムから生成する方法の2つがある。そこで、そのどちらの方法も使える乱数生成機のようなオブジェクトを作る。
randgen a("1.bin") ;
randgen a(seed=3) ;
　のようにして確保して、a.uli(), a.f01(), a.f(-1,1)というように使う。
*/

#include "file.hpp"
#include "str.hpp"
#include "matrix.hpp"

class randgen
{
public:
    static const str rand_dir ;
    static unsigned long int used ;
    file rand_file ;

    randgen(const str &file_name=str(rand_dir) + "/1.bin") : rand_file(file_name) {}

    unsigned long int uint64() const
        {return uli(rand_file) ;} 
    double float64(double start = 0, double end = 1) const
        {return fcustom(rand_file, start, end) ;}

    /*
    std::vector<double> weights = {0.1, 0.2, 0.3, 0.4}; // 各値の確率
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    std::cout << dist(gen) << " "; // 0,1,2,3を確立にしたがってランダムに返す。
    　で使える。乱数ファイルをつかって再現性を高めるためにもこれを自作する。

    　確率の配列をn行1列のmatrixで受け取って乱数を用いて抽選する関数。[0.3, 0.7]を受け取ったら30%の確率で0を返す。[0.5, 0.7, 1.5]などでも動きはするが2を返すことはない。
    */
    int roulette(matrix probabilities) const
    {
        if(probabilities.shape[1] != 1)
            throw std::invalid_argument("randgen.roulette(): probabilities.shape[1] != 1") ;
        double random = (*this).float64(0.0, 1.0) ;
        int i ;
        for(i = 0 ; random > probabilities(i, 0) && i < probabilities.shape[0] ; ++i)
            random -= probabilities(i, 0) ;
        if(i == probabilities.shape[0])
            throw std::invalid_argument("randgen.roulette(): i == probabilities.shape[0]") ;
        return i ;
    }


    // ランダムビットで構成された乱数ファイルのfileを受け取って、それをもとに[0,2^64-1]の整数乱数を返す関数。つまり、ランダムビットをもとにランダムなunsigned long intを生成する関数。
    static unsigned long int uli(const file &rand_file=file(str(rand_dir) + "/1.bin"))
    { 
        unsigned long int result ;
        rand_file.read(&result, sizeof(unsigned long int)) ; 
        used += sizeof(unsigned long int) ;
        return result ;
    }

    // ランダムビットで構成された乱数ファイルのfileを受け取って、それをもとに[0,1]の乱数を返す関数。rand_uliを使っている。
    static double f01(const file &rand_file=file(str(rand_dir) + "/1.bin"))
    {
        unsigned long int max = 0 - 1 ;
        return ((double)uli(rand_file) / max) ;
    }

    // ランダムビットで構成された乱数ファイルのポインタと二つのdouble a, bを受け取って、それをもとに[a,b]の乱数を返す関数。つまり、範囲をカスタマイズできる。
    static double fcustom
        (const file &rand_file=file(str(rand_dir) + "/1.bin"), double start=-1, double end=1)
        {return (f01(rand_file) * (end - start) + start) ;}

} ;


#endif

