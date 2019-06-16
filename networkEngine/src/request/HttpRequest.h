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
            explicit HttpRequest(std::string );
            ~HttpRequest();

            void postTextPlain(const std::string &, const std::string &) const;
            std::string getTextPlain(const std::string &) const;

        private:
            std::string executeRequest(const std::string &) const;

            CURL *curl;
            std::string basePath;

            struct curl_slist *textPlainHttpHeader;
    };

}

#endif
