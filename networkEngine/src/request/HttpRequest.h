#pragma once

#include <string>
#include <vector>
#include <curl/curl.h>

namespace urchin {

    class HttpRequest {
        public:
            explicit HttpRequest(std::string);
            ~HttpRequest();

            void postTextPlain(const std::string&, const std::string&, std::vector<std::string>) const;
            std::string getTextPlain(const std::string&, std::vector<std::string>) const;

        private:
            std::string executeRequest(const std::string&, const std::vector<std::string>&) const;

            CURL* curl;
            std::string basePath;
    };

}
