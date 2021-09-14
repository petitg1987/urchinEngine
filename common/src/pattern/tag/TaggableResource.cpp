#include <algorithm>

#include <pattern/tag/TaggableResource.h>
#include <pattern/tag/TagHolder.h>
#include <util/VectorUtil.h>

namespace urchin {

    TaggableResource::TaggableResource() :
            tagHolder(nullptr) {

    }

    TaggableResource::~TaggableResource() {
        if (tagHolder) {
            tagHolder->removeTaggableResource(this);
        }
    }

    void TaggableResource::setupTagHolder(TagHolder* tagHolder) {
        if (tagHolder) {
            for (std::string& tag: tags) {
                tagHolder->addTag(this, tag);
            }
        } else {
            for (std::string& tag: tags) {
                this->tagHolder->removeTag(this, tag);
            }
        }
        this->tagHolder = tagHolder;
    }

    const std::vector<std::string>& TaggableResource::getTags() {
        return tags;
    }

    void TaggableResource::addTag(const std::string& tag) {
        tags.push_back(tag);

        if (tagHolder) {
            tagHolder->addTag(this, tag);
        }
    }

    void TaggableResource::removeTag(const std::string& tag) {
        tags.erase(std::remove(tags.begin(), tags.end(), tag), tags.end());

        if (tagHolder) {
            tagHolder->removeTag(this, tag);
        }
    }

}