/*
　配達状況のwebページからhtmlをcurlで取得して、配達中でなくなったときにdiscordに通知を送るプログラム。

　urlは、
https://toi.kuronekoyamato.co.jp/cgi-bin/tneko?type=1&no01=623023866161&id=1901845
https://toi.kuronekoyamato.co.jp/cgi-bin/tneko?type=1&no01=437661283006&id=46ec00b
https://toi.kuronekoyamato.co.jp/cgi-bin/tneko?type=1&no01={id1}&id={id2}
　という構造。
https://member.kms.kuronekoyamato.co.jp/parcel/detail?pno=623023866161
https://member.kms.kuronekoyamato.co.jp/parcel/detail?pno=437661283006
https://member.kms.kuronekoyamato.co.jp/parcel/detail?pno={id}
　でも見られるが、curlは上手くいかなかった。

$ curl "https://toi.kuronekoyamato.co.jp/cgi-bin/tneko?type=1&no01=623023866161&id=1901845" > ./a.txt
　で配達状況のページのhtmlを取得できる。これで取得したhtmlは、
...
89: swd.writeln(' <th width="300" bgcolor="#CCCCCC"><font size="3">配達状況</font></th>');
...
95:  swd.writeln(' <td><font size="3">配達中<br></font></td>');
...
176: swd.writeln(' <td>配達中<br></td>');
...
254:      <div class="data state"><a href="#AA000" class="js-tracking-detail"><div class="data arrow sp-only">▶</div> <div class="sp-only">01/12　</div>配達中</a></div>
...
341: <div class="tracking-anchor" id="AA000"></div>
342: <div class="parts-tracking-invoice-block">
343:  <h3 class="tracking-invoice-block-title">1件目：6230-2386-6161</h3>
344:  <div class="tracking-invoice-block-message"></div>
345:  <div class="tracking-invoice-block-state is-safe-blue">
346:   <h4 class="tracking-invoice-block-state-title">配達中</h4>
347:   <div class="tracking-invoice-block-state-summary">ただいまお届けするためにお品物を持ち出しております。</div>
348:  <div class="tracking-invoice-block-state-note">お問い合わせはサービスセンターまでお願いいたします。</div>
...
　という感じなので、とりあえず、curlをcppから使って、urlからhtmlを取得して、346行に配達中の文字がないときに、配達が完了したとして、discordに通知を送る。ちなみにgptが生成したもので、curlの使い方の部分は理解していない。
　米は360だったので、行数が変わる。うむむ。


　でも見られた。こっちのほうが処理しやすいかも。curlは試してない。
*/


#include <iostream>
#include <string>
#include <curl/curl.h>
#include <vector>
#include <sstream>
#include <thread>
#include <chrono>

#include "notify.hpp"


size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb) ;
    return size * nmemb ;
}


std::string fetch_url(const char *url)
{
    CURL *curl ;
    CURLcode res ;
    std::string readBuffer ;

    curl = curl_easy_init() ;
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return readBuffer;
}


bool is_delivered(const std::string& response)
{
    std::istringstream stream(response) ;
    std::string line ;
    int line_number = 0 ;

    while(std::getline(stream, line))
    {
        ++line_number ;
        if(line_number == 360)
            return line.find("配達中") == std::string::npos ;
    }
    return false ;
}

int main()
{
    const char *url = "https://toi.kuronekoyamato.co.jp/cgi-bin/tneko?type=1&no01=437661283006&id=46ec00b" ;

    while(true)
    {
        std::cout << "checking url...\n" ;
        std::string response = fetch_url(url) ;
        if(is_delivered(response))
        {
            notify::send("Delivered!!!") ;
            break ;
        }
        std::this_thread::sleep_for(std::chrono::seconds(600)) ;
    }

    return 0;
}

