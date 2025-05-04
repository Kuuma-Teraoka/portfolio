
#ifndef _infer_links_hpp
#define _infer_links_hpp

#include <vector>

#include "matrix.hpp"
#include "calc.hpp"
#include "echo_state_network.hpp"

namespace infer_links
{


    matrix stcd(const echo_state_network &model, const matrix &u, double tau=0.02)
    {
        matrix x = model.w_x * model.w_out.pinv() + model.w_in ;
        matrix sum(u.shape[0], u.shape[0], 0.0) ;
        // -1 * identityをfor前に確保。これで01:58が01:56に。-O3ありでもう最適化がされていたのかも。matrix iにしていて、for内ではこのiをつかっているつもりがカウンタのiになっていて　stcdがすべて1+2+...+60000の値になってF値が0.01になる問題があった。y_to_xのあとにこの改良をしてからonly_xをしたので、only_xだけの問題かと思ったが、こんなくだらないミスだった。ちゃんとy_to_xで1.0、only_xで0.46くらいになった。
        matrix ide = -1 * matrix::identity(u.shape[0]) ;

        progress bar(u.shape[1], "stcd():") ;
        matrix a = x * u(0) ;
        matrix b = a.fill(calc::dtanh).hadamard(x) ;
        matrix current =  model.w_out * b ; 
        
        for(int i = 1 ; i < u.shape[1] ; i++)
        {
            // 下の生成は02:26で、上の書き換えは01:58
            model.w_out.mul(x.mul(u(i), a).fill(calc::dtanh).hadamard(x, b), current) ;
            //matrix current = model.w_out * (x * u(i)).fill(calc::dtanh).hadamard(x) ;

            // これでもできる。計算式ちがうかも。
            //matrix current = (w_out * (x * z).dtanh().T()) * x ;
             
            current += ide ;
            current *= ((double)1 / tau) ;
            current.fill(calc::abs) ; 
            sum += current ;
            bar.update(i) ;
        }
        return sum * ((double)1 / u.shape[1]) ;
    }

    
    matrix bayesian(const matrix &input, int edge, int debug = 0)
    {
        matrix result(input.shape[0], input.shape[1], 0.0) ;
        if(debug)
            std::cout << input ;
        for(int edge_count = 0 ; edge_count < edge ; edge_count++)
        {
            int row_max = -1 ;
            int col_max = -1 ;
            double value_max = 0 ;
            for(int i = 0 ; i < input.shape[0] ; i++)
                for(int j = 0 ; j < input.shape[1] ; j++)
                    if(i != j && result(i, j) == 0 && input(i, j) > value_max)
                    {
                        row_max = i ;
                        col_max = j ;
                        value_max = input(i, j) ;
                    }
            if(debug)
                std::cout << row_max << ", " << col_max << ": " << value_max << "\n" ;
            if(row_max == -1 || col_max == -1)
                throw std::out_of_range
                    ("infer_links::bayesian(): row_max == -1 || col_max == -1") ;
            result(row_max, col_max) = 1 ;
        }
        return result ;
    }

    class confusion_matrix : public matrix
    {
    public:
        using matrix::matrix ;
        confusion_matrix(const matrix &truth_links, const matrix &inferred_links) : matrix(truth_links)
        {
            for(int i = 0 ; i < shape[0] ; i++)
                for(int j = 0 ; j < shape[1] ; j++)
                {
                    if(truth_links(i, j) == 0 && inferred_links(i, j) == 0)
                        (*this)(i, j) = 0 ;
                    else if(truth_links(i, j) == 1 && inferred_links(i, j) == 1)
                        (*this)(i, j) = 1 ; 
                    else if(truth_links(i, j) == 1 && inferred_links(i, j) == 0)
                        (*this)(i, j) = 2 ;
                    else if(truth_links(i, j) == 0 && inferred_links(i, j) == 1)
                        (*this)(i, j) = 3 ;
                }
        }

        matrix to_truth() const
        {
            matrix result(*this) ;
            for(int i = 0 ; i < result.shape[0] ; i++)
                for(int j = 0 ; j < result.shape[1] ; j++)
                {
                    if((*this)(i, j) == 0)
                        result(i, j) = 0 ;
                    else if((*this)(i, j) == 1)
                        result(i, j) = 1 ; 
                    else if((*this)(i, j) == 2)
                        result(i, j) = 1 ;
                    else if((*this)(i, j) == 3)
                        result(i, j) = 0 ;
                }
            return result ;
        }

