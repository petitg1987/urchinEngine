#include <curl/curl.h>

#include "Request.h"

namespace urchin
{
    Request::Request()
    {
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_ALL);
    }
}
