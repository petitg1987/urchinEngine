#pragma once

#include <request/HttpRequest.h>
#include <request/RequestResult.h>

namespace urchin {

    class RequestExecutor {
        public:
            RequestExecutor();
            ~RequestExecutor();

            RequestResult executeRequest(const HttpRequest&, unsigned int = 0);

        private:
            unsigned int defaultRequestTimeoutSec;
            void* curl;
    };

}
