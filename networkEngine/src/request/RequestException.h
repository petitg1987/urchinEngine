#ifndef URCHINENGINE_REQUESTEXCEPTION_H
#define URCHINENGINE_REQUESTEXCEPTION_H

#include <exception>
#include <string>

namespace urchin {

    class RequestException : public std::exception {
        public:
            explicit RequestException(std::string message);

            const char* what() const noexcept override;

        private:
            std::string message;
    };

}

#endif
