#include <iostream>
#include <chrono>
#include <thread>

#include "HttpRequest.h"

namespace urchin
{
    HttpRequestInitializer::HttpRequestInitializer()
    {
        static int callNumber = 0;
        if(++callNumber > 1)
        {
             throw std::runtime_error("HttpRequestInitializer constructor cannot be called several times");
        }

        curl_global_init(CURL_GLOBAL_ALL); //no thread-safe: cannot be called several times
    }

    HttpRequestInitializer::~HttpRequestInitializer()
    {
        curl_global_cleanup();
    }

    HttpRequest::HttpRequest(const std::string &basePath) :
        basePath(basePath)
    {
        static HttpRequestInitializer instance;

        curl = curl_easy_init();
        if(!curl)
        {
            throw std::runtime_error("Impossible to initialize ");
        }

        textPlainHttpHeader = curl_slist_append(nullptr, "Content-Type: text/plain");
    }

    HttpRequest::~HttpRequest()
    {
        if(curl)
        {
            curl_easy_cleanup(curl);
        }

        if(textPlainHttpHeader)
        {
            curl_slist_free_all(textPlainHttpHeader);
        }
    }

    void HttpRequest::postText(const std::string &path, const std::string &value)
    {
        std::string url = basePath + path;

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, textPlainHttpHeader);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

        CURLcode res = curl_easy_perform(curl);

        if(res != CURLE_OK)
        {
            std::cout<<"ERROR:"<<curl_easy_strerror(res)<<std::endl; //TODO add context...url + data
        }
    }
}
