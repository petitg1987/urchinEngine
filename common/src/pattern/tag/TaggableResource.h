#pragma once

#include <vector>
#include <string>

namespace urchin {

    class TagHolder;

    class TaggableResource {
        public:
            TaggableResource();
            virtual ~TaggableResource();

            void setupTagHolder(TagHolder*);

            const std::vector<std::string>& getTags() const;
            void addTag(const std::string&);
            void removeTag(const std::string&);
            void removeAllTags();

        private:
            TagHolder* tagHolder;
            std::vector<std::string> tags;
    };

}