#include <utility>
#include <iostream>
#include <chrono>

#include "HttpRequest.h"
#include "RequestException.h"

namespace urchin {
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    HttpRequestInitializer::HttpRequestInitializer() {
        static int callNumber = 0;
        if (++callNumber > 1) {
             throw std::runtime_error("HttpRequestInitializer constructor cannot be called several times");
        }

        curl_global_init(CURL_GLOBAL_DEFAULT); //no thread-safe: cannot be called several times
    }

    HttpRequestInitializer::~HttpRequestInitializer() {
        curl_global_cleanup();
    }

    HttpRequest::HttpRequest(std::string basePath) :
        basePath(std::move(basePath)) {
        static HttpRequestInitializer instance;

        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Impossible to initialize ");
        }

        textPlainHttpHeader = curl_slist_append(nullptr, "Content-Type: text/plain");
    }

    HttpRequest::~HttpRequest() {
        if (curl) {
            curl_easy_cleanup(curl);
        }

        if (textPlainHttpHeader) {
            curl_slist_free_all(textPlainHttpHeader);
        }
    }

    void HttpRequest::postTextPlain(const std::string &path, const std::string &value) const {
        std::string url = basePath + path;

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, textPlainHttpHeader);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

        executeRequest(url);
    }

    std::string HttpRequest::getTextPlain(const std::string &path) const {
        std::string url = basePath + path;

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        return executeRequest(url);
    }

    std::string HttpRequest::executeRequest(const std::string &url) const {
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::string error = "HTTP request fail on \"" + url + "\" : " + std::string(curl_easy_strerror(res));
            throw RequestException(error);
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode >= 300) {
                std::string error = "HTTP request fail on \"" + url + "\" with status " + std::to_string(httpCode) + ". Raw response content: " + readBuffer;
                throw RequestException(error);

            }
        }

        return readBuffer;
    }
}
