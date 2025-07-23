#include "util/UserAuthorityException.h"

namespace urchin {

    UserAuthorityException::UserAuthorityException(std::string message, std::string recommendation) :
            message(std::move(message)),
            recommendation(std::move(recommendation)) {

    }

    const char* UserAuthorityException::what() const noexcept {
        return message.c_str();
    }

    const std::string& UserAuthorityException::getRecommendation() const {
        return recommendation;
    }

}
