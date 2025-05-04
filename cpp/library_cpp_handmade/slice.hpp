
#ifndef _slice_hpp
#define _slice_hpp


/*
　none,none,noneのときにstepは1。startとendは、stepが正なら0,max、負ならmax-1,-1にsetする。これはmaxがないと設定できないのでコンストラクタではなく、matrixなどsliceを処理するときにset(max)で設定する。
　そして、intではnoneがない。ポインタにすればnullとint全範囲が使えるがヒープ領域に確保することになる。正に限定して-1をnoneとして使うのもいいかもしれない。唯一endはstep負で-1を与えることがあるが、これをnoneと解釈しても結局−1になるので問題ない。
*/


#include <vector>
#include <iostream>

class str ;

class slice
{
public:
    int start ;
    int end ;
    int step ; 

    // step=0だと無限ループになるのでここでエラーハンドリングしてもいいかも。
    // {}なら{-1,-1,1}, {3}なら{3,4,1}にする。
    slice(int s, int e, int step = 1): start(s), end(e), step(step) {}
    slice(): slice(-1,-1,1) {}
    slice(int s): slice(s, s+1) {}

    slice(std::initializer_list<int> args)
    {
        const int *it = args.begin() ;
        if(it != args.end())
        {
            start = *(it++) ;
            end = (it != args.end()) ? *(it++) : start + 1 ;
        }
        else
        {
            start = -1 ;
            end = (it != args.end()) ? *(it++) : -1 ;
        }
        step = (it != args.end()) ? *(it++) : 1 ; 
    }

    slice(const str &input) ;
    slice(const char *input) ;

    friend std::ostream& operator<<(std::ostream& os, const slice& slc)
    {
        os << "slice(" << slc.start << ", " << slc.end << ", " << slc.step << ")" << std::endl ; 
        return os ;
    }

    // slice.end == -1のときに、mat.cols = 4なら、4という値にしたい。これをsliceの関数でj.setend(4)とすると、end = -1のときにその値をendに書き換えるようにする。ようはsliceからは-1を4に変えることができず、sliceを呼び出したmatrix::operator[]で-1のときにshape[1]にするというようにするしかないのだ。 
    void setend(unsigned int genuine_end)
    {
        if(step > 0)
        {
            if(start == -1)
                start = 0 ;
            if(end == -1)
                end = genuine_end ; 
        }
        else if(step < 0)
        {
            if(start == -1)
                start = genuine_end - 1 ;
            if(end == -1)
                end = -1 ;
        } 
    }

    // スライスの要素数を返す関数。1,5,2なら1,3で2。1,7,3なら1,4で2。1,8,3なら1,4,7で3。end-startをstepで割って切り上げ。
    unsigned int time()
    {
        if(end == -1)
            throw std::invalid_argument("slice.time(): end == -1") ;
        if((end - start) % step == 0)
            return (end - start) / step ;
        else
            return (end - start) / step + 1 ;
    }

} ;

#endif

