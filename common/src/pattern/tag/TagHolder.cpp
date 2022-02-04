#include <pattern/tag/TagHolder.h>

namespace urchin {

    void TagHolder::addTaggableResource(TaggableResource& taggable) {
        taggable.setupTagHolder(this);
    }

    void TagHolder::removeTaggableResource(TaggableResource& taggable) const {
        taggable.setupTagHolder(nullptr);
    }

    void TagHolder::addTag(TaggableResource* taggable, std::string tag) {
        tagsMap.emplace(std::move(tag), taggable);
    }

    void TagHolder::removeTag(const TaggableResource* taggable, std::string_view tag) {
        std::erase_if(tagsMap, [&](const auto& tagPair) {
            return tagPair.second == taggable && tagPair.first == tag;
        });
    }

}
