
const char *help =
    "\n"
    "Usage: binpri [OPTIONS] MATRIX_FILE\n"
    "\n"
    "$ binpri a.bin\n"
    "　でa.binを読みこんで中身をbin::print。\n"
    "$ binpri < a.bin\n"
    "　というように引数なしなら標準出力から受け取る\n"
    "$ cat abc | binpri\n"
    "　で文字列のバイナリ確認\n" ;

#include "str.hpp"
#include "bin.hpp"
#include "file.hpp"

int main(int argc, char **argv)
{
    if(argc == 2)
    {
        const file input_file = argv[1] ;
        unsigned char *buffer = bin::malloc<unsigned char>(input_file.size()) ;
        input_file.read(buffer, input_file.size()) ;
        bin::print(buffer, input_file.size()) ;
    }
    else
        std::cout << help ;

    return 0 ;
}
