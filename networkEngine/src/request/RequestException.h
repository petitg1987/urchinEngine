#pragma once

#include <exception>
#include <string>

namespace urchin {

    class RequestException final : public std::exception {
        public:
            explicit RequestException(std::string message);

            const char* what() const noexcept override;

        private:
            std::string message;
    };

}
