
#ifndef _str_hpp
#define _str_hpp

/*
　61 62 63 0A 00ならlen = 4。文字列の長さを測るうえで、最後に00をいれて毎回それで数える方法と、文字列の隣にint型の文字数を書き込んでおく方法の2つがあり、ここで定義しているstrクラスは後者をとっているので、本来前者の00は必要ないのだ。ただ、C言語にはchar *を受け取る関数が多いので、それにも対応できるように00も入れておく。
　といっても、何度やってもstringsが61 62 63 00 00 00というようになるのは偶然なのか。いや、malloc(3)をstringsにいれているので偶然なはず。
　str(char *)というコンストラクタがあることで、引数がstrであればchar *を受け取っても、暗黙の型変換でstrに変換してくれる。const str &の場合も、これはstrのポインタを受け取るというものだが、呼び出し元でchar *からstrを生成してメモリに確保し、そのポインタを渡してくれる。なので、関数の引数はすべてconst str &にしておけばstrにもchar *にも対応できる。
　問題は戻り値だが、char *にしておけば、char *を引数にもつ関数に直接わたせるが、strだとわざわざ.stringsをつける必要がある。しかし、char *だとstrのインスタンス関数を呼び出すときにstr()をつける必要があるので、どっちもどっちかもしれない。とりあえず、すべてstrを返すようにする。
*/

#include <stdlib.h> // malloc()
#include <iostream> // std::cout
#include <ostream> // std::ostream
#include <cstring>
#include <vector>
#include <format>

#include "bin.hpp"

class slice ;

class str
{
public:
    char *strings ;
    int len ;
  
    str(const char *input)
    {
        len = length(input) ;
        strings = copy(input) ; 
        //std::cout << "str::str(): " << (*this) << " (" << (void *)strings << ")" <<  std::endl ;
    }
    str() : str("") {}
    str(int input) : str(std::to_string(input).c_str()) {}
    str(long long int input) : str(std::to_string(input).c_str()) {}
    str(double input) : str(std::to_string(input).c_str()) {}
    // binを受け取って"32 20 33 0A"という文字列を生成するコンストラクタ。binというクラスをつくって<<をオーバーライドしてもよかったが、こっち。
    str(const void *input, const int size): str()
    {
        (*this) = str(" ") * (size * 3 - 1) ; // 1byteあたり'0A 'というように3文字で末尾だけ2文 
        for(int i = 0 ; i < size ; i++)
        {
            strings[3 * i + 0] = std::format("{:X}", ((unsigned char *)input)[i] / 16)[0] ;
            strings[3 * i + 1] = std::format("{:X}", ((unsigned char *)input)[i] % 16)[0] ; 
        }

    }
    str(const str &input) : str(input.strings) {} // copy constructer


    ~str()
    {
        //std::cout << "str::~str(): " << (*this) << " (" << (void *)strings << ")" <<  std::endl ;
        free(strings) ;
    }


    // char *を受け取って00により長さを数えてintを返す関数。31 33 00なら2。
    static int length(const char *input)
    {
        int result = 0 ;
        for(result = 0 ; input[result] != '\0' ; result++)
            if(result > 2147483647)
                throw std::length_error("str::length(): result > 2147483647") ;
        return result ;
    } 
    // char *を受け取って、確保済みのメモリにバイナリだけコピーする関数。
    static char *copy(const char *original, char *dist)
    {
        int original_length = length(original) ;
        for(unsigned int i = 0 ; i < original_length + 1 ; i++)
            dist[i] = original[i] ;
        return dist ;
    }
    // char *を受け取って、メモリを確保してコピーしてメモリのchar *ポインタを返す関数。
    static char *copy(const char *input)
    {
        char *result = (char *)malloc(sizeof(char) * (length(input)+1)) ;
        return copy(input, result) ; 
    }


