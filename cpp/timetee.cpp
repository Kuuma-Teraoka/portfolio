
const char *help =
    "\n"
    "Usage: timetee LOG_FILE\n"
    "Usage: timetee log.txt\n"
    "\n"
    "[2025-04-24 14:11:42.441] macbook:~$ ping 10.0.0.1\n"
    " というように、タイムスタンプ付きのteeを作った"
    "　file.writeに用いているfwriteはバッファが有効で、即時に書き込まれるわけではなく、fclose時などに書き込まれる。そのため、ctrl+cで終了するとファイルが0byteになってしまう。空行で終了するようにしたので、空行を入力すること。あるいは、echo a | timetee a.txtとしても良い。"
    "　fgetsだと一行ずつwhileが実行されるので、uriel:~$が表示されなくなってしまう。そこで、read(), write()をつかうようにしたので、ちゃんと機能する。"
    "　しかし、作った目的であるtelnetのタイムスタンプ付きlogについては上手くいかない。telnetは接続時に過去のものも表示するのと、改行コードがおかしいなどがあり、sshのようにはいかなかった。しかし、cisco routerのlogを取得したいのなら、cisco router内で補助記憶装置にlogを残すほうが本質的なので、それにした。gns3.txt参照。"
　"　改行ごとにwhileを使わなくしたので、whileで\nがあればそこでsplitして、\nのあとにタイムスタンプを入れるという挙動にする。また必要になったらこの通りに改良してくれ。"
    "\n" ; 

#include <iostream>
#include <ctime>
#include "file.hpp"
#include "str.hpp"
    
#include <iomanip>
#include <sstream>

#include <unistd.h>  // read, write

std::string timestamp()
{
    struct timespec ts ;
    struct tm tm ;

    clock_gettime(CLOCK_REALTIME, &ts) ;
    localtime_r(&ts.tv_sec, &tm) ;

    std::stringstream ss;
    ss << "["
       << (tm.tm_year + 1900) << "-" 
       << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1) << "-"
       << std::setw(2) << std::setfill('0') << tm.tm_mday << " "
       << std::setw(2) << std::setfill('0') << tm.tm_hour << ":"
       << std::setw(2) << std::setfill('0') << tm.tm_min << ":"
       << std::setw(2) << std::setfill('0') << tm.tm_sec << "."
       << std::setw(3) << std::setfill('0') << (ts.tv_nsec / 1000000)
       << "]";

    return ss.str();
}


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cout << help ;
        std::cout << "Error: main(): argc != 2\n" ;
        return 1 ;
    }

    char buf[1024] ;
    ssize_t size ;
    file output_file(argv[argc - 1], "x") ;
 
    while((size = read(0, buf, 1024)) > 0)
    {
        write(1, buf, size) ; 
        std::string time = timestamp() ;
        output_file.write(time.c_str(), str(time.c_str()).len) ;
        output_file.write(" ", 1) ;
        output_file.write(buf, str(buf).len) ; 
    }

    return 0 ;
}

