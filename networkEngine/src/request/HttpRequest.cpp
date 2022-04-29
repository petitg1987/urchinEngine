#include <utility>
#include <iostream>
#include <chrono>
#include <UrchinCommon.h>

#include <request/HttpRequest.h>
#include <request/RequestException.h>

namespace urchin {
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    HttpRequest::HttpRequest(std::string basePath) :
            basePath(std::move(basePath)) {
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Impossible to initialize curl");
        }
    }

    HttpRequest::~HttpRequest() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    void HttpRequest::postTextPlain(const std::string& path, const std::string& body, std::vector<std::string> headers) const {
        std::string url = basePath + path;
        headers.emplace_back("Content-Type: text/plain");

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

        executeRequest(url, headers);
    }

    std::string HttpRequest::getTextPlain(const std::string& path, std::vector<std::string> headers) const {
        std::string url = basePath + path;
        headers.emplace_back("Content-Type: text/plain");

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");

        return executeRequest(url, headers);
    }

    std::string HttpRequest::executeRequest(const std::string& url, const std::vector<std::string>& headers) const {
        struct curl_slist* curlHttpHeaders = {};
        for (auto& header : headers) {
            curlHttpHeaders = curl_slist_append(curlHttpHeaders, header.c_str());
        }

        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHttpHeaders);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            throw RequestException("HTTP request fail on \"" + url + "\" : " + std::string(curl_easy_strerror(res)));
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode >= 300) {
                throw RequestException("HTTP request fail on \"" + url + "\" with status " + std::to_string(httpCode) + ". Raw response content: " + readBuffer);
            }
        }

        Logger::instance().logInfo("Request executed on: " + url);

        curl_slist_free_all(curlHttpHeaders);
        return readBuffer;
    }
}
