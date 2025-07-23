#pragma once

#include <map>
#include <string>

#include "pattern/tag/TaggableResource.h"

namespace urchin {

    class TagHolder {
        public:
            void addTaggableResource(TaggableResource&);
            void removeTaggableResource(TaggableResource&) const;

            template<class T> void findByTag(std::string_view, std::vector<T>&) const;
            void addTag(TaggableResource*, std::string);
            void removeTag(const TaggableResource*, std::string_view);

        private:
            std::multimap<std::string, TaggableResource*, std::less<>> tagsMap;
    };

    #include "TagHolder.inl"

}
