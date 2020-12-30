#ifndef LASERRIDDLE_USERAUTHORITYEXCEPTION_H
#define LASERRIDDLE_USERAUTHORITYEXCEPTION_H

#include <string>
#include <stdexcept>

namespace urchin {

    class UserAuthorityException : public std::exception {
        public:
            UserAuthorityException(std::string, std::string);

            const char* what() const noexcept override;

            const std::string& getRecommendation() const;
        private:
            std::string message;
            std::string recommendation;
    };

}

#endif
