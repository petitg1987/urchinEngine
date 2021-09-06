#pragma once

#include <string>

namespace urchin {

    class Resource {
        public:
            Resource();
            virtual ~Resource() = default;

            const std::string& getId() const;
            void setId(const std::string&);

            const std::string& getName() const;
            void setName(const std::string&);

            void resetDeathTime();
            void increaseDeathTime(float);
            float getDeathTime() const;

        private:
            std::string id;
            std::string name;
            float deathTime;
    };

}