        matrix to_inferred()
        {
            matrix result(*this) ;
            for(int i = 0 ; i < result.shape[0] ; i++)
                for(int j = 0 ; j < result.shape[1] ; j++)
                {
                    if((*this)(i, j) == 0)
                        result(i, j) = 0 ;
                    else if((*this)(i, j) == 1)
                        result(i, j) = 1 ; 
                    else if((*this)(i, j) == 2)
                        result(i, j) = 0 ;
                    else if((*this)(i, j) == 3)
                        result(i, j) = 1 ;
                }
            return result ;
        }

    } ;


    /*
    TN (true negative) White square
    TP (true positive) Black square
    FN (false negative) Red square
    FP (false positive) Blue square
    　つまり、
    TN: 0を0と予測
    TP: 1を1と予測
    FN: 1を0と予測
    FP: 0を1と予測
    　より、
    TN -> 0
    TP -> 1
    FN -> 2
    FP -> 3
    　とする。

    　論文本文にはconfusion matrixというワードはでてきていないが、chatGPTより、TP,TN,FP,FNの行列のことをconfusion matrixというらしい。実際、ブラウザで検索すると、TP,TN,FP,FNの図が出てくる。
    */
    /*matrix confusion_matrix(const matrix &truth_links, const matrix &inferred_links)
    {
        matrix result(truth_links) ;
        for(int i = 0 ; i < result.shape[0] ; i++)
            for(int j = 0 ; j < result.shape[1] ; j++)
            {
                if(truth_links(i, j) == 0 && inferred_links(i, j) == 0)
                    result(i, j) = 0 ;
                else if(truth_links(i, j) == 1 && inferred_links(i, j) == 1)
                    result(i, j) = 1 ; 
                else if(truth_links(i, j) == 1 && inferred_links(i, j) == 0)
                    result(i, j) = 2 ;
                else if(truth_links(i, j) == 0 && inferred_links(i, j) == 1)
                    result(i, j) = 3 ;
            }
        return result ;
    }*/

    double f_value(const matrix &input)
    {
        int tp = input.count(1.0) ;
        int fn = input.count(2.0) ;
        int fp = input.count(3.0) ;
        double f1 = (double)tp / (tp + fp) ;
        double f2 = (double)tp / (tp + fn) ;
        return 2 * f1 * f2 / (f1 + f2) ;
    }
    double accuracy(const matrix &input)
        {return (double)(input.count(0.0) + input.count(1.0)) / (input.shape[0] * input.shape[1]) ;}
    double fp_fpr(const matrix &input)
    {
        int tp = input.count(1.0) ;
        int fn = input.count(2.0) ;
        int fp = input.count(3.0) ;
        int l = tp + fn ;
        int n = input.shape[0] ;
        double fpr = l * (n * n - n - l) / (n * n - n) ;
        return fp / fpr ;
    }





    

    /*
    　時系列をうけとって、遅延によって拡張した時系列を返す関数。7を受け取ったら、(20, 60000)を(140, 60000-delay*6)にして返す。delayは自己相関を用いて求めるものと、適当に決める2パターンがある。自己相関のほうは、main()で状態変数それぞれに対して求めてvectorにまとめて関数に渡す。delayが大きい行が一番短くなるのでそれに合わせて他も短くする。delayをintでひとつ受け取ったらそれをすべての行に適応。
    */
    matrix delayed_embedding(const matrix &input, int dimension, int delay)
    {
        matrix result(input.shape[0] * dimension, input.shape[1] - delay * (dimension-1)) ;
        for(int i = 0 ; i < input.shape[0] ; i++)
            for(int j = 0 ; j < dimension ; j++)
                result({i*dimension + j}, {}) = input
                    (i, {j * delay, input.shape[1] - (dimension-1-j) * delay}) ; 
        return result ;
    }
    matrix delayed_embedding(const matrix &input, int dimension, std::vector<int> delay)
    { 
        int max = *std::max_element(delay.begin(), delay.end()) ;
        matrix result(input.shape[0] * dimension, input.shape[1] - max * (dimension-1)) ;
        for(int i = 0 ; i < input.shape[0] ; i++)
            for(int j = 0 ; j < dimension ; j++)
                result(i*dimension + j, slice()) = input
                    (i, {j * delay[i], input.shape[1] - (dimension-1-j) * delay[i]})
                    ({0, result.shape[1]}) ; // もっとも短い時系列に合わせてカット。
        return result ;
    }










}

#endif