    // selfを解放して、コピーする。str相手でかつ一時オブジェクトのときはムーブ処理にしてもいいかも。
    str &operator=(const char *input)
    { 
        free(strings) ;
        len = length(input) ;
        strings = copy(input) ;
        return (*this) ;
    }
    str &operator=(const str input)
        {return ((*this) = input.strings) ;}





    // 読み込みのみ。書き換えをする場合はmatrixと同様にビューを実装する必要があり、実装がより複雑にあってしまう。とりあえず書き換えなしにする。
    // matrixでは()だが、strは1変数引数で足りるので[]をつかう。
    // [int]は1文字なのでcharを返すようにする。[slice]はstrを返す。 
    // char *で構築するか、strで構築するか悩んだが、str(int)でint分確保するというコンストラクタはいまのstr(int)であるintをstrに変換するコンストラクタと競合するので、char *で構築する。
    char operator[](int i) const
        {return strings[i] ;}
    str operator[](slice input) const ;
        // str.cpp



    // 文字列が完全に一致しているか確認する関数。str.__eq__()。
    friend int operator==(const str &str_a, const str &str_b)
    {
        if(str_a.len != str_b.len)
            return 0 ;
        for(int i = 0 ; i < str_a.len ; i++)
            if(str_a.strings[i] != str_b.strings[i])
                return 0 ;
        return 1 ;
    }
    friend int operator!=(const str &str_a, const str &str_b)
    {
        if(str_a == str_b)
            return 0 ;
        else
            return 1 ; 
    }



    // 31 33 00, 34 35 36 20 00を受け取ったら、str_lenをもとに2+4+1[bytes]を確保して、31 33 34 35 36 20 00を入れて返す関数。char同士の足し算はオーバーロードできないので注意。
    // charを返すようにすると、ヒープ領域にmallocで確保されたchar *が解放されない。strでラップして返したほうが、呼び出し元で一時オブジェクトとして使った後にデストラクタで自動解放できるので使い勝手がいい。(str("a") + str("b")).stringsでcharを参照できるし。
    friend str operator+(const str &input_a, const str &input_b)
    {
        char *result = (char *)malloc(sizeof(char) * (input_a.len + input_b.len + 1)) ;
        copy(input_a.strings, result ) ;
        copy(input_b.strings, result + input_a.len) ;  
        return str(result) ;
    }
    friend str operator+(const str &input_a, const char *input_b)
        {return input_a + str(input_b) ;}
    friend str operator+(const char *input_a, const str &input_b)
        {return str(input_a) + input_b ;}

    friend str operator*(const str &input_a, const int input_b)
    {
        if(input_b <= 0)
            return str("") ;
        char *result = (char *)malloc(sizeof(char) * (input_a.len * input_b + 1)) ; 
        for(int i = 0 ; i < input_b ; i++)
            copy(input_a.strings, result + i*input_a.len) ;
 
        return str(result) ; 
    }


    // なぜpythonのstr.__str__()が改行を\nとして標準出力するかわかった。デバッグだと改行が邪魔になる。''で囲って、\nを\\nとしてただの\nという文字にした文字列を表示する。''で囲うのやめた
    friend std::ostream& operator<<(std::ostream& os, const str& input)
    {
        for(int i = 0 ; i < input.len ; i++)
            if(input[i] == '\n')
                os << "\\n" ;
            else
                os << input[i] ;
        return os ;
    }


    // inputで始まっていれば1、でなければ0を返す関数。pythonより。
    int startwith(const str &input) const
    {
        if(input.len > len)
            throw std::invalid_argument("str::startwith(): input.len > len") ;
        for(int i = 0 ; i < input.len ; i++)
            if((*this)[i] != input[i])
                return 0 ;
        return 1 ;
    }
    // "12359279"というインスタンスから、"12,359,279"という新しいインスタンスを生成して返す関数
    str comma() const
    {
        str result = str((*this).strings) ;
        return result ;
    }
    /*str comma() const
    {
        int is_negative = (*this).startwith("-") ;
        str number ;
        if(is_negative)
            number = (*this)[{1,-1}] ;
        else
            number = (*this)[{len-1,  ;

        number = number



        
    }*/
    
