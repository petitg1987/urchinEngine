#pragma once

#include <request/HttpRequest.h>
#include <request/RequestResult.h>

namespace urchin {

    class RequestExecutor {
        public:
            RequestExecutor();
            ~RequestExecutor();

            RequestResult executeRequest(const HttpRequest&);

        private:
            void* curl;
    };

}
