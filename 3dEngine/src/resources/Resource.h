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

            bool isPermanent() const;
            void setPermanent(bool);

        private:
            std::string id;
            std::string name;
            bool permanent;
    };

}
