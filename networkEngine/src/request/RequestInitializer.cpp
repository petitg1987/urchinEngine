#include <curl/curl.h>
#include <stdexcept>

#include "request/RequestInitializer.h"

namespace urchin {

    RequestInitializer::RequestInitializer() {
        static int callNumber = 0;
        if (++callNumber > 1) {
            throw std::runtime_error("RequestInitializer constructor cannot be called several times");
        }

        curl_global_init(CURL_GLOBAL_DEFAULT); //no thread-safe: cannot be called several times
    }

    RequestInitializer::~RequestInitializer() {
        curl_global_cleanup();
    }

}