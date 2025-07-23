#pragma once

#include <string>
#include <vector>

#include "request/HttpMethod.h"

namespace urchin {

    class HttpRequest {
        public:
            HttpRequest();
            static HttpRequest newPostTextPlain(std::string, std::string, std::vector<std::string>);
            static HttpRequest newGetTextPlain(std::string, std::vector<std::string>);

            HttpMethod getHttpMethod() const;
            const std::string& getUrl() const;
            const std::string& getBody() const;
            const std::vector<std::string>& getHeaders() const;

        private:
            HttpRequest(HttpMethod, std::string, std::string, std::vector<std::string>);

            HttpMethod httpMethod;
            std::string url;
            std::string body;
            std::vector<std::string> headers;
    };

}
