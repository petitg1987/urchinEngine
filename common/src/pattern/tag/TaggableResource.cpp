#include <algorithm>

#include "pattern/tag/TaggableResource.h"
#include "pattern/tag/TagHolder.h"

namespace urchin {

    TaggableResource::TaggableResource() :
            tagHolder(nullptr) {

    }

    TaggableResource::~TaggableResource() {
        if (tagHolder) {
            tagHolder->removeTaggableResource(*this);
        }
    }

    void TaggableResource::setupTagHolder(TagHolder* tagHolder) {
        if (tagHolder) {
            for (const std::string& tag: tags) {
                tagHolder->addTag(this, tag);
            }
        } else {
            for (const std::string& tag: tags) {
                this->tagHolder->removeTag(this, tag);
            }
        }
        this->tagHolder = tagHolder;
    }

    const std::vector<std::string>& TaggableResource::getTags() const {
        return tags;
    }

    bool TaggableResource::hasTag(std::string_view tag) const {
        return std::ranges::find(tags, tag) != tags.end();
    }

    void TaggableResource::addTag(std::string tag) {
        if (tagHolder) {
            tagHolder->addTag(this, tag);
        }
        tags.push_back(std::move(tag));
    }

    void TaggableResource::addTags(const std::vector<std::string>& tags) {
        for (const auto& tag : tags) {
            addTag(tag);
        }
    }

    void TaggableResource::removeTag(std::string_view tag) {
        if (tagHolder) {
            tagHolder->removeTag(this, tag);
        }
        std::erase(tags, tag);
    }

    void TaggableResource::removeAllTags() {
        if (tagHolder) {
            for (const std::string& tag: tags) {
                tagHolder->removeTag(this, tag);
            }
        }
        tags.clear();
    }

}