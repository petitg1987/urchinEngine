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

            const std::vector<std::string>& getTags();
            void addTag(const std::string&);
            void removeTag(const std::string&);

        private:
            TagHolder* tagHolder;
            std::vector<std::string> tags;
    };

}