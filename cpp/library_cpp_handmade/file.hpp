
#ifndef _file_h
#define _file_h

/*

 最初に使い方のメモをまとめるべき。

file output_file("a.txt") ;
char buf[100] = "adsb" ;
output_file.write(buf, str(buf).len) ;
　でa.txtにadsbと書き込まれる。



　file.writeに用いているfwriteはバッファが有効で、即時に書き込まれるわけではなく、fclose時などに書き込まれる。そのため、ctrl+cで終了するとファイルが0byteになってしまう。timetee.cpp参照。

　すべてにconstをつけている。ファイルポインタやmodeは生成後は変化しないので問題はない。なぜconstにしたかというと、random.hppでキーワード引数で、
file rand_file=file("1.bin")
　という形にしていた。しかし、これだと、rand_file引数にfileを与えて呼び出すと関数のスタック領域にfileのfpとmodeがコピーされる形になる。そしてデストラクタでfcloseが実行されてしまう。
　これを防ぐには
file &rand_file
　というように、file型引数はすべて参照型にする必要がある。そして、
file &rand_file=file("a.bin")
　とすると、
/home/lucifer/d/software/handmade/cpp/library_cpp_handmade/random.hpp:15:43: error: cannot bind non-const lvalue reference of type ‘file&’ to an rvalue of type ‘file’
   15 |     unsigned long int uli(file &rand_file=file((str(rand_dir) + str("/") + str("1.bin")).strings))
　というエラーがでてしまう。一時オブジェクトをconstでない参照型に代入、つまりバインドすることはできないらしい。

file &rand_file=file("a.bin")
　ではなく、
file &rand_file="a.bin"
　としても同じバインドエラーがでる。

　一時オブジェクトは書き換え不可能。ただしインスタンス関数による書き換えは可能。matrix("a.fmat").mul(3)という感じで、mulがconst selfでないのに呼び出せる。詳しくはcpp.txt。

const file &rand_file=file("a.bin")
　とする必要があるので、readは、read() constというようにして、const修飾子がついたインスタンスでも実行できるようにする必要がある。

　最初fseekによるファイルサイズ取得がうまくいかなかったが、ただelseの紐付けが間違っていただけで、constでもうまくいった。あと、a.txtを書き込むためにwモードで開いたFILE *fpを確保している状態で、a.txtのFILE aを確保して操作していたことも原因。ほかのファイルストリームに干渉するので注意。

　fp自体はfreadで変化しない。fpに格納されているのは、SSDのポインタではなく、メモリ上に展開されたファイルストリームのポインタである。データをよみとると、そのファイルストリーム内のデータが書き変わるだけで、ファイルストリームの場所はずっと同じ。
　FILE *fpをメンバ変数にもつstrというクラスをconst strとしても、fpの番地が変更できないだけで、fpがさすメモリは変更できる。const FILE *fpであれば、fpがさすファイルストリームFILEが格納されているメモリは変更できなくなり、fread, fwrite, fseekは使えない。
　
*/


#include <stdio.h> // FILE, fopen()
#include <iostream> // std::cout
#include <sys/stat.h>

#include "str.hpp"


class file
{
public:
    FILE *fp ;
    const str mode ;
    const str file_name ;

    class file_exists_error : public std::exception
    {
    public:
        str message ;
        file_exists_error(const str& message): message(message) {}  
        const char* what() const noexcept override {return message.strings ;}
    } ;

    file(const char *file_name, const str &mode = "r"): mode(mode), file_name(file_name)
    {
        if(mode == "r")
        {
            fp = fopen(file_name, "r") ;
            if(fp == NULL)
                throw std::ios_base::failure
                    (str("file(\"r\"): fp == NULL. mean File not found.: " + str(file_name)).strings) ;
        }
        else if(mode == "x")
        {
            if(exists(file_name))
                throw file_exists_error
                    ("file_exists_error: '" + str(file_name) + "' already exists.") ;
            fp = fopen(file_name, "w") ;
            if(fp == NULL)
                throw std::ios_base::failure
                (
                    "file::file(\"x\"): fp == NULL. "
                    "mean not able to open file. but I have no idea."
                ) ;
        }

        //std::cout << "file::file(): " << (void *)fp <<  std::endl ;
    }
    // file(str, str)だけで、str(char *)より、file(char *, char *)に対応するので十分だが、上でいちいちfopen(file_Name.strings)とするのがめんどくさいのと、char *をstrに変換してから、str.stringsにアクセスするのはすこしリソースの無駄。
    file(const str &file_name, const str &mode = "r") : file(file_name.strings, mode) {}

