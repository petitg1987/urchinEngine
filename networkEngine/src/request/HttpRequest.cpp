#include <utility>

#include <request/HttpRequest.h>
#include <request/RequestException.h>

namespace urchin {

    HttpRequest::HttpRequest() :
            httpMethod(HttpMethod::GET) {

    }

    HttpRequest::HttpRequest(HttpMethod httpMethod, std::string url, std::string body, std::vector<std::string> headers) :
            httpMethod(httpMethod),
            url(std::move(url)),
            body(std::move(body)),
            headers(std::move(headers)) {

    }

    HttpRequest HttpRequest::newPostTextPlain(std::string url, std::string body, std::vector<std::string> headers) {
        return {HttpMethod::POST, std::move(url), std::move(body), std::move(headers)};
    }

    HttpRequest HttpRequest::newGetTextPlain(std::string url, std::vector<std::string> headers) {
        return {HttpMethod::GET, std::move(url), "", std::move(headers)};
    }

    HttpMethod HttpRequest::getHttpMethod() const {
        return httpMethod;
    }

    const std::string& HttpRequest::getUrl() const {
        return url;
    }

    const std::string& HttpRequest::getBody() const {
        return body;
    }

    const std::vector<std::string>& HttpRequest::getHeaders() const {
        return headers;
    }

}
