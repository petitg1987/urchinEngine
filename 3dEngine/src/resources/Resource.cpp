#include "resources/Resource.h"

namespace urchin {

    Resource::Resource() :
            hashId(0),
            permanent(false) {

    }

    const std::string& Resource::getId() const {
        return id;
    }

    void Resource::setId(std::string id) {
        this->id = std::move(id);
    }

    std::size_t Resource::retrieveHashId() const {
        if (id.empty()) {
            throw std::runtime_error("No resource id defined on resource named: " + name);
        }
        if (hashId == 0) {
            hashId = std::hash<std::string>{}(id);
        }
        return hashId;
    }

    const std::string& Resource::getName() const {
        return name;
    }

    void Resource::setName(std::string name) {
        this->name = std::move(name);
    }

    bool Resource::isPermanent() const {
        return permanent;
    }

    void Resource::setPermanent(bool permanent) {
        this->permanent = permanent;
    }

}
