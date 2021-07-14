#pragma once

#include <string>

namespace urchin {

    class Resource {
        public:
            virtual ~Resource() = default;

            const std::string& getId() const;
            void setId(const std::string&);

            const std::string& getName() const;
            void setName(const std::string&);

        private:
            std::string id;
            std::string name;
    };

}
