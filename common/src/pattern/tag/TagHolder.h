#pragma once

#include <map>
#include <string>

#include <pattern/tag/TaggableResource.h>

namespace urchin {

    class TagHolder {
        public:
            void addTaggableResource(TaggableResource*);
            void removeTaggableResource(TaggableResource*);

            template<class T> void findByTag(const std::string&, std::vector<T>&) const;
            void addTag(TaggableResource*, const std::string&);
            void removeTag(TaggableResource*, const std::string&);

        private:
            std::multimap<std::string, TaggableResource*> tagsMap;
    };

    #include "TagHolder.inl"

}
