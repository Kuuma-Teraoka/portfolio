
#ifndef _progress_hpp
#define _progress_hpp

/*
　progress.py参照。メモはそっちに。
*/

#include <stdio.h> // stdout
#include <ctime> // clock_gettime()
#include <iomanip> // std::setprecision

#include "str.hpp"

class progress
{
public:
    struct timespec start_time ;
    struct timespec current_time ;
    long long int max ;
    int step ;
    str label ;
    int bar_length ;
    
    // 進捗表示ではなく時間だけ測りたいときにprogress timeとしたい。maxに適当に100を設定。 
    progress(long long int max = 100, str label = "", int step = 1000, int bar_length = 40) :  
        max(max), 
        step(step < max ? step : 0), 
        label(label), 
        bar_length(bar_length)
    {
        //std::cout << "progress()" << "\n" ;
        clock_gettime(CLOCK_REALTIME, &start_time) ;
        // カーソル非表示、doubleの固定小数点表示、小数点以下1桁
        std::cout << "\x1b[?25l" << std::fixed << std::setprecision(1) ; 
        std::cout << label << std::flush ; 
    }

    // ctrl+cで中断したときにカーソル非表示のままになるのを防ぐため。ここだけにすると、インスタンスを生成した関数が終わるまで非表示になってしまうので、updateでも非表示を戻している。
    ~progress()
    {
        //std::cout <<"~progress()" << "\n" ;
        std::cout << "\x1b[0m\x1b[?25h" << std::defaultfloat << std::setprecision(6) ;
    }

    // start_timeからの経過時間をsec単位のint32で返す関数
    int elapsed()
    {
        clock_gettime(CLOCK_REALTIME, &current_time) ;
        return (current_time.tv_sec - start_time.tv_sec) ; 
    }

    // sec単位のint32を受け取って、38:00というmin:secのstrに変換して返す関数。float64を与えてもintにキャストされる。
    str display(int input_time)
    { 
        int min = input_time / 60 ;
        int sec = input_time % 60 ; 
        return str(min / 10) + str(min % 10) + ":" + str(sec / 10) + str(sec % 10) ; 
    }

    void update(long long int current)
    {
        current += 1 ;

        if(step != 0 && current != 1 && current != max && current % (max / step) != 0)
            return ;

        double elap = (double)elapsed() ;
        if(0 < current && current < max)
        {
            int bar_num = (int)((double)current / max * bar_length) ;
            double percentage = (double)current / max * 100 ; 
            double eta = elap / current * max ; 
            std::cout
                << "\r\x1b[2K"
                << label
                << "     \x1b[38;5;198m" << str("━") * bar_num 
                << "\x1b[38;5;237m" << str("─") * (bar_length - bar_num)
                << " \x1b[38;5;37m" << percentage << "%"
                << " \x1b[38;5;28m" << str(current).comma() << "/" << str(max).comma()
                << " \x1b[38;5;28m" << (int)(current / elap) << "/s"
                << " \x1b[0m" << "eta " << "\x1b[38;5;37m" << display(elap) << "/" << display(eta)
                << "\x1b[0m" << std::flush ; 
        }
        else if(current == max)
            std::cout
                << "\r\x1b[2K"
                << label
                << "     \x1b[38;5;71m" << str("━") * bar_length
                << " \x1b[38;5;37m" << "100%"
                << " \x1b[38;5;28m" << str(current).comma()<< "/" << str(max).comma()
                << " \x1b[38;5;28m" << (int)(current / elap) << "/s"
                << " \x1b[0m" << "elapsed: " << "\x1b[38;5;37m" <<display(elap)
                << "\x1b[0m" << "\n" << "\x1b[?25h" ;
        else
            throw std::invalid_argument
                (std::format("progress::print(): current(:{}) <=0 || max(:{}) < current(:{})", current, max, current)) ;
    }
} ;

#endif

