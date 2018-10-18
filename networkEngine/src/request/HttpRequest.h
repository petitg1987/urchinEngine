#ifndef URCHINENGINE_HTTPREQUEST_H
#define URCHINENGINE_HTTPREQUEST_H

#include <string>
#include <curl/curl.h>

namespace urchin
{

    struct HttpRequestInitializer
    {
        HttpRequestInitializer();
        ~HttpRequestInitializer();
    };

    class HttpRequest
    {
        public:
            explicit HttpRequest(const std::string &);
            ~HttpRequest();

            void postText(const std::string &, const std::string &);

        private:
            CURL *curl;
            std::string basePath;

            struct curl_slist *textPlainHttpHeader;
    };

}

#endif
