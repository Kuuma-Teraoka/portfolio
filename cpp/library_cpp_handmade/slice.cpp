

#include "slice.hpp"
#include "str.hpp"




// pythonのstrings.__slice__()を作ろうとしたが、少し変更した。こっちのほうが綺麗。
slice::slice(const str &input): slice()
{ 
    std::vector<str> str_vec = input.split(":") ;
    if(str_vec.size() == 1)
    {
        start = input.int32() ;
        end = input.int32() + 1 ;
    }
    else if(str_vec.size() == 2)
    {
        start = str_vec.at(0).int32() ;
        end = str_vec.at(1).int32() ;
    }
    else if(str_vec.size() == 3)
    {
        start = str_vec.at(0).int32() ;
        end = str_vec.at(1).int32() ;
        step = str_vec.at(2).int32() ;
    }
    else
        throw std::invalid_argument("slice(str): str_vec.size() != 1, 2, 3.") ;
}

slice::slice(const char *input): slice(str(input)) {}