    /*
    　pythonの"tmp.mat".endswith(".mat")の自作。.matで終わる場合は1、終わらない場合は0を返す。 
    　str(".mat").lenは4で、str("tmp.mat").lenは7。str("tmp.mat").strings[3,4,5,6]とstr(".mat"J).strings[0,1,2,3]で一致していればいい。
    　suffix.len > (*this).lenだと、(*this).strings[-1]とかにアクセスしてしまうのでエラーハンドリングも忘れずに。
    */
    int endswith(const str &suffix) const
    {
        if(suffix.len > len)
            throw std::length_error("str::endswith(): suffix.len > len") ;
        for(int i = 0 ; i < suffix.len ; i++)
            if(strings[i + len - suffix.len] != suffix.strings[i])
                return 0 ;
        return 1 ;
    }





    /* 
    　slice(str)用。pythonのstrings.__slice__()参照。findとsplitは自作して、listはstd::vector<str>にする。あとは、pythonと同様にstd::vector<str>をsliceにするだけ。
    　initilizer<int>にしてもいいし、std::vector<int>にしてもいいし。そしてmatpriつくって、gnuplotつくったら完成。
    */


    int find(const str &input) const ; 
    std::vector<str> split(const str &sep, int maxsplit = -1) const ; 





    

    // 31 32のstrから、12を返す関数。検知機能はなし。31 32 0Aなら0A-30=-26=-38(10進数)で、120+(-38)=82になる。実験済み。unsigned_int.__init__()。まあすべてのオブジェクトで__str__を設定することから、strで__ui__を設定するのも間違いではないので、str.__ui__()。
    unsigned int ui() const
    {
        unsigned int sum = 0 ;
        for(unsigned int i = 0 ; i < len ; i++)
            sum = sum * 10 + (strings[i] - '0') ;
        return sum ;
    }

    // 2D(-) 31 32から、-12を返す関数。1バイト目が-かどうかで全体に-をかけるだけで、str_to_uiと同じ。つまり、2D 31 32 0Aなら-82になる。実験済み。int.__init__()、str.__int__()。
    int int32() const
    {
        int sign = 1 ;
        int sum = 0 ;
        unsigned int i = 0 ;
        if(strings[0] == '-')
        {
            sign = -1 ;
            i = 1 ;
        }
        for( ; i < len ; i++)
            sum = sum * 10 + (strings[i] - '0') ;
        return (sign * sum) ;
    }

    // 文字列を受け取って、NULLまでのバイナリをdoubleに入れる関数。jあたりがややこしく、0-1、つまりMAXで初期化しておいて、.が来たら0にする。MAXでなくなったら数字を読み込むたびに+1する。最後にj回10で割る。つまりjにカウンタとフラッグを兼任させている。.が来なければj=MAXのままで、その場合は10で割らない。jが使える範囲が[0, MAX]から[0, MAX-1]になるが問題ないだろう。float64.__init__()、str.__float64__()。
    // commaの位置までを抜き取って、str().int32でint求めて、としてもいいかも。
    double float64() const
    {
        int sign = 1 ;
        double sum = 0 ;
        unsigned int j = 0 - 1 ;
        if(strings[0] == '-')
            sign = -1 ;
        for(unsigned int i = 0 ; i < len ; i++)
        {
            if(strings[i] == '.')
                j = 0 ;
            else
            {
                sum = sum * 10 + (strings[i] - '0') ;
                if(j != 0 - 1)
                    j++ ;
            }
        }

        if(j != 0 - 1)
            for(unsigned int i = 0 ; i < j ; i++)
                sum /= 10 ;

        return sign * sum ;
    }
} ;

#endif

