#include <curl/curl.h>
#include <request/RequestExecutor.h>

namespace urchin {

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    RequestExecutor::RequestExecutor() :
            defaultRequestTimeoutSec(ConfigService::instance().getUnsignedIntValue("http-request.timeout")) {
        curl = curl_easy_init();
        if (!curl) {
            throw std::runtime_error("Impossible to initialize curl");
        }
    }

    RequestExecutor::~RequestExecutor() {
        if (curl) {
            curl_easy_cleanup(curl);
        }
    }

    RequestResult RequestExecutor::executeRequest(const HttpRequest& httpRequest, unsigned int requestTimeoutSec) {
        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, httpRequest.getUrl().c_str());
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); //avoid multi-thread error (see https://stackoverflow.com/questions/30098087/is-libcurl-really-thread-safe)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, requestTimeoutSec == 0 ? defaultRequestTimeoutSec : requestTimeoutSec);

        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        if (httpRequest.getHttpMethod() == HttpMethod::POST) {
            if (!httpRequest.getBody().empty()) {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, httpRequest.getBody().c_str());
            }
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        } else if (httpRequest.getHttpMethod() == HttpMethod::GET) {
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
        }

        struct curl_slist* curlHttpHeaders = {};
        for (const std::string& header : httpRequest.getHeaders()) {
            curlHttpHeaders = curl_slist_append(curlHttpHeaders, header.c_str());
        }
        if (httpRequest.getHttpMethod() == HttpMethod::POST) {
            curlHttpHeaders = curl_slist_append(curlHttpHeaders, "Content-Type: text/plain");
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHttpHeaders);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::string failMessage = "HTTP request fail on '" + httpRequest.getUrl() + "' : " + std::string(curl_easy_strerror(res));
            if (res == CURLE_OPERATION_TIMEDOUT /* too slow connection */ || res == CURLE_COULDNT_RESOLVE_HOST /* no connection */) {
                Logger::instance().logWarning(failMessage);
            } else {
                Logger::instance().logError(failMessage);
            }
            return RequestResult{.success = false, .responseContent = ""};
        } else {
            long httpCode;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
            if (httpCode >= 300) {
                Logger::instance().logError("HTTP request fail on '" + httpRequest.getUrl() + "' with status " + std::to_string(httpCode) + ". Raw response content: " + readBuffer);
                return RequestResult{.success = false, .responseContent = ""};
            }
        }

        Logger::instance().logInfo("Request executed on: " + httpRequest.getUrl());
        curl_slist_free_all(curlHttpHeaders);
        return RequestResult{.success = true, .responseContent = readBuffer};
    }

}
