#pragma once

#include <string>
#include <stdexcept>

namespace urchin {

    class UserAuthorityException final : public std::exception {
        public:
            UserAuthorityException(std::string, std::string);

            const char* what() const noexcept override;

            const std::string& getRecommendation() const;
        private:
            std::string message;
            std::string recommendation;
    };

}
