#pragma once

#include <map>
#include <string>

#include <pattern/tag/TaggableResource.h>

namespace urchin {

    class TagHolder {
        public:
            void addTaggableResource(TaggableResource&);
            void removeTaggableResource(TaggableResource&) const;

            template<class T> void findByTag(const std::string&, std::vector<T>&) const;
            void addTag(TaggableResource*, std::string);
            void removeTag(const TaggableResource*, std::string_view);

        private:
            std::multimap<std::string, TaggableResource*> tagsMap;
    };

    #include "TagHolder.inl"

}
