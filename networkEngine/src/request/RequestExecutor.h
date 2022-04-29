#pragma once

#include <request/HttpRequest.h>

namespace urchin {

    class RequestExecutor {
        public:
            void executeRequest(const HttpRequest&);
    };

}
