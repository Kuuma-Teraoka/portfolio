
#ifndef _notify_hpp
#define _notify_hpp

// コンパイル時に-lcurl必要。

#include <iostream>
#include <curl/curl.h>

#include "str.hpp"

namespace notify
{
    void send
    (
        const str& message,
        const str& url = "https://discord.com/api/webhooks/1246834618313736203/oR7GPyn6-eEI4AlYIK4DzD1vsVFJ9wYvDXD8gc8czInCFov-rF65LTJUbx7CqJZlXz_J"
    )
    {
        CURL* curl ;
        CURLcode res ;
        curl = curl_easy_init() ;
        if(curl)
        {
            struct curl_slist* headers = nullptr ;
            headers = curl_slist_append(headers, "Content-Type: application/json") ;

            str json_data = "{\"content\": \"" + message + "\"}" ;

            curl_easy_setopt(curl, CURLOPT_URL, url.strings) ;
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers) ;
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.strings) ;

            res = curl_easy_perform(curl) ;
            if (res != CURLE_OK)
                std::cerr << "Failed to send message:" << curl_easy_strerror(res) << "\n" ;
            else
                std::cout << "sent to Discord: \"" + message + "\"" << "\n" ;
            curl_easy_cleanup(curl) ;
            curl_slist_free_all(headers) ;
        }
        else
            std::cerr << "Failed to initialize CURL" << std::endl;
    }
}

#endif

