#pragma once

#include <string>

namespace urchin {

    class Resource {
        public:
            Resource();
            Resource(const Resource&) = delete; //Resources are immutable and cannot be copied. This deletion guarantee that comparison of shared_ptr<Resource> compares identical resources.
            virtual ~Resource() = default;

            const std::string& getId() const;
            void setId(std::string);
            std::size_t retrieveHashId() const;

            const std::string& getName() const;
            void setName(std::string);

            bool isPermanent() const;
            void setPermanent(bool);

        private:
            std::string id;
            mutable std::size_t hashId;
            std::string name;
            bool permanent;
    };

}
