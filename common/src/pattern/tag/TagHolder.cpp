#include <pattern/tag/TagHolder.h>

namespace urchin {

    void TagHolder::addTaggableResource(TaggableResource* taggable) {
        taggable->setupTagHolder(this);
    }

    void TagHolder::removeTaggableResource(TaggableResource* taggable) {
        taggable->setupTagHolder(nullptr);
    }

    void TagHolder::addTag(TaggableResource* taggable, const std::string& tag) {
        tagsMap.emplace(tag, taggable);
    }

    void TagHolder::removeTag(TaggableResource* taggable, const std::string& tag) {
        for (auto it = tagsMap.begin(); it != tagsMap.end(); ) {
            if (it->second == taggable && it->first == tag) {
                it = tagsMap.erase(it);
            } else {
                ++it;
            }
        }
    }

}
