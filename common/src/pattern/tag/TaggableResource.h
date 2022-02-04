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
            bool hasTag(std::string_view) const;
            void addTag(std::string);
            void addTags(const std::vector<std::string>&);
            void removeTag(std::string_view);
            void removeAllTags();

        private:
            TagHolder* tagHolder;
            std::vector<std::string> tags;
    };

}