    ~file()
    {
        //std::cout << "file::~file(): " << (void *)fp <<  std::endl ;
        fclose(fp) ;
    }


    // pythonのos.path.exists("a.mat")の自作
    static int exists(const char *file_name)
    {
        FILE *fp = fopen(file_name, "r") ;
        if(fp != NULL)
        {   
            fclose(fp) ;
            return 1 ;
        }
        else
            return 0 ;
    }

    // pythonの環境変数を取得するos.environの自作。existsと合わせて別のファイルにすべきかも。getenv()は存在しない環境変数だと無言でNULLを返すだけなので。
    static const str environ(const char *input)
    {
        const char *result ;
        if((result = getenv(input)) == NULL)
            throw std::invalid_argument
                ("file::environ(): (result=getenv(input)) == NULL. mean ENV not found.") ;
        return result ;
    }

    static void makedir(const str input)
    {
        if(mkdir(input.strings, 0775) == 0)
            std::cout << "generated: " << input.strings << "/" << "\n" ;
        else if(errno == EEXIST)
            std::cout << input.strings << "/: is already exists\n" ;
        else
            throw std::runtime_error
                ("file::makedir(): else. not able to make dir. probably, permission.\n") ;
    }

    void write(const void *data, unsigned int size) const
    {
        if(str(mode) != str("x"))
            throw std::invalid_argument
            (
                "file::write(): str(mode) != str(\"x\"). "
                "mean you write file of read mode."
            ) ;
        if(fwrite(data, size, 1, fp) != 1)
            throw std::ios_base::failure
            (
                "file::write(): fwrite(data, size, 1, fp) != 1."
                "mean fwrite return error. I have no idea."
            ) ;
    }

    // なにか値を返すのもいいが、バイナリを返すにもなんとも。とりあえずmatrix::read()に使えればいいので、freadのエラーハンドリングだけかく。ポインタとサイズを与えると、そのポインタにファイルのバイナリを書き出す関数。
    void read(void *data, unsigned int size) const
    {
        if(str(mode) != str("r"))
            throw std::invalid_argument
            (
                "file::read(): str(mode) != str(\"r\")."
                "you shuld open file as \"r\" mode."
            ) ;
        if(fread(data, size, 1, fp) != 1)
            throw std::ios_base::failure
            (
                "file::read(): fread(data, size, 1, fp) != 1."
                "mean fread return error. maybe, file ends."
            ) ;
    }


    long int tell() const
    {
        long int result ;
        if((result = ftell(fp)) == -1)
            throw std::runtime_error("file::tell(): ftell return error code -1") ;
        return result ;
    }

    // seekをインスタンス関数にしただけ。+1したければ、input_file.seek(input_file.tell() + 1)とする。
    const file &seek(long int offset) const
    {
        if(offset < 0)
        {
            if(fseek(fp, 0, SEEK_END) < 0)
                throw std::runtime_error("file::seek(): fseek return error code -1") ; 
        }
        else
            if(fseek(fp, offset, SEEK_SET) < 0)
                throw std::runtime_error("file::seek(): fseek return error code -1") ; 
        return *this ;
    }

    // ファイルサイズを返すインスタンス関数。fseek, ftellについてはlanguage_c.txt参照。
    long int size() const
    {
        long int current = (*this).tell() ; 
        long int result = (*this).seek(-1).tell() ;
        (*this).seek(current) ;
        return result ;
    }
} ;

#endif

