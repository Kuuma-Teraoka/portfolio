

#include "str.hpp"
#include "slice.hpp"



str str::operator[](slice input) const 
{
    input.setend(len) ;
    if(input.start < 0 || len < input.start || input.end < 0 || len + 1 < input.end)
        throw std::out_of_range("str::operator[](slice): out_of_range") ;
    char *result = bin::malloc<char>(input.time() + 1) ;
    int j = 0 ;
    for(int i = input.start ; i < input.end ; i += input.step)
        result[j++] = (*this)[i] ;
    result[j] = '\0' ;
    str result_str(result) ;
    free(result) ;
    return result_str ;
}


// pythonより。引数の文字列が、self中で最初に出現する位置を返す。見つからない場合は-1を返す。
// self10文字でinput4文字を探すときに、self[0,1,2,3]からinput[0,1,2,3]に一致するか探していき、self[6,7,8,9]で一致しなければ-1を返す。10-4で6個ではなく、7個で、1つずれるので注意。6,7,8,9の先頭6終わりで、7,8,9はないから-3というイメージ。 
int str::find(const str &input) const
{
    if(input.len > len)
        throw std::length_error("str::rfind(): input.len > len") ;
    for(int i = 0 ; i <= len - input.len ; i++)
        if((*this)[{i, i + input.len}] == input)
            return i ;
    return -1 ;
}

// python.txtより、split([sep, maxsplit]): 文字列を指定された区切り文字（デフォルトは空白）で分割し、リストを返します。maxsplitで分割回数を指定可能。"hello world".split() -> ["hello", "world"]
// const str &sep = " "　とできるのか。とりあえず、str sep = " "とする。
// ,区切りで、ab,cd,we,dをsplitするときにadとcd,we,dに分ける関数を再帰的に使うことで実装する。pythonでは回数を指定できるので、それを実装する。
std::vector<str> str::split(const str &sep, int maxsplit) const
{
    if(maxsplit == 0 || maxsplit == 1)
        return std::vector<str>({(*this)}) ; 
    else if(maxsplit == 2)
    {
        int index = (*this).find(sep) ;
        if(index == -1)
            throw std::invalid_argument("str::split(,2): (*this).find() == -1") ;
        return std::vector<str>({(*this)[{0, index}], (*this)[{index+1, -1}]}) ;
    } 
    else if(maxsplit < 0)
    {
        std::vector<str> result(0) ;
        try
            {result = (*this).split(sep, 2) ;}
        catch(const std::invalid_argument& e)
            {return std::vector<str>({(*this)}) ;} 
        while(1)
        {
            std::vector<str> tmp(0) ;
            try
                {tmp = (*(result.end() - 1)).split(sep, 2) ;}
            catch(const std::invalid_argument& e)
                {return result ;}
            result.pop_back() ;
            result.insert(result.end(), {tmp[0], tmp[1]}) ; 
        }
    }
    else 
        throw std::runtime_error("I dont have make it, sorry.") ;
}